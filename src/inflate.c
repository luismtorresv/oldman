#include "inflate.h"
static int decode_fixed_ll(bitreader_t* br) {
  if (!br_need(br, 7)) return -1;
  unsigned p7 = br->bitbuf & 0x7F;
  if (p7 <= 23) {
    br_get(br, 7);
    return 256 + p7;
  }
  if (!br_need(br, 8)) return -1;
  unsigned p8 = br->bitbuf & 0xFF;
  if (p8 <= 191) {
    br_get(br, 8);
    return (int)(p8 - 48);
  }
  if (!br_need(br, 9)) return -1;
  unsigned p9 = br->bitbuf & 0x1FF;
  if (p9 <= 511) {
    br_get(br, 9);
    return 144 + (int)(p9 - 400);
  }
  if (!br_need(br, 8)) return -1;
  p8 = br->bitbuf & 0xFF;
  br_get(br, 8);
  return 280 + (int)(p8 - 192);
}
static int read_len(int sym, bitreader_t* br) {
  static const int base[29] = {3,  4,  5,  6,   7,   8,   9,   10,  11, 13,
                               15, 17, 19, 23,  27,  31,  35,  43,  51, 59,
                               67, 83, 99, 115, 131, 163, 195, 227, 258};
  static const int eb[29] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2,
                             2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};
  int i = sym - 257;
  if (i < 0 || i >= 29) return -1;
  int e = eb[i];
  int val = base[i];
  if (e) {
    if (!br_need(br, e)) return -1;
    val += (int)br_get(br, e);
  }
  return val;
}
static int read_dist(bitreader_t* br) {
  if (!br_need(br, 5)) return -1;
  int code = (int)br_get(br, 5);
  static const int base[30] = {1,    2,    3,    4,     5,     7,    9,    13,
                               17,   25,   33,   49,    65,    97,   129,  193,
                               257,  385,  513,  769,   1025,  1537, 2049, 3073,
                               4097, 6145, 8193, 12289, 16385, 24577};
  static const int eb[30] = {0, 0, 0,  0,  1,  1,  2,  2,  3,  3,
                             4, 4, 5,  5,  6,  6,  7,  7,  8,  8,
                             9, 9, 10, 10, 11, 11, 12, 12, 13, 13};
  int e = eb[code];
  int val = base[code];
  if (e) {
    if (!br_need(br, e)) return -1;
    val += (int)br_get(br, e);
  }
  return val;
}
size_t inflate_fixed_block(bitreader_t* br, unsigned char* out, size_t outcap) {
  size_t op = 0;
  for (;;) {
    int sym = decode_fixed_ll(br);
    if (sym < 0) return 0;
    if (sym < 256) {
      if (op >= outcap) return 0;
      out[op++] = (unsigned char)sym;
    } else if (sym == 256) {
      return op;
    } else {
      int L = read_len(sym, br);
      if (L < 0) return 0;
      int D = read_dist(br);
      if (D <= 0 || D > (int)op) return 0;
      if (op + (size_t)L > outcap) return 0;
      for (int i = 0; i < L; i++) {
        out[op] = out[op - D];
        op++;
      }
    }
  }
}
