#include "crc32.h"
static uint32_t table[256];
static int table_init = 0;
static void crc32_init(void) {
  for (uint32_t i = 0; i < 256; i++) {
    uint32_t c = i;
    for (int j = 0; j < 8; j++) {
      c = (c & 1) ? (0xEDB88320u ^ (c >> 1)) : (c >> 1);
    }
    table[i] = c;
  }
  table_init = 1;
}
uint32_t crc32_update(uint32_t crc, const unsigned char* buf, size_t len) {
  if (!table_init) crc32_init();
  crc ^= 0xFFFFFFFFu;
  for (size_t i = 0; i < len; i++) {
    crc = table[(crc ^ buf[i]) & 0xFF] ^ (crc >> 8);
  }
  return crc ^ 0xFFFFFFFFu;
}
