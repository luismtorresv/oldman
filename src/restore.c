#include "restore.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aes256ctr.h"
#include "fsutils.h"
#include "gzip.h"
#include "tar.h"

int restore_flow(const char* input_file, const char** parts, size_t nparts,
                 const unsigned char* key32, const unsigned char noncectr16[16],
                 const char* dest_dir) {
  unsigned char* enc = NULL;
  size_t nenc = 0;
  if (nparts > 0) {
    enc = assemble_parts_to_memory(parts, nparts, &nenc);
    if (!enc) return -1;
  } else if (input_file) {
    enc = fs_read_all(input_file, &nenc);
    if (!enc) return -1;
  } else {
    return -1;
  }
  unsigned char* comp = NULL;
  size_t ncomp = 0;
  if (key32) {
    comp = (unsigned char*)malloc(nenc);
    if (!comp) {
      free(enc);
      return -1;
    }
    memcpy(comp, enc, nenc);
    aes256ctr_t ctx;
    aes256ctr_init(&ctx, key32, noncectr16);
    aes256ctr_xor(&ctx, comp, nenc);
    ncomp = nenc;
    free(enc);
  } else {
    comp = enc;
    ncomp = nenc;
  }
  unsigned char* tarbuf = (unsigned char*)malloc(50 * 1024 * 1024);
  if (!tarbuf) {
    free(comp);
    return -1;
  }
  size_t wrote = gzip_decompress_fixed(comp, ncomp, tarbuf, 50 * 1024 * 1024);
  if (wrote == 0) {
    free(comp);
    free(tarbuf);
    return -1;
  }
  free(comp);
  const char* tmp = "/mnt/data/_restore_tmp.tar";
  if (fs_write_all(tmp, tarbuf, wrote) != 0) {
    free(tarbuf);
    return -1;
  }
  free(tarbuf);
  return tar_extract(tmp, dest_dir);
}
