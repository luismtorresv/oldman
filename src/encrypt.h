#ifndef ENCRYPT_H
#define ENCRYPT_H

#include <stddef.h>
#include <sys/types.h>

#define DEFAULT_KEY "gsea2025"

/**
 * Encrypts a file using Vigenere cipher.
 * Returns 0 on success, -1 on failure.
 */
int encrypt_file(const char *input_path, const char *output_path);

/**
 * Decrypts a file using Vigenere cipher.
 * Returns 0 on success, -1 on failure.
 */
int decrypt_file(const char *input_path, const char *output_path);

/**
 * Encrypts a buffer using Vigenere cipher.
 */
void vigenere_encrypt(const unsigned char *input, size_t size,
                      unsigned char *output, const char *key);

/**
 * Decrypts a buffer using Vigenere cipher.
 */
void vigenere_decrypt(const unsigned char *input, size_t size,
                      unsigned char *output, const char *key);

#endif
