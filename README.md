# Utilidad de Gestión Segura y Eficiente de Archivos (GSEA)

> [!IMPORTANT]
>
> Enlace al vídeo de sustentación: <https://youtu.be/3XSrHHPwwfo>

Utilidad de línea de comandos escrita en C (estándar 11) que permite a un
usuario, de una máquina corriendo Linux, comprimir/descomprimir y
encriptar/desencriptar archivos o incluso directorios completos de manera
eficiente.

## Características

- Compresión y descompresión con algoritmo RLE (Run-Length Encoding)
- Encriptación y desencriptación con cifrado Vigenère
- Procesamiento concurrente de múltiples archivos usando threads POSIX
- Operaciones de E/S mediante llamadas al sistema (open, read, write, close)
- Soporte para archivos individuales y directorios completos

## Cómo compilar

```bash
make
```

Requisitos:
- gcc con soporte para C11
- pthread library
- Sistema operativo Linux

## Uso

### Sintaxis general

```bash
./gsea [OPCIONES] -i [entrada] -o [salida]
```

### Ejemplos

Comprimir un archivo:
```bash
./gsea -c -i archivo.txt -o archivo.rle
```

Descomprimir un archivo:
```bash
./gsea -d -i archivo.rle -o archivo.txt
```

Encriptar un archivo:
```bash
./gsea -e -i documento.pdf -o documento.enc
```

Desencriptar un archivo:
```bash
./gsea -u -i documento.enc -o documento.pdf
```

Procesar un directorio (compresión concurrente):
```bash
./gsea -c -i ./datos/ -o ./datos_comprimidos/
```

### Opciones disponibles

- `-c` : Comprimir entrada
- `-d` : Descomprimir entrada
- `-e` : Encriptar entrada
- `-u` : Desencriptar entrada
- `-i` : Ruta de archivo o directorio de entrada
- `-o` : Ruta de archivo o directorio de salida
- `--comp-alg` : Algoritmo de compresión (por defecto: rle)
- `--enc-alg` : Algoritmo de encriptación (por defecto: vigenere)
- `-h` : Mostrar ayuda

## Arquitectura

El proyecto está organizado en módulos con responsabilidades específicas:

- `main.c` - Punto de entrada y parseo de argumentos CLI
- `compress.c/h` - Implementación de compresión RLE
- `encrypt.c/h` - Implementación de cifrado Vigenère
- `file_ops.c/h` - Operaciones de archivo con syscalls
- `worker.c/h` - Gestión de concurrencia y threads

## Documentación

Consultar `REPORT.md` para documentación técnica detallada sobre:
- Diseño de la solución
- Justificación de algoritmos
- Estrategia de concurrencia
- Manual completo de usuario

## Licencia

No aplica pues es un proyecto académico.
