#ifndef __CIVICLIGHT_H__
#define __CIVICLIGHT_H__ 1

#include <stdint.h>
#include "algo-gate-api.h"

int civiclight_hash( void *output, const void *input, int thr_id );
bool register_civiclight_algo( algo_gate_t* gate );

#endif
