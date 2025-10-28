#ifndef TAR_H
#define TAR_H
#include <stdint.h>
#include <stddef.h>
int tar_create(const char** files, size_t nfiles, const char* out_tar_path);
int tar_extract(const char* tar_path, const char* dest_dir);
#endif
