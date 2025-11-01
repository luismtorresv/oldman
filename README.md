# `gsea` - Final project/exam for Operating Systems class

Command-line utility written in C for compressing and decompressing files or
directories with symmetric encryption.

# More in-depth description

This project implements a command-line backup system in C (C11) with OpenMP and
no third-party dependencies. It creates USTAR archives, applies GZIP (Deflate;
fixed Huffman) compression with a custom encoder, optionally encrypts with
AES‑256‑CTR (in-house implementation), supports parallel splitting into USB
fragments, and includes a minimal HTTP PUT uploader for on‑premises servers
(plain HTTP).

## Build

````bash
make
````

This builds the gsea binary. Invoke:

````bash
./gsea
````

## Usage

### Create a backup

````bash
./gsea create \
  --folders /path/dir1 /path/dir2 \
  --out /path/output.sbk \
  --threads 8 \
  --encrypt yes \
  --key  00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff \
  --nonce 00000000000000000000000000000001 \
  --split-mb 4096 \
  --usb-dirs /media/USB1 /media/USB2 \
  --http-put http://127.0.0.1:8080/upload/backup.sbk
````

### Restore a backup

````bash
./gsea restore \
  --input /path/output.sbk \
  --dest /path/restore \
  --key  00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff \
  --nonce 00000000000000000000000000000001
````

### Restore from fragments

````bash
./gsea restore \
  --fragments /media/USB1/output.sbk.part_000000 /media/USB2/output.sbk.part_000001 \
  --dest /path/restore \
  --key  00112233445566778899aabbccddeeff00112233445566778899aabbccddeeff \
  --nonce 00000000000000000000000000000001
````

## Notes

- Compression: Deflate (fixed Huffman) with a GZIP wrapper.
- Parallelism: OpenMP for USB splitting and concurrent HTTP upload. The encoder
  is single-pass; additional parallelism is possible by partitioning the TAR
  into multiple members.
- AES‑256‑CTR provides confidentiality; integrity/authentication are out of
  scope.
- The inflater supports only fixed‑Huffman streams produced by this encoder.
- The HTTP uploader supports only `http://` for demonstration.
