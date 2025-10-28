#ifndef INFLATE_H
#define INFLATE_H
#include <stddef.h>
#include <stdint.h>

#include "bitio.h"
size_t inflate_fixed_block(bitreader_t* br, unsigned char* out, size_t outcap);
#endif
