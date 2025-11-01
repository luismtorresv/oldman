#include "encrypt.h"
#include "file_ops.h"
#include <stdlib.h>
#include <string.h>

/**
 * Encrypts a buffer using Vigenere cipher.
 * Each byte is shifted by the corresponding key byte.
 */
void vigenere_encrypt(const unsigned char *input, size_t size,
                      unsigned char *output, const char *key) {
    if (!input || !output || !key) {
        return;
    }

    size_t key_len = strlen(key);
    if (key_len == 0) {
        return;
    }

    for (size_t i = 0; i < size; i++) {
        output[i] = input[i] + (unsigned char)key[i % key_len];
    }
}

/**
 * Decrypts a buffer using Vigenere cipher.
 * Each byte is shifted back by the corresponding key byte.
 */
void vigenere_decrypt(const unsigned char *input, size_t size,
                      unsigned char *output, const char *key) {
    if (!input || !output || !key) {
        return;
    }

    size_t key_len = strlen(key);
    if (key_len == 0) {
        return;
    }

    for (size_t i = 0; i < size; i++) {
        output[i] = input[i] - (unsigned char)key[i % key_len];
    }
}

/**
 * Encrypts a file using Vigenere cipher.
 */
int encrypt_file(const char *input_path, const char *output_path) {
    unsigned char *input_buffer = NULL;
    unsigned char *output_buffer = NULL;
    ssize_t file_size;
    int result = -1;

    file_size = read_file_syscall(input_path, &input_buffer);
    if (file_size < 0) {
        goto cleanup;
    }

    output_buffer = malloc(file_size);
    if (!output_buffer) {
        goto cleanup;
    }

    vigenere_encrypt(input_buffer, file_size, output_buffer, DEFAULT_KEY);

    if (write_file_syscall(output_path, output_buffer, file_size) != 0) {
        goto cleanup;
    }

    result = 0;

cleanup:
    free(input_buffer);
    free(output_buffer);
    return result;
}

/**
 * Decrypts a file using Vigenere cipher.
 */
int decrypt_file(const char *input_path, const char *output_path) {
    unsigned char *input_buffer = NULL;
    unsigned char *output_buffer = NULL;
    ssize_t file_size;
    int result = -1;

    file_size = read_file_syscall(input_path, &input_buffer);
    if (file_size < 0) {
        goto cleanup;
    }

    output_buffer = malloc(file_size);
    if (!output_buffer) {
        goto cleanup;
    }

    vigenere_decrypt(input_buffer, file_size, output_buffer, DEFAULT_KEY);

    if (write_file_syscall(output_path, output_buffer, file_size) != 0) {
        goto cleanup;
    }

    result = 0;

cleanup:
    free(input_buffer);
    free(output_buffer);
    return result;
}
