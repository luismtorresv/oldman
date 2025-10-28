# Technical Report

- Language: C (C11)
- Parallelism: OpenMP
- Conventional compression: GZIP (Deflate, fixed codes)
- Optional encryption: AES-256-CTR (custom implementation)
- Output: Single file with optional chunking and HTTP upload

1. Archiving (minimal TAR/USTAR).
2. Conventional compression: Deflate with fixed codes and a GZIP wrapper with
   CRC32/ISIZE.
3. Encryption: AES-256-CTR.
4. Transfers: parallel USB chunking and HTTP PUT without TLS.
5. Restoration: read/assemble, decrypt if applicable, decompress, TAR
   extraction.

- Parallelism: `#pragma omp parallel for` for chunking and `#pragma omp parallel
  sections` to run chunking and HTTP PUT concurrently.

- Limitations: minimalist Deflate encoder; inflater for fixed-code streams; HTTP
without TLS; no MAC.
