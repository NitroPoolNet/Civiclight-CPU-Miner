#include "simd-utils.h"

#if defined(SIMD512)

const __m512i V512_BSWAP64 =     { 0x0001020304050607, 0x08090a0b0c0d0e0f,
                                   0x0001020304050607, 0x08090a0b0c0d0e0f,
                                   0x0001020304050607, 0x08090a0b0c0d0e0f,
                                   0x0001020304050607, 0x08090a0b0c0d0e0f };

const __m512i V512_BSWAP32 =     { 0x0405060700010203, 0x0c0d0e0f08090a0b,
                                   0x0405060700010203, 0x0c0d0e0f08090a0b,
                                   0x0405060700010203, 0x0c0d0e0f08090a0b,
                                   0x0405060700010203, 0x0c0d0e0f08090a0b };

#endif

/*
 * Always emit constants for every vector width supported by the compiler
 * target. Some source files use narrower GCC target attributes even when the
 * overall binary is built for AVX2 or AVX-512. Those objects require real
 * V128/V256 symbols rather than the cast macros exposed to the wider target.
 */
#if defined(__AVX2__)

#ifdef V256_BSWAP64
#undef V256_BSWAP64
#undef V256_BSWAP32
#endif

const __m256i V256_BSWAP64     = { 0x0001020304050607, 0x08090a0b0c0d0e0f,
                                   0x0001020304050607, 0x08090a0b0c0d0e0f };

const __m256i V256_BSWAP32     = { 0x0405060700010203, 0x0c0d0e0f08090a0b,
                                   0x0405060700010203, 0x0c0d0e0f08090a0b };

const __m256i V256_SHUFLR64_8  = { 0x0007060504030201, 0x080f0e0d0c0b0a09,
                                   0x0007060504030201, 0x080f0e0d0c0b0a09 };

const __m256i V256_SHUFLR64_24 = { 0x0201000706050403, 0x0a09080f0e0d0c0b,
                                   0x0201000706050403, 0x0a09080f0e0d0c0b };

const __m256i V256_SHUFLL64_8  = { 0x0605040302010007, 0x0e0d0c0b0a09080f,
                                   0x0605040302010007, 0x0e0d0c0b0a09080f };

const __m256i V256_SHUFLL64_24 = { 0x0403020100070605, 0x0c0b0a09080f0e0d,
                                   0x0403020100070605, 0x0c0b0a09080f0e0d };

const __m256i V256_SHUFLR32_8  = { 0x0407060500030201, 0x0c0f0e0d080b0a09,
                                   0x0407060500030201, 0x0c0f0e0d080b0a09 };

const __m256i V256_SHUFLL32_8  = { 0x0605040702010003, 0x0e0d0c0f0a09080b,
                                   0x0605040702010003, 0x0e0d0c0f0a09080b };

#endif

#if defined(__SSSE3__)

#ifdef V128_BSWAP64
#undef V128_BSWAP64
#undef V128_BSWAP32
#undef V128_SHUFLR64_8
#undef V128_SHUFLR64_24
#undef V128_SHUFLL64_8
#undef V128_SHUFLL64_24
#undef V128_SHUFLR32_8
#undef V128_SHUFLL32_8
#endif

const v128_t V128_BSWAP64      = { 0x0001020304050607, 0x08090a0b0c0d0e0f };
const v128_t V128_BSWAP32      = { 0x0405060700010203, 0x0c0d0e0f08090a0b };

const v128_t V128_SHUFLR64_8   = { 0x0007060504030201, 0x080f0e0d0c0b0a09 };
const v128_t V128_SHUFLR64_24  = { 0x0201000706050403, 0x0a09080f0e0d0c0b };
const v128_t V128_SHUFLL64_8   = { 0x0605040302010007, 0x0e0d0c0b0a09080f };
const v128_t V128_SHUFLL64_24  = { 0x0403020100070605, 0x0c0b0a09080f0e0d };

const v128_t V128_SHUFLR32_8   = { 0x0407060500030201, 0x0c0f0e0d080b0a09 };
const v128_t V128_SHUFLL32_8   = { 0x0605040702010003, 0x0e0d0c0f0a09080b };

#endif
