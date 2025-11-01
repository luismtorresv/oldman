# GSEA - Quick Start Guide

## Installation

```bash
# Clone or navigate to the project directory
cd /path/to/gsea

# Compile
make

# Verify installation
./gsea -h
```

## Basic Usage

### Compress a file
```bash
./gsea -c -i input.txt -o input.txt.rle
```

### Decompress a file
```bash
./gsea -d -i input.txt.rle -o restored.txt
```

### Encrypt a file
```bash
./gsea -e -i document.pdf -o document.enc
```

### Decrypt a file
```bash
./gsea -u -i document.enc -o document.pdf
```

### Process a directory
```bash
./gsea -c -i ./data/ -o ./data_compressed/
```

## Testing

```bash
# Run all tests
./test.sh

# Run demo
./demo.sh
```

## Common Workflows

### Backup sensitive logs
```bash
# Compress and encrypt logs
./gsea -c -i /var/log/app/ -o /tmp/logs_compressed/
./gsea -e -i /tmp/logs_compressed/ -o /backup/logs_secure/
```

### Restore backup
```bash
# Decrypt and decompress
./gsea -u -i /backup/logs_secure/ -o /tmp/logs_compressed/
./gsea -d -i /tmp/logs_compressed/ -o /restore/logs/
```

## Performance Tips

- RLE works best with repetitive data (logs, simple images)
- For random data, compression may increase file size
- Directory processing automatically uses multiple threads
- Larger directories benefit more from parallel processing

## Troubleshooting

**Problem**: Compiled file larger than original
**Solution**: This is normal for random data with RLE. Use different algorithm or skip compression.

**Problem**: Compilation fails
**Solution**: Ensure you have gcc with C11 support and pthread library.

**Problem**: Permission denied
**Solution**: Make sure you have read access to input and write access to output directory.

## Key Files

- `src/` - Source code
- `README.md` - Full documentation
- `REPORT.md` - Technical report (Spanish)
- `EXAMPLES.md` - More usage examples
- `DEVELOPER.md` - For contributors

## Default Settings

- Compression algorithm: RLE (Run-Length Encoding)
- Encryption algorithm: Vigenère cipher
- Encryption key: "gsea2025" (hardcoded in src/encrypt.h)
- Max concurrent threads: 256

## Getting Help

```bash
./gsea -h
```

Or read the documentation:
- User guide: README.md
- Technical details: REPORT.md
- Examples: EXAMPLES.md
- Development: DEVELOPER.md
