#ifndef LZ77_H
#define LZ77_H
#include <stddef.h>
#include <stdint.h>
typedef struct { size_t pos; int len; int dist; unsigned char lit; } lz77_item_t;
size_t lz77_greedy(const unsigned char* in, size_t n, lz77_item_t* out);
#endif
