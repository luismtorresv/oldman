#ifndef BITIO_H
#define BITIO_H
#include <stddef.h>
#include <stdint.h>
typedef struct {
  unsigned char* buf;
  size_t cap;
  size_t pos;
  unsigned bitbuf;
  int bitcount;
} bitwriter_t;
void bw_init(bitwriter_t* bw, unsigned char* buf, size_t cap);
int bw_put(bitwriter_t* bw, unsigned v, int nb);
int bw_flush(bitwriter_t* bw);
size_t bw_size(const bitwriter_t* bw);

typedef struct {
  const unsigned char* buf;
  size_t len;
  size_t pos;
  unsigned bitbuf;
  int bitcount;
} bitreader_t;
void br_init(bitreader_t* br, const unsigned char* buf, size_t len);
int br_need(bitreader_t* br, int nbits);
unsigned br_get(bitreader_t* br, int nbits);
#endif
