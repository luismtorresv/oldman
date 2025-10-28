#ifndef RESTORE_H
#define RESTORE_H
#include <stddef.h>
int restore_flow(const char* input_file, const char** parts, size_t nparts,
                 const unsigned char* key32, const unsigned char noncectr16[16],
                 const char* dest_dir);
#endif
