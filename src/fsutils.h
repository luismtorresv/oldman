#ifndef FSUTILS_H
#define FSUTILS_H
#include <stddef.h>
int fs_enum_files(const char** roots, size_t nroots, char*** out_paths,
                  size_t* out_count);
void fs_free_list(char** paths, size_t count);
unsigned char* fs_read_all(const char* path, size_t* n);
int fs_write_all(const char* path, const unsigned char* buf, size_t n);
int fs_mkdirs(const char* path);
#endif
