#ifndef GZIP_H
#define GZIP_H
#include <stddef.h>
#include <stdint.h>
size_t gzip_wrap(const unsigned char* deflate_data, size_t deflate_len,
                 const unsigned char* uncompressed, size_t uncompressed_len,
                 unsigned char* out, size_t outcap);
size_t gzip_compress_fixed(const unsigned char* in, size_t n,
                           unsigned char* out, size_t outcap);
size_t gzip_decompress_fixed(const unsigned char* in, size_t n,
                             unsigned char* out, size_t outcap);
#endif
