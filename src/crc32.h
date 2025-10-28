#ifndef CRC32_H
#define CRC32_H
#include <stdint.h>
#include <stddef.h>
uint32_t crc32_update(uint32_t crc, const unsigned char* buf, size_t len);
#endif
