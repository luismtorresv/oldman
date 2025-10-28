#include "gzip.h"

#include <stdlib.h>
#include <string.h>

#include "crc32.h"
#include "deflate.h"
#include "lz77.h"
size_t gzip_wrap(const unsigned char* deflate_data, size_t deflate_len,
                 const unsigned char* uncompressed, size_t uncompressed_len,
                 unsigned char* out, size_t outcap) {
  if (outcap < deflate_len + 18) return 0;
  size_t pos = 0;
  out[pos++] = 0x1f;
  out[pos++] = 0x8b;
  out[pos++] = 8;
  out[pos++] = 0;
  out[pos++] = 0;
  out[pos++] = 0;
  out[pos++] = 0;
  out[pos++] = 0;
  out[pos++] = 0;
  out[pos++] = 255;
  memcpy(out + pos, deflate_data, deflate_len);
  pos += deflate_len;
  uint32_t crc = crc32_update(0xFFFFFFFFu, uncompressed, uncompressed_len);
  uint32_t isize = (uint32_t)(uncompressed_len & 0xFFFFFFFFu);
  out[pos++] = (unsigned char)(crc & 0xFF);
  out[pos++] = (unsigned char)((crc >> 8) & 0xFF);
  out[pos++] = (unsigned char)((crc >> 16) & 0xFF);
  out[pos++] = (unsigned char)((crc >> 24) & 0xFF);
  out[pos++] = (unsigned char)(isize & 0xFF);
  out[pos++] = (unsigned char)((isize >> 8) & 0xFF);
  out[pos++] = (unsigned char)((isize >> 16) & 0xFF);
  out[pos++] = (unsigned char)((isize >> 24) & 0xFF);
  return pos;
}
size_t gzip_compress_fixed(const unsigned char* in, size_t n,
                           unsigned char* out, size_t outcap) {
  lz77_item_t* items = (lz77_item_t*)malloc(sizeof(lz77_item_t) * n);
  if (!items) return 0;
  size_t nitems = lz77_greedy(in, n, items);
  unsigned char* tmp = (unsigned char*)malloc(n * 2 + 1024);
  if (!tmp) {
    free(items);
    return 0;
  }
  size_t deflen = deflate_fixed_block(items, nitems, tmp, n * 2 + 1024);
  if (!deflen) {
    free(items);
    free(tmp);
    return 0;
  }
  size_t wrote = gzip_wrap(tmp, deflen, in, n, out, outcap);
  free(items);
  free(tmp);
  return wrote;
}
