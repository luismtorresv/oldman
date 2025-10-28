#ifndef SPLIT_H
#define SPLIT_H
#include <stddef.h>
int split_file_round_robin(const char* infile, size_t part_bytes, const char** outdirs, size_t ndirs);
unsigned char* assemble_parts_to_memory(const char** parts, size_t nparts, size_t* out_len);
#endif
