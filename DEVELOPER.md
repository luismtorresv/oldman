# GSEA - Developer Guide

## Code Organization

### Module Overview

#### main.c
- Entry point for the application
- CLI argument parsing using getopt_long
- Options validation
- Operation orchestration

Key functions:
- `parse_arguments()`: Processes command-line arguments
- `validate_options()`: Ensures valid operation combinations
- `execute_operations()`: Dispatches work to appropriate modules

#### compress.c/h
- Run-Length Encoding implementation
- Buffer-based compression and decompression
- File-level compression interface

Key functions:
- `rle_compress()`: Core RLE compression algorithm
- `rle_decompress()`: Core RLE decompression algorithm
- `compress_file()`: High-level file compression
- `decompress_file()`: High-level file decompression

RLE Format:
```
Runs:     [count > 0][byte]              - repeat 'byte' count times
Literals: [0][count][byte1][byte2]...    - count literal bytes follow
```

#### encrypt.c/h
- Vigenère cipher implementation
- Symmetric encryption with hardcoded key
- Buffer and file-level encryption interface

Key functions:
- `vigenere_encrypt()`: XOR-like encryption with key
- `vigenere_decrypt()`: Reverse operation of encrypt
- `encrypt_file()`: High-level file encryption
- `decrypt_file()`: High-level file decryption

Default key: "gsea2025" (defined in encrypt.h)

#### file_ops.c/h
- Low-level file operations using system calls
- Path manipulation utilities
- Directory management

Key functions:
- `get_file_type()`: Determines if path is file or directory
- `read_file_syscall()`: Reads entire file using open/read/close
- `write_file_syscall()`: Writes buffer using open/write/close
- `create_directory()`: Creates directory if needed
- `construct_output_path()`: Builds output paths for directory processing

System calls used:
- `open()`: Opens files with flags
- `read()`: Reads data in chunks
- `write()`: Writes data in chunks
- `close()`: Closes file descriptors
- `stat()`: Gets file metadata
- `fstat()`: Gets file descriptor metadata
- `mkdir()`: Creates directories
- `opendir()`: Opens directory stream
- `readdir()`: Reads directory entries
- `closedir()`: Closes directory stream

#### worker.c/h
- Thread management for concurrent processing
- Directory traversal and file dispatching
- Thread lifecycle management

Key functions:
- `process_path()`: Entry point for file/directory processing
- `process_single_file()`: Handles single file operations
- `process_directory()`: Spawns threads for each file
- `process_file_thread()`: Thread function for file processing

Concurrency model:
- One pthread per file in directory
- Maximum 256 concurrent threads
- No shared state between threads
- Main thread joins all worker threads

## Data Flow

### Single File Processing

```
Input File
    ↓
read_file_syscall() → buffer in memory
    ↓
Algorithm (compress/decompress/encrypt/decrypt)
    ↓
write_file_syscall() → Output File
```

### Directory Processing

```
Input Directory
    ↓
opendir() + readdir() → list of files
    ↓
For each file:
    ↓
    pthread_create() → Worker Thread
        ↓
        read_file_syscall()
        ↓
        Algorithm
        ↓
        write_file_syscall()
    ↓
pthread_join() all threads
    ↓
Output Directory
```

## Error Handling

### Pattern Used

All functions follow this pattern:

```c
int operation() {
    int result = -1;
    resource_t *resource = NULL;

    // Allocate resources
    resource = allocate();
    if (!resource) {
        goto cleanup;
    }

    // Perform operation
    if (operation_fails()) {
        goto cleanup;
    }

    result = 0;

cleanup:
    free(resource);
    return result;
}
```

Benefits:
- Single cleanup path
- No resource leaks
- Clear success/failure indication

### Return Values

- `0` indicates success
- `-1` indicates failure
- `ssize_t` functions return size on success, `-1` on failure

## Memory Management

### Allocation Strategy

- Buffers allocated on heap for flexibility
- Compression output buffer: `input_size * 2 + 1024`
- Decompression output buffer: `input_size * 256`
- Encryption buffers: same size as input

### Deallocation

- Always use goto cleanup pattern
- Free all allocated memory before return
- Null checks before free (safe but redundant)

## Thread Safety

### Thread-Safe Operations

- Each thread operates on different files
- No shared global state
- Thread-local variables only

### Not Thread-Safe

- stdout/stderr writing (acceptable for error messages)
- Directory creation (acceptable, mkdir is idempotent)

## Building and Compilation

### Makefile Targets

- `make` or `make all`: Build the executable
- `make clean`: Remove build artifacts
- `make run`: Build and run (shows help)
- `make format`: Format code with clang-format

### Compiler Flags

```makefile
-Wall -Wextra -Wpedantic  # All warnings
-Wunused                   # Unused variable warnings
-Wunreachable-code        # Dead code detection
-pthread                   # POSIX threads support
-g                        # Debug symbols
-std=c11                  # C11 standard
-fopenmp                  # OpenMP support (future)
```

## Testing

### Running Tests

```bash
./test.sh
```

### Test Coverage

1. Single file compression/decompression
2. Single file encryption/decryption
3. Directory processing with 10 files
4. Data integrity verification
5. Size comparison

### Adding New Tests

Edit `test.sh` and follow the existing pattern:

```bash
echo "[TEST N] Description"
# Setup
# Run operation
# Verify
# Cleanup
```

## Extending the Code

### Adding a New Algorithm

1. Create new module: `algorithm.c/h`
2. Implement interface:
   ```c
   int algorithm_compress(const char *in, const char *out);
   int algorithm_decompress(const char *in, const char *out);
   ```
3. Add to main.c switch statement
4. Update CLI parsing for new algorithm name
5. Document in REPORT.md

### Adding Combined Operations

Currently not supported. To implement:

1. Modify options validation to allow multiple operations
2. Create pipeline in execute_operations()
3. Use temporary files for intermediate results
4. Chain operations in order

Example:
```c
if (opts->compress && opts->encrypt) {
    compress_file(input, temp_file);
    encrypt_file(temp_file, output);
    unlink(temp_file);
}
```

## Performance Optimization

### Current Bottlenecks

1. Loading entire files into memory
2. No buffering for large files
3. Sequential directory reading

### Potential Improvements

1. Streaming processing for large files
2. Thread pool with work queue
3. Asynchronous I/O
4. Parallel directory scanning

## Code Style

### Naming Conventions

- Functions: `lowercase_with_underscores()`
- Types: `name_t` suffix for typedefs
- Constants: `UPPERCASE_WITH_UNDERSCORES`
- Variables: `lowercase_with_underscores`

### Formatting

- Indent: 4 spaces (no tabs)
- Brace style: K&R
- Max line length: 100 characters
- Comments: `/* */` for block, `//` for inline

### Documentation

Every public function must have a docstring:

```c
/**
 * Brief description of what the function does.
 * Additional details if needed.
 * Returns: description of return value.
 */
int function_name(params);
```

## Common Issues

### Compilation Errors

**Error**: `unknown type name 'ssize_t'`
**Fix**: Add `#include <sys/types.h>` to header

**Error**: `implicit declaration of function 'strdup'`
**Fix**: Add `#define _POSIX_C_SOURCE 200809L` before includes

### Runtime Issues

**Issue**: Compressed file larger than original
**Reason**: RLE performs poorly on random data
**Solution**: Skip compression or use different algorithm

**Issue**: Segmentation fault on directory processing
**Reason**: Exceeding max thread limit
**Solution**: Already limited to 256 threads

## Debugging

### Compile with Debug Symbols

Already enabled with `-g` flag in Makefile.

### Using GDB

```bash
gdb ./gsea
(gdb) run -c -i test.txt -o test.rle
(gdb) bt  # backtrace on crash
```

### Valgrind for Memory Leaks

```bash
valgrind --leak-check=full ./gsea -c -i test.txt -o test.rle
```

## Architecture Decisions

### Why RLE over Huffman?

- Simpler implementation (no tree construction)
- O(n) vs O(n log n) complexity
- Predictable output size
- Sufficient for educational purposes

### Why Vigenère over AES?

- Much simpler implementation
- No need for S-boxes or round functions
- Demonstrates basic encryption concepts
- Adequate for basic obfuscation

### Why POSIX Threads over OpenMP?

- More explicit control over thread lifecycle
- Better for I/O-bound tasks
- One thread per file model is natural
- OpenMP better for CPU-bound data parallelism

### Why System Calls over stdio?

- Required by assignment
- More control over I/O operations
- Educational value in understanding low-level operations
- Minimal overhead

## Performance Metrics

### Expected Performance

- RLE compression: ~100 MB/s (depends on data)
- Vigenère encryption: ~500 MB/s (CPU bound)
- Thread overhead: ~1-2ms per thread
- Scalability: Linear with number of files (up to core count)

### Benchmarking

```bash
time ./gsea -c -i large_file.txt -o large_file.rle
```

## Future Work

1. Add progress indicators with percentage
2. Implement streaming for files larger than RAM
3. Add checksum verification (CRC32)
4. Support for configurable encryption key
5. Implement Huffman or LZW compression
6. Add verbose/quiet modes
7. Support for archive format (TAR-like)
8. Parallel compression within single file
