#ifndef COMPRESS_H
#define COMPRESS_H

#include <stddef.h>
#include <sys/types.h>

/**
 * Compresses a file using RLE algorithm.
 * Returns 0 on success, -1 on failure.
 */
int compress_file(const char *input_path, const char *output_path);

/**
 * Decompresses a file using RLE algorithm.
 * Returns 0 on success, -1 on failure.
 */
int decompress_file(const char *input_path, const char *output_path);

/**
 * Compresses a buffer using RLE algorithm.
 * Returns the size of compressed data, or -1 on failure.
 */
ssize_t rle_compress(const unsigned char *input, size_t input_size,
                     unsigned char *output, size_t output_size);

/**
 * Decompresses a buffer using RLE algorithm.
 * Returns the size of decompressed data, or -1 on failure.
 */
ssize_t rle_decompress(const unsigned char *input, size_t input_size,
                       unsigned char *output, size_t output_size);

#endif
