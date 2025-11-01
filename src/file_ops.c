#define _POSIX_C_SOURCE 200809L
#include "file_ops.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <stdio.h>

/**
 * Determines the type of file at the given path.
 */
file_type_t get_file_type(const char *path) {
    struct stat st;

    if (stat(path, &st) != 0) {
        return FILE_TYPE_ERROR;
    }

    if (S_ISDIR(st.st_mode)) {
        return FILE_TYPE_DIRECTORY;
    }

    if (S_ISREG(st.st_mode)) {
        return FILE_TYPE_REGULAR;
    }

    return FILE_TYPE_ERROR;
}

/**
 * Reads entire file into a buffer using system calls.
 */
ssize_t read_file_syscall(const char *path, unsigned char **buffer) {
    int fd;
    struct stat st;
    ssize_t total_read = 0;
    ssize_t bytes_read;
    unsigned char *data = NULL;

    fd = open(path, O_RDONLY);
    if (fd < 0) {
        return -1;
    }

    if (fstat(fd, &st) != 0) {
        close(fd);
        return -1;
    }

    data = malloc(st.st_size);
    if (!data) {
        close(fd);
        return -1;
    }

    while (total_read < st.st_size) {
        bytes_read = read(fd, data + total_read, st.st_size - total_read);
        if (bytes_read <= 0) {
            free(data);
            close(fd);
            return -1;
        }
        total_read += bytes_read;
    }

    close(fd);
    *buffer = data;
    return total_read;
}

/**
 * Writes a buffer to a file using system calls.
 */
int write_file_syscall(const char *path, const unsigned char *buffer, size_t size) {
    int fd;
    ssize_t total_written = 0;
    ssize_t bytes_written;

    fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        return -1;
    }

    while (total_written < (ssize_t)size) {
        bytes_written = write(fd, buffer + total_written, size - total_written);
        if (bytes_written <= 0) {
            close(fd);
            return -1;
        }
        total_written += bytes_written;
    }

    close(fd);
    return 0;
}

/**
 * Creates a directory if it does not exist.
 */
int create_directory(const char *path) {
    struct stat st;

    if (stat(path, &st) == 0) {
        return S_ISDIR(st.st_mode) ? 0 : -1;
    }

    return mkdir(path, 0755);
}

/**
 * Constructs an output path based on input path and output directory.
 */
char *construct_output_path(const char *input_path, const char *output_dir) {
    char *input_copy;
    char *base_name;
    size_t output_len;
    char *output_path;

    input_copy = strdup(input_path);
    if (!input_copy) {
        return NULL;
    }

    base_name = basename(input_copy);
    output_len = strlen(output_dir) + strlen(base_name) + 2;
    output_path = malloc(output_len);

    if (!output_path) {
        free(input_copy);
        return NULL;
    }

    snprintf(output_path, output_len, "%s/%s", output_dir, base_name);
    free(input_copy);

    return output_path;
}
