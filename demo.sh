#!/bin/bash
# Demo script for GSEA video presentation

echo "======================================"
echo "GSEA - Demo Video"
echo "======================================"
echo

# Demo 1: Show help
echo "[Demo 1] Program capabilities"
echo "$ ./gsea -h"
./gsea -h
echo
sleep 2

# Demo 2: Compression
echo "[Demo 2] File compression"
echo "Creating test file with repetitive data..."
echo "$ yes 'test data' | head -1000 > demo_test.txt"
yes "test data" | head -1000 > demo_test.txt
echo
echo "$ ls -lh demo_test.txt"
ls -lh demo_test.txt
echo
sleep 1

echo "Compressing file..."
echo "$ ./gsea -c -i demo_test.txt -o demo_test.rle"
./gsea -c -i demo_test.txt -o demo_test.rle
echo
echo "Comparing sizes:"
echo "$ ls -lh demo_test.txt demo_test.rle"
ls -lh demo_test.txt demo_test.rle
echo
sleep 2

echo "Decompressing file..."
echo "$ ./gsea -d -i demo_test.rle -o demo_test_restored.txt"
./gsea -d -i demo_test.rle -o demo_test_restored.txt
echo
echo "Verifying integrity:"
echo "$ diff demo_test.txt demo_test_restored.txt && echo 'Files match'"
diff demo_test.txt demo_test_restored.txt && echo 'Files match'
echo
sleep 2

# Demo 3: Encryption
echo "[Demo 3] File encryption"
echo "Creating file with sensitive data..."
echo "$ echo 'Información confidencial: Password123' > demo_secret.txt"
echo "Información confidencial: Password123" > demo_secret.txt
echo "$ cat demo_secret.txt"
cat demo_secret.txt
echo
sleep 1

echo "Encrypting file..."
echo "$ ./gsea -e -i demo_secret.txt -o demo_secret.enc"
./gsea -e -i demo_secret.txt -o demo_secret.enc
echo
echo "Attempting to read encrypted file:"
echo "$ cat demo_secret.enc"
cat demo_secret.enc
echo
sleep 2

echo "Decrypting file..."
echo "$ ./gsea -u -i demo_secret.enc -o demo_secret_restored.txt"
./gsea -u -i demo_secret.enc -o demo_secret_restored.txt
echo "$ cat demo_secret_restored.txt"
cat demo_secret_restored.txt
echo
sleep 2

# Demo 4: Directory processing
echo "[Demo 4] Directory compression with concurrent processing"
echo "Creating directory with multiple files..."
echo "$ mkdir -p demo_dir && cd demo_dir"
mkdir -p demo_dir
cd demo_dir
echo "$ for i in {1..10}; do yes 'data' | head -100 > file\$i.txt; done"
for i in {1..10}; do yes "data" | head -100 > file$i.txt; done
cd ..
echo "$ ls demo_dir/"
ls demo_dir/
echo
sleep 1

echo "Compressing directory with concurrent processing..."
echo "$ time ./gsea -c -i demo_dir/ -o demo_dir_compressed/"
time ./gsea -c -i demo_dir/ -o demo_dir_compressed/
echo
echo "$ ls demo_dir_compressed/"
ls demo_dir_compressed/
echo
sleep 2

# Cleanup
echo "Cleaning up demo files..."
rm -rf demo_test.txt demo_test.rle demo_test_restored.txt
rm -rf demo_secret.txt demo_secret.enc demo_secret_restored.txt
rm -rf demo_dir demo_dir_compressed

echo
echo "======================================"
echo "Demo completed"
echo "======================================"
