#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "compress.h"
#include "encrypt.h"
#include "file_ops.h"
#include "worker.h"

#define DEFAULT_COMP_ALG "rle"
#define DEFAULT_ENC_ALG "vigenere"

typedef struct {
    int compress;
    int decompress;
    int encrypt;
    int decrypt;
    char *comp_alg;
    char *enc_alg;
    char *input;
    char *output;
} options_t;

/**
 * Prints usage information for the program.
 */
static void print_usage(const char *prog_name) {
    printf("Usage: %s [OPTIONS]\n", prog_name);
    printf("\nOptions:\n");
    printf("  -c              Compress input\n");
    printf("  -d              Decompress input\n");
    printf("  -e              Encrypt input\n");
    printf("  -u              Decrypt input\n");
    printf("  --comp-alg ALG  Compression algorithm (default: %s)\n", DEFAULT_COMP_ALG);
    printf("  --enc-alg ALG   Encryption algorithm (default: %s)\n", DEFAULT_ENC_ALG);
    printf("  -i FILE/DIR     Input file or directory\n");
    printf("  -o FILE/DIR     Output file or directory\n");
    printf("  -h              Show this help message\n");
}

/**
 * Parses command line arguments into an options structure.
 */
static int parse_arguments(int argc, char **argv, options_t *opts) {
    int opt;
    int option_index = 0;

    static struct option long_options[] = {
        {"comp-alg", required_argument, 0, 'C'},
        {"enc-alg",  required_argument, 0, 'E'},
        {0, 0, 0, 0}
    };

    memset(opts, 0, sizeof(options_t));
    opts->comp_alg = DEFAULT_COMP_ALG;
    opts->enc_alg = DEFAULT_ENC_ALG;

    while ((opt = getopt_long(argc, argv, "cdeui:o:h", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'c':
                opts->compress = 1;
                break;
            case 'd':
                opts->decompress = 1;
                break;
            case 'e':
                opts->encrypt = 1;
                break;
            case 'u':
                opts->decrypt = 1;
                break;
            case 'C':
                opts->comp_alg = optarg;
                break;
            case 'E':
                opts->enc_alg = optarg;
                break;
            case 'i':
                opts->input = optarg;
                break;
            case 'o':
                opts->output = optarg;
                break;
            case 'h':
                return -1;
            default:
                return -1;
        }
    }

    return 0;
}

/**
 * Validates the parsed options.
 */
static int validate_options(const options_t *opts) {
    int operation_count = opts->compress + opts->decompress + opts->encrypt + opts->decrypt;

    if (operation_count == 0) {
        fprintf(stderr, "Error: no operation specified\n");
        return -1;
    }

    if (opts->compress && opts->decompress) {
        fprintf(stderr, "Error: cannot compress and decompress simultaneously\n");
        return -1;
    }

    if (opts->encrypt && opts->decrypt) {
        fprintf(stderr, "Error: cannot encrypt and decrypt simultaneously\n");
        return -1;
    }

    if (!opts->input) {
        fprintf(stderr, "Error: input not specified\n");
        return -1;
    }

    if (!opts->output) {
        fprintf(stderr, "Error: output not specified\n");
        return -1;
    }

    return 0;
}

/**
 * Executes the requested operations on the input.
 */
static int execute_operations(const options_t *opts) {
    file_type_t input_type = get_file_type(opts->input);

    if (input_type == FILE_TYPE_ERROR) {
        fprintf(stderr, "Error: cannot access input\n");
        return -1;
    }

    if (opts->compress) {
        if (process_path(opts->input, opts->output, compress_file, input_type) != 0) {
            fprintf(stderr, "Error: compression failed\n");
            return -1;
        }
    } else if (opts->decompress) {
        if (process_path(opts->input, opts->output, decompress_file, input_type) != 0) {
            fprintf(stderr, "Error: decompression failed\n");
            return -1;
        }
    } else if (opts->encrypt) {
        if (process_path(opts->input, opts->output, encrypt_file, input_type) != 0) {
            fprintf(stderr, "Error: encryption failed\n");
            return -1;
        }
    } else if (opts->decrypt) {
        if (process_path(opts->input, opts->output, decrypt_file, input_type) != 0) {
            fprintf(stderr, "Error: decryption failed\n");
            return -1;
        }
    }

    return 0;
}

int main(int argc, char **argv) {
    options_t opts;

    if (parse_arguments(argc, argv, &opts) != 0) {
        print_usage(argv[0]);
        return 1;
    }

    if (validate_options(&opts) != 0) {
        return 1;
    }

    if (execute_operations(&opts) != 0) {
        return 1;
    }

    return 0;
}
