#include "civiclight.h"
#include "../sha/sha256-hash.h"
#include "yespower/yespower.h"
#include <string.h>
#include <stdint.h>

// Fork activation: block headers with nTime before this use the original
// algorithm; at/after this, they use civiclight v2 (yespower-based).
static const uint32_t CIVICLIGHT_V2_ACTIVATION_TIME = 1784797200;

static const civic_yespower_params_t CIVICLIGHT_YESPOWER_PARAMS = {
   YESPOWER_1_0,
   2048,
   8,
   NULL,
   0
};

static void sha256d_local( void *output, const void *input, size_t len )
{
   uint8_t h[32];
   sha256_full( h, input, len );
   sha256_full( output, h, 32 );
}

// ---- v1: original algorithm ----
static void civiclight_core_v1( void *output, const void *input, size_t len )
{
   uint8_t hash1[32];
   sha256_full( hash1, input, len );
   for ( int i = 0; i < 32; i++ )
      hash1[i] ^= 0x5A;
   sha256_full( output, hash1, 32 );
}

// ---- v2: ASIC-resistant algorithm (SHA256 -> yespower -> XOR -> SHA256) ----
static void civiclight_core_v2( void *output, const void *input, size_t len )
{
   uint8_t hash1[32];
   uint8_t xor_buf[32];
   sha256_full( hash1, input, len );

   civic_yespower_binary_t yp_out;
   if ( civic_yespower_tls( hash1, sizeof(hash1),
                            &CIVICLIGHT_YESPOWER_PARAMS, &yp_out ) != 0 )
   {
      // Never allow an allocation failure to produce a candidate share.
      memset( output, 0xff, 32 );
      return;
   }

   for ( int i = 0; i < 32; i++ )
      xor_buf[i] = yp_out.uc[i] ^ hash1[i];

   sha256_full( output, xor_buf, 32 );
}

// Extract nTime from raw 80-byte block header (bytes 68-71, little-endian)
static uint32_t extract_ntime( const void *header80 )
{
   const uint8_t *b = (const uint8_t*)header80;
   return (uint32_t)b[68] | ((uint32_t)b[69] << 8) |
          ((uint32_t)b[70] << 16) | ((uint32_t)b[71] << 24);
}

static void civiclight_powhash( void *output, const void *header80,
                                bool force_v2 )
{
   uint8_t intermediate[32];
   sha256d_local( intermediate, header80, 80 );

   uint32_t ntime = extract_ntime( header80 );
   if ( force_v2 || ntime >= CIVICLIGHT_V2_ACTIVATION_TIME )
      civiclight_core_v2( output, intermediate, 32 );
   else
      civiclight_core_v1( output, intermediate, 32 );
}

int civiclight_hash( void *output, const void *input, int thr_id )
{
   civiclight_powhash( output, input, false );
   return 1;
}

int scanhash_civiclight( struct work *work, uint32_t max_nonce,
                          uint64_t *hashes_done, struct thr_info *mythr )
{
   uint32_t edata[20] __attribute__((aligned(64)));
   uint32_t hash[8] __attribute__((aligned(64)));
   uint32_t *pdata = work->data;
   uint32_t *ptarget = work->target;
   const uint32_t first_nonce = pdata[19];
   const uint32_t last_nonce = max_nonce - 1;
   uint32_t n = first_nonce;
   const int thr_id = mythr->id;
   const bool bench = opt_benchmark;
   v128_bswap32_80( edata, pdata );
   do
   {
      edata[19] = n;
      // Benchmark work is zero-initialized, including nTime. Force the current
      // v2 path so benchmark results measure the live yespower algorithm.
      civiclight_powhash( hash, edata, bench );
      if ( unlikely( valid_hash( hash, ptarget ) && !bench ) )
      {
         pdata[19] = bswap_32( n );
         submit_solution( work, hash, mythr );
      }
      n++;
   } while ( n < last_nonce && !work_restart[thr_id].restart );
   *hashes_done = n - first_nonce;
   pdata[19] = n;
   return 0;
}

bool register_civiclight_algo( algo_gate_t* gate )
{
   gate->optimizations = SSE2_OPT | AVX2_OPT | AVX512_OPT | NEON_OPT;
   gate->scanhash       = (void*)&scanhash_civiclight;
   gate->hash           = (void*)&civiclight_hash;
   return true;
};
