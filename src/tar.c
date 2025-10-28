#include "tar.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "fsutils.h"

#pragma pack(push, 1)
typedef struct {
  char name[100];
  char mode[8];
  char uid[8];
  char gid[8];
  char size[12];
  char mtime[12];
  char chksum[8];
  char typeflag;
  char linkname[100];
  char magic[6];
  char version[2];
  char uname[32];
  char gname[32];
  char devmajor[8];
  char devminor[8];
  char prefix[155];
  char pad[12];
} tar_hdr_t;
#pragma pack(pop)

static void zero(void* p, size_t n) { memset(p, 0, n); }
static void octal(size_t v, char* out, size_t n) {
  snprintf(out, n, "%0*zo", (int)(n - 1), v);
}
static unsigned hdr_checksum(const tar_hdr_t* h) {
  unsigned sum = 0;
  const unsigned char* p = (const unsigned char*)h;
  for (size_t i = 0; i < sizeof(*h); ++i) sum += p[i];
  return sum;
}

int tar_create(const char** files, size_t nfiles, const char* out_tar_path) {
  FILE* out = fopen(out_tar_path, "wb");
  if (!out) return -1;
  unsigned char* buf = (unsigned char*)malloc(1 << 20);
  if (!buf) {
    fclose(out);
    return -1;
  }
  for (size_t i = 0; i < nfiles; i++) {
    const char* path = files[i];
    struct stat st;
    if (stat(path, &st) != 0) {
      free(buf);
      fclose(out);
      return -1;
    }
    if (!S_ISREG(st.st_mode)) continue;
    tar_hdr_t h;
    zero(&h, sizeof(h));
    if (strlen(path) >= 100) {
      free(buf);
      fclose(out);
      return -1;
    }
    memcpy(h.name, path, strlen(path));
    snprintf(h.mode, sizeof(h.mode), "%07o", 0644);
    snprintf(h.uid, sizeof(h.uid), "%07o", 0);
    snprintf(h.gid, sizeof(h.gid), "%07o", 0);
    octal((size_t)st.st_size, h.size, sizeof(h.size));
    octal((size_t)st.st_mtime, h.mtime, sizeof(h.mtime));
    memcpy(h.magic, "ustar", 5);
    memcpy(h.version, "00", 2);
    h.typeflag = '0';
    memset(h.chksum, ' ', sizeof(h.chksum));
    unsigned sum = hdr_checksum(&h);
    snprintf(h.chksum, sizeof(h.chksum), "%06o", sum);
    h.chksum[6] = '\0';
    h.chksum[7] = ' ';
    fwrite(&h, 1, sizeof(h), out);
    FILE* in = fopen(path, "rb");
    if (!in) {
      free(buf);
      fclose(out);
      return -1;
    }
    size_t remain = (size_t)st.st_size;
    while (remain > 0) {
      size_t take = remain > (1 << 20) ? (1 << 20) : remain;
      size_t rd = fread(buf, 1, take, in);
      if (rd != take) {
        fclose(in);
        free(buf);
        fclose(out);
        return -1;
      }
      fwrite(buf, 1, rd, out);
      remain -= rd;
    }
    fclose(in);
    size_t pad = (512 - (st.st_size % 512)) % 512;
    memset(buf, 0, pad);
    fwrite(buf, 1, pad, out);
  }
  unsigned char zeros[512] = {0};
  fwrite(zeros, 1, 512, out);
  fwrite(zeros, 1, 512, out);
  free(buf);
  fclose(out);
  return 0;
}

int tar_extract(const char* tar_path, const char* dest_dir) {
  if (fs_mkdirs(dest_dir) != 0) return -1;
  FILE* f = fopen(tar_path, "rb");
  if (!f) return -1;
  unsigned char* buf = (unsigned char*)malloc(1 << 20);
  if (!buf) {
    fclose(f);
    return -1;
  }
  for (;;) {
    tar_hdr_t h;
    size_t rd = fread(&h, 1, sizeof(h), f);
    if (rd != sizeof(h)) {
      free(buf);
      fclose(f);
      return -1;
    }
    int allzero = 1;
    for (size_t i = 0; i < sizeof(h); ++i)
      if (((unsigned char*)&h)[i]) {
        allzero = 0;
        break;
      }
    if (allzero) break;
    char name[256] = {0};
    memcpy(name, h.name, 100);
    size_t fsize = strtoull(h.size, NULL, 8);
    char outp[1024] = {0};
    snprintf(outp, sizeof(outp), "%s/%s", dest_dir, name);
    fs_mkdirs(outp);
    FILE* o = fopen(outp, "wb");
    if (!o) {
      free(buf);
      fclose(f);
      return -1;
    }
    size_t remain = fsize;
    while (remain > 0) {
      size_t take = remain > (1 << 20) ? (1 << 20) : remain;
      size_t r = fread(buf, 1, take, f);
      if (r != take) {
        fclose(o);
        free(buf);
        fclose(f);
        return -1;
      }
      fwrite(buf, 1, r, o);
      remain -= r;
    }
    fclose(o);
    size_t pad = (512 - (fsize % 512)) % 512;
    if (pad) fread(buf, 1, pad, f);
  }
  free(buf);
  fclose(f);
  return 0;
}
