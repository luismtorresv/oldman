#ifndef FILE_OPS_H
#define FILE_OPS_H

#include <sys/types.h>

#define BUFFER_SIZE 4096

typedef enum {
    FILE_TYPE_ERROR = -1,
    FILE_TYPE_REGULAR,
    FILE_TYPE_DIRECTORY
} file_type_t;

/**
 * Determines the type of file at the given path.
 */
file_type_t get_file_type(const char *path);

/**
 * Reads entire file into a buffer using system calls.
 * Returns the number of bytes read, or -1 on failure.
 * Caller is responsible for freeing the buffer.
 */
ssize_t read_file_syscall(const char *path, unsigned char **buffer);

/**
 * Writes a buffer to a file using system calls.
 * Returns 0 on success, -1 on failure.
 */
int write_file_syscall(const char *path, const unsigned char *buffer, size_t size);

/**
 * Creates a directory if it does not exist.
 * Returns 0 on success, -1 on failure.
 */
int create_directory(const char *path);

/**
 * Constructs an output path based on input path and output directory.
 * Caller is responsible for freeing the returned string.
 */
char *construct_output_path(const char *input_path, const char *output_dir);

#endif
