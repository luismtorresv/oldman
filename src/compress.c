#include "compress.h"
#include "file_ops.h"
#include <stdlib.h>
#include <string.h>

/**
 * Compresses a buffer using Run-Length Encoding.
 * Format: [count][byte] where count is 1 byte (max 255 repetitions).
 * For non-repeating sequences, count of 0 indicates literal bytes follow.
 */
ssize_t rle_compress(const unsigned char *input, size_t input_size,
                     unsigned char *output, size_t output_size) {
    if (!input || !output || input_size == 0) {
        return -1;
    }

    size_t out_pos = 0;
    size_t i = 0;

    while (i < input_size) {
        unsigned char current = input[i];
        size_t count = 1;

        while (i + count < input_size && input[i + count] == current && count < 255) {
            count++;
        }

        if (out_pos + 2 > output_size) {
            return -1;
        }

        if (count >= 3) {
            output[out_pos++] = (unsigned char)count;
            output[out_pos++] = current;
            i += count;
        } else {
            size_t literal_start = i;
            size_t literal_count = 0;

            while (i < input_size && literal_count < 255) {
                size_t run_len = 1;
                while (i + run_len < input_size &&
                       input[i + run_len] == input[i] &&
                       run_len < 3) {
                    run_len++;
                }

                if (run_len >= 3) {
                    break;
                }

                i++;
                literal_count++;
            }

            if (out_pos + 1 + literal_count > output_size) {
                return -1;
            }

            output[out_pos++] = 0;
            output[out_pos++] = (unsigned char)literal_count;
            memcpy(output + out_pos, input + literal_start, literal_count);
            out_pos += literal_count;
        }
    }

    return out_pos;
}

/**
 * Decompresses a buffer using Run-Length Encoding.
 */
ssize_t rle_decompress(const unsigned char *input, size_t input_size,
                       unsigned char *output, size_t output_size) {
    if (!input || !output || input_size == 0) {
        return -1;
    }

    size_t in_pos = 0;
    size_t out_pos = 0;

    while (in_pos < input_size) {
        unsigned char count = input[in_pos++];

        if (count == 0) {
            if (in_pos >= input_size) {
                return -1;
            }

            unsigned char literal_count = input[in_pos++];

            if (in_pos + literal_count > input_size ||
                out_pos + literal_count > output_size) {
                return -1;
            }

            memcpy(output + out_pos, input + in_pos, literal_count);
            in_pos += literal_count;
            out_pos += literal_count;
        } else {
            if (in_pos >= input_size || out_pos + count > output_size) {
                return -1;
            }

            unsigned char byte = input[in_pos++];
            memset(output + out_pos, byte, count);
            out_pos += count;
        }
    }

    return out_pos;
}

/**
 * Compresses a file using RLE algorithm.
 */
int compress_file(const char *input_path, const char *output_path) {
    unsigned char *input_buffer = NULL;
    unsigned char *output_buffer = NULL;
    ssize_t input_size;
    ssize_t output_size;
    int result = -1;

    input_size = read_file_syscall(input_path, &input_buffer);
    if (input_size < 0) {
        goto cleanup;
    }

    output_buffer = malloc(input_size * 2 + 1024);
    if (!output_buffer) {
        goto cleanup;
    }

    output_size = rle_compress(input_buffer, input_size,
                               output_buffer, input_size * 2 + 1024);
    if (output_size < 0) {
        goto cleanup;
    }

    if (write_file_syscall(output_path, output_buffer, output_size) != 0) {
        goto cleanup;
    }

    result = 0;

cleanup:
    free(input_buffer);
    free(output_buffer);
    return result;
}

/**
 * Decompresses a file using RLE algorithm.
 */
int decompress_file(const char *input_path, const char *output_path) {
    unsigned char *input_buffer = NULL;
    unsigned char *output_buffer = NULL;
    ssize_t input_size;
    ssize_t output_size;
    int result = -1;

    input_size = read_file_syscall(input_path, &input_buffer);
    if (input_size < 0) {
        goto cleanup;
    }

    output_buffer = malloc(input_size * 256);
    if (!output_buffer) {
        goto cleanup;
    }

    output_size = rle_decompress(input_buffer, input_size,
                                 output_buffer, input_size * 256);
    if (output_size < 0) {
        goto cleanup;
    }

    if (write_file_syscall(output_path, output_buffer, output_size) != 0) {
        goto cleanup;
    }

    result = 0;

cleanup:
    free(input_buffer);
    free(output_buffer);
    return result;
}
