#ifndef WORKER_H
#define WORKER_H

#include "file_ops.h"

typedef int (*file_processor_t)(const char *input, const char *output);

/**
 * Processes a file or directory using the provided processor function.
 * For directories, spawns a thread for each file.
 * Returns 0 on success, -1 on failure.
 */
int process_path(const char *input, const char *output,
                 file_processor_t processor, file_type_t type);

#endif
