# GSEA - Ejemplos de Uso

## Comprimir un archivo individual

```bash
./gsea -c -i documento.txt -o documento.txt.rle
```

## Descomprimir un archivo

```bash
./gsea -d -i documento.txt.rle -o documento_restaurado.txt
```

## Encriptar un archivo

```bash
./gsea -e -i datos_sensibles.csv -o datos_sensibles.enc
```

## Desencriptar un archivo

```bash
./gsea -u -i datos_sensibles.enc -o datos_sensibles.csv
```

## Comprimir un directorio completo

```bash
./gsea -c -i ./logs/ -o ./logs_compressed/
```

El programa creará automáticamente el directorio de salida y procesará todos los archivos en paralelo usando threads.

## Encriptar un directorio completo

```bash
./gsea -e -i ./documentos/ -o ./documentos_encriptados/
```

## Caso de uso combinado

Para comprimir y luego encriptar (requiere dos pasos):

```bash
# Paso 1: Comprimir
./gsea -c -i datos/ -o datos_comprimidos/

# Paso 2: Encriptar
./gsea -e -i datos_comprimidos/ -o datos_seguros/
```

Para restaurar:

```bash
# Paso 1: Desencriptar
./gsea -u -i datos_seguros/ -o datos_desencriptados/

# Paso 2: Descomprimir
./gsea -d -i datos_desencriptados/ -o datos_restaurados/
```

## Notas importantes

- La clave de encriptación por defecto es "gsea2025" (definida en `src/encrypt.h`)
- El algoritmo de compresión RLE funciona mejor con datos repetitivos
- El procesamiento de directorios crea un thread por cada archivo (máximo 256 threads)
- Los archivos de salida se crean con permisos 0644
- Los directorios de salida se crean con permisos 0755
