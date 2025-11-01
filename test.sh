#!/bin/bash
# Test script for GSEA utility

set -e

echo "=== GSEA Test Suite ==="
echo

# Test 1: Compression and decompression
echo "[TEST 1] Compression and decompression of single file"
echo "Creating test file with repetitive data..."
echo "AAAAAAAAAA BBBBBBBBBB CCCCCCCCCC repeating text repeating text repeating text" > test_file.txt
ORIGINAL_SIZE=$(stat -f%z test_file.txt 2>/dev/null || stat -c%s test_file.txt)
echo "Original size: $ORIGINAL_SIZE bytes"

echo "Compressing..."
./gsea -c -i test_file.txt -o test_file.rle
COMPRESSED_SIZE=$(stat -f%z test_file.rle 2>/dev/null || stat -c%s test_file.rle)
echo "Compressed size: $COMPRESSED_SIZE bytes"

echo "Decompressing..."
./gsea -d -i test_file.rle -o test_file_restored.txt

echo "Verifying integrity..."
if diff test_file.txt test_file_restored.txt > /dev/null; then
    echo "PASSED: Compression/decompression successful"
else
    echo "FAILED: Files differ after compression/decompression"
    exit 1
fi
echo

# Test 2: Encryption and decryption
echo "[TEST 2] Encryption and decryption of single file"
echo "Encrypting test file..."
./gsea -e -i test_file.txt -o test_file.enc

echo "Decrypting..."
./gsea -u -i test_file.enc -o test_file_decrypted.txt

echo "Verifying integrity..."
if diff test_file.txt test_file_decrypted.txt > /dev/null; then
    echo "PASSED: Encryption/decryption successful"
else
    echo "FAILED: Files differ after encryption/decryption"
    exit 1
fi
echo

# Test 3: Directory processing
echo "[TEST 3] Directory compression with concurrent processing"
echo "Creating test directory..."
mkdir -p test_directory
for i in {1..10}; do
    echo "File number $i with data XXXXXXXXXX YYYYYYYYYY" > test_directory/file_$i.txt
done
echo "Created 10 files in test_directory/"

echo "Compressing directory..."
./gsea -c -i test_directory/ -o test_directory_compressed/
echo "Compressed files:"
ls -1 test_directory_compressed/ | wc -l

echo "Decompressing directory..."
./gsea -d -i test_directory_compressed/ -o test_directory_restored/

echo "Verifying all files..."
ALL_MATCH=true
for i in {1..10}; do
    if ! diff test_directory/file_$i.txt test_directory_restored/file_$i.txt > /dev/null; then
        echo "FAILED: file_$i.txt differs"
        ALL_MATCH=false
    fi
done

if $ALL_MATCH; then
    echo "PASSED: All files in directory processed correctly"
else
    exit 1
fi
echo

# Cleanup
echo "Cleaning up test files..."
rm -rf test_file.txt test_file.rle test_file_restored.txt
rm -rf test_file.enc test_file_decrypted.txt
rm -rf test_directory test_directory_compressed test_directory_restored

echo
echo "=== All tests passed ==="
