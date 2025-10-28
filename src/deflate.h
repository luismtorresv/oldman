#ifndef DEFLATE_H
#define DEFLATE_H
#include <stddef.h>
#include <stdint.h>

#include "bitio.h"
#include "lz77.h"
size_t deflate_fixed_block(const lz77_item_t* items, size_t nitems,
                           unsigned char* outbuf, size_t outcap);
int deflate_len_to_code(int L, int* extra_bits, int* extra_val);
int deflate_dist_to_code(int D, int* extra_bits, int* extra_val);
#endif
