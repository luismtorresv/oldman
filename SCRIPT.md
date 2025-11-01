---
title: "Script de video demostrativo para GSEA"
author: "Equipo de Desarrollo"
date: "31 de octubre de 2025"
lang: es-ES
documentclass: article
geometry: margin=1in
fontsize: 11pt
---

## Minuto 0-1: Introducción
**Mostrar**: Terminal con comando `./gsea -h`
**Explicar**: Capacidades del programa, objetivo del proyecto

## Minuto 1-3: Demo de Compresión
**Mostrar**:
- Crear archivo de prueba con datos repetitivos: `yes "test data" | head -1000 > test.txt`
- Ver tamaño: `ls -lh test.txt`
- Comprimir: `./gsea -c -i test.txt -o test.rle`
- Comparar tamaños: `ls -lh test.txt test.rle`
- Descomprimir: `./gsea -d -i test.rle -o test_restored.txt`
- Verificar: `diff test.txt test_restored.txt`

## Minuto 3-5: Demo de Encriptación
**Mostrar**:
- Crear archivo de texto: `echo "Información confidencial" > secret.txt`
- Mostrar contenido: `cat secret.txt`
- Encriptar: `./gsea -e -i secret.txt -o secret.enc`
- Intentar leer: `cat secret.enc` (mostrar caracteres ilegibles)
- Desencriptar: `./gsea -u -i secret.enc -o secret_restored.txt`
- Verificar: `cat secret_restored.txt`

## Minuto 5-7: Demo de Directorio
**Mostrar**:
- Crear directorio con múltiples archivos: `mkdir testdir && cd testdir && for i in {1..10}; do yes "data" | head -100 > file$i.txt; done && cd ..`
- Comprimir directorio: `time ./gsea -c -i testdir/ -o testdir_compressed/`
- Mostrar archivos procesados: `ls testdir_compressed/`
- Comparar tiempos con procesamiento secuencial vs paralelo

## Minuto 7-8: Código - Llamadas al Sistema
**Mostrar**: Archivo `file_ops.c`
**Explicar**:
- Función `read_file_syscall()`: uso de `open()`, `fstat()`, `read()`, `close()`
- Función `write_file_syscall()`: uso de `open()` con flags, `write()`, `close()`
- Ventajas de syscalls vs funciones de biblioteca

## Minuto 8-9: Código - Concurrencia
**Mostrar**: Archivo `worker.c`
**Explicar**:
- Función `process_directory()`: uso de `opendir()`, `readdir()`
- Creación de threads: `pthread_create()` para cada archivo
- Sincronización: `pthread_join()` para esperar completación
- Estructura `thread_data_t` para pasar datos a threads

## Minuto 9-10: Código - Algoritmos
**Mostrar**: Archivos `compress.c` y `encrypt.c`
**Explicar**:
- RLE: lógica de detección de runs y encoding
- Vigenère: operación simple de suma/resta con clave
- Trade-offs: simplicidad vs compresión/seguridad

## Conclusión
**Resumir**: Proyecto cumple objetivos de eficiencia, concurrencia y uso de syscalls
**Mostrar**: Posibles extensiones (más algoritmos, compresión+encriptación
combinada)
