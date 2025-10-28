#include "split.h"

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fsutils.h"

int split_file_round_robin(const char* infile, size_t part_bytes,
                           const char** outdirs, size_t ndirs) {
  if (part_bytes == 0 || ndirs == 0) return 0;
  FILE* in = fopen(infile, "rb");
  if (!in) return -1;
  fseek(in, 0, SEEK_END);
  long sz = ftell(in);
  fseek(in, 0, SEEK_SET);
  size_t parts = (sz + part_bytes - 1) / part_bytes;
#pragma omp parallel for schedule(dynamic)
  for (long i = 0; i < (long)parts; i++) {
    size_t off = (size_t)i * part_bytes;
    size_t len =
        ((size_t)sz - off) > part_bytes ? part_bytes : ((size_t)sz - off);
    const char* dir = outdirs[i % ndirs];
    fs_mkdirs(dir);
    char outp[1024];
    snprintf(outp, sizeof(outp), "%s/%s.part_%06ld", dir,
             strrchr(infile, '/') ? strrchr(infile, '/') + 1 : infile, (long)i);
    FILE* in2 = fopen(infile, "rb");
    FILE* o = fopen(outp, "wb");
    if (!in2 || !o) {
      if (in2) fclose(in2);
      if (o) fclose(o);
      continue;
    }
    fseek(in2, off, SEEK_SET);
    unsigned char* buf = (unsigned char*)malloc(1 << 20);
    size_t remain = len;
    while (remain > 0) {
      size_t take = remain > (1 << 20) ? (1 << 20) : remain;
      size_t r = fread(buf, 1, take, in2);
      fwrite(buf, 1, r, o);
      remain -= r;
    }
    free(buf);
    fclose(o);
    fclose(in2);
#pragma omp critical
    {
      fprintf(stderr, "[USB] wrote %s\n", outp);
    }
  }
  fclose(in);
  return 0;
}
unsigned char* assemble_parts_to_memory(const char** parts, size_t nparts,
                                        size_t* out_len) {
  size_t total = 0;
  for (size_t i = 0; i < nparts; i++) {
    FILE* f = fopen(parts[i], "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fclose(f);
    total += (size_t)sz;
  }
  unsigned char* out = (unsigned char*)malloc(total);
  if (!out) return NULL;
  size_t pos = 0;
  unsigned char* buf = (unsigned char*)malloc(1 << 20);
  for (size_t i = 0; i < nparts; i++) {
    FILE* f = fopen(parts[i], "rb");
    if (!f) {
      free(out);
      free(buf);
      return NULL;
    }
    while (!feof(f)) {
      size_t r = fread(buf, 1, 1 << 20, f);
      memcpy(out + pos, buf, r);
      pos += r;
    }
    fclose(f);
  }
  free(buf);
  *out_len = pos;
  return out;
}
