#define _XOPEN_SOURCE 700
#include "fsutils.h"

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static int is_regular(const char* path) {
  struct stat st;
  if (stat(path, &st) != 0) return 0;
  return S_ISREG(st.st_mode);
}
static int is_dir(const char* path) {
  struct stat st;
  if (stat(path, &st) != 0) return 0;
  return S_ISDIR(st.st_mode);
}
static int append_path(char*** list, size_t* count, const char* path) {
  char** nlist = (char**)realloc(*list, (*count + 1) * sizeof(char*));
  if (!nlist) return -1;
  *list = nlist;
  (*list)[*count] = strdup(path);
  if (!(*list)[*count]) return -1;
  (*count)++;
  return 0;
}
static int enum_dir(const char* root, char*** list, size_t* count) {
  DIR* d = opendir(root);
  if (!d) return -1;
  struct dirent* de;
  while ((de = readdir(d))) {
    if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) continue;
    char path[4096];
    snprintf(path, sizeof(path), "%s/%s", root, de->d_name);
    if (is_dir(path)) {
      if (enum_dir(path, list, count) != 0) {
        closedir(d);
        return -1;
      }
    } else if (is_regular(path)) {
      if (append_path(list, count, path) != 0) {
        closedir(d);
        return -1;
      }
    }
  }
  closedir(d);
  return 0;
}
int fs_enum_files(const char** roots, size_t nroots, char*** out_paths,
                  size_t* out_count) {
  *out_paths = NULL;
  *out_count = 0;
  for (size_t i = 0; i < nroots; i++) {
    const char* r = roots[i];
    if (is_dir(r)) {
      if (enum_dir(r, out_paths, out_count) != 0) return -1;
    } else if (is_regular(r)) {
      if (append_path(out_paths, out_count, r) != 0) return -1;
    } else {
      fprintf(stderr, "[WARN] Skipping: %s\n", r);
    }
  }
  return 0;
}
void fs_free_list(char** paths, size_t count) {
  if (!paths) return;
  for (size_t i = 0; i < count; i++) free(paths[i]);
  free(paths);
}
unsigned char* fs_read_all(const char* path, size_t* n) {
  *n = 0;
  FILE* f = fopen(path, "rb");
  if (!f) return NULL;
  fseek(f, 0, SEEK_END);
  long sz = ftell(f);
  fseek(f, 0, SEEK_SET);
  unsigned char* buf = (unsigned char*)malloc((size_t)sz);
  if (!buf) {
    fclose(f);
    return NULL;
  }
  size_t rd = fread(buf, 1, (size_t)sz, f);
  fclose(f);
  if (rd != (size_t)sz) {
    free(buf);
    return NULL;
  }
  *n = (size_t)sz;
  return buf;
}
int fs_write_all(const char* path, const unsigned char* buf, size_t n) {
  FILE* f = fopen(path, "wb");
  if (!f) return -1;
  size_t wr = fwrite(buf, 1, n, f);
  fclose(f);
  return wr == n ? 0 : -1;
}
int fs_mkdirs(const char* path) {
  char tmp[4096];
  snprintf(tmp, sizeof(tmp), "%s", path);
  for (char* p = tmp + 1; *p; ++p) {
    if (*p == '/') {
      *p = 0;
      mkdir(tmp, 0755);
      *p = '/';
    }
  }
  if (mkdir(tmp, 0755) != 0 && errno != EEXIST) return -1;
  return 0;
}
