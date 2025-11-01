#include "worker.h"
#include "file_ops.h"
#include <pthread.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct {
    char *input_path;
    char *output_path;
    file_processor_t processor;
    int result;
} thread_data_t;

/**
 * Thread function that processes a single file.
 */
static void *process_file_thread(void *arg) {
    thread_data_t *data = (thread_data_t *)arg;
    data->result = data->processor(data->input_path, data->output_path);
    return NULL;
}

/**
 * Processes a single file.
 */
static int process_single_file(const char *input, const char *output,
                                file_processor_t processor) {
    return processor(input, output);
}

/**
 * Processes all files in a directory using threads.
 */
static int process_directory(const char *input_dir, const char *output_dir,
                             file_processor_t processor) {
    DIR *dir;
    struct dirent *entry;
    pthread_t *threads = NULL;
    thread_data_t *thread_data = NULL;
    int thread_count = 0;
    int max_threads = 256;
    int result = -1;

    if (create_directory(output_dir) != 0) {
        return -1;
    }

    dir = opendir(input_dir);
    if (!dir) {
        return -1;
    }

    threads = malloc(sizeof(pthread_t) * max_threads);
    thread_data = malloc(sizeof(thread_data_t) * max_threads);

    if (!threads || !thread_data) {
        goto cleanup;
    }

    while ((entry = readdir(dir)) != NULL && thread_count < max_threads) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        size_t input_len = strlen(input_dir) + strlen(entry->d_name) + 2;
        thread_data[thread_count].input_path = malloc(input_len);
        if (!thread_data[thread_count].input_path) {
            goto cleanup;
        }
        snprintf(thread_data[thread_count].input_path, input_len,
                 "%s/%s", input_dir, entry->d_name);

        thread_data[thread_count].output_path =
            construct_output_path(thread_data[thread_count].input_path, output_dir);
        if (!thread_data[thread_count].output_path) {
            free(thread_data[thread_count].input_path);
            goto cleanup;
        }

        thread_data[thread_count].processor = processor;
        thread_data[thread_count].result = -1;

        if (pthread_create(&threads[thread_count], NULL, process_file_thread,
                          &thread_data[thread_count]) != 0) {
            free(thread_data[thread_count].input_path);
            free(thread_data[thread_count].output_path);
            goto cleanup;
        }

        thread_count++;
    }

    result = 0;
    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
        if (thread_data[i].result != 0) {
            result = -1;
        }
    }

cleanup:
    for (int i = 0; i < thread_count; i++) {
        free(thread_data[i].input_path);
        free(thread_data[i].output_path);
    }
    free(threads);
    free(thread_data);
    if (dir) {
        closedir(dir);
    }

    return result;
}

/**
 * Processes a file or directory using the provided processor function.
 */
int process_path(const char *input, const char *output,
                 file_processor_t processor, file_type_t type) {
    if (type == FILE_TYPE_REGULAR) {
        return process_single_file(input, output, processor);
    }

    if (type == FILE_TYPE_DIRECTORY) {
        return process_directory(input, output, processor);
    }

    return -1;
}
