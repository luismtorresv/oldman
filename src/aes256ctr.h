#ifndef AES256CTR_H
#define AES256CTR_H
#include <stdint.h>
#include <stddef.h>
typedef struct { uint32_t rk[60]; int nr; unsigned char ctr[16]; } aes256ctr_t;
void aes256ctr_init(aes256ctr_t* ctx, const unsigned char key[32], const unsigned char noncectr[16]);
void aes256ctr_xor(aes256ctr_t* ctx, unsigned char* buf, size_t len);
#endif
