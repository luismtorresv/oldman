#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "fsutils.h"
#include "tar.h"
#include "gzip.h"
#include "aes256ctr.h"
#include "split.h"
#include "cloud_http.h"
#include "restore.h"

static void usage(void){
    printf("Usage:\n"
           "  oldman create --folders DIR... --out FILE [--threads N] [--encrypt yes|no]\n"
           "                    [--key HEX64] [--nonce HEX32] [--split-mb N] [--usb-dirs DIR...]\n"
           "                    [--http-put URL]\n"
           "  oldman restore (--input FILE | --fragments PARTS... ) --dest DIR [--key HEX64] [--nonce HEX32]\n");
}
static int parse_hex(const char* s, unsigned char* out, size_t outlen){
    size_t n=strlen(s); if(n!=outlen*2) return -1;
    for(size_t i=0;i<outlen;i++){ unsigned v; if(sscanf(s+2*i,"%2x",&v)!=1) return -1; out[i]=(unsigned char)v; }
    return 0;
}
int main(int argc, char** argv){
    if(argc<2){ usage(); return 0; }
    if(strcmp(argv[1],"create")==0){
        int i=2; char** folders=NULL; size_t nfolders=0; const char* out_file=NULL; int threads=omp_get_max_threads();
        int encrypt=0; unsigned char key[32]={0}, nonce[16]={0}; size_t split_mb=0; char** usb_dirs=NULL; size_t n_usb=0; const char* http_url=NULL;
        while(i<argc){
            const char* k=argv[i++];
            if(strcmp(k,"--folders")==0){ while(i<argc && argv[i][0]!='-'){ folders=(char**)realloc(folders,(nfolders+1)*sizeof(char*)); folders[nfolders++]=argv[i++]; } }
            else if(strcmp(k,"--out")==0) out_file=argv[i++];
            else if(strcmp(k,"--threads")==0) threads=atoi(argv[i++]);
            else if(strcmp(k,"--encrypt")==0) encrypt=(strcmp(argv[i++],"yes")==0);
            else if(strcmp(k,"--key")==0){ if(parse_hex(argv[i++],key,32)!=0){ fprintf(stderr,"Invalid --key\n"); return 1;} }
            else if(strcmp(k,"--nonce")==0){ if(parse_hex(argv[i++],nonce,16)!=0){ fprintf(stderr,"Invalid --nonce\n"); return 1;} }
            else if(strcmp(k,"--split-mb")==0) split_mb=(size_t)atol(argv[i++]);
            else if(strcmp(k,"--usb-dirs")==0){ while(i<argc && argv[i][0]!='-'){ usb_dirs=(char**)realloc(usb_dirs,(n_usb+1)*sizeof(char*)); usb_dirs[n_usb++]=argv[i++]; } }
            else if(strcmp(k,"--http-put")==0) http_url=argv[i++];
            else { fprintf(stderr,"Unknown option: %s\n", k); return 1; }
        }
        if(!folders || !out_file){ usage(); return 1; }
        if(encrypt && (memcmp(key,"\0",32)==0 || memcmp(nonce,"\0",16)==0)){ fprintf(stderr,"Encryption enabled but --key/--nonce missing.\n"); return 1; }
        omp_set_num_threads(threads);
        char** files=NULL; size_t nfiles=0; if(fs_enum_files((const char**)folders,nfolders,&files,&nfiles)!=0){ fprintf(stderr,"Enumerate failed\n"); return 1; }
        const char* tmp_tar="/mnt/data/_oldman_tmp.tar"; if(tar_create((const char**)files,nfiles,tmp_tar)!=0){ fprintf(stderr,"tar_create failed\n"); fs_free_list(files,nfiles); return 1; }
        fs_free_list(files,nfiles);
        size_t ntar=0; unsigned char* tarbuf=fs_read_all(tmp_tar,&ntar); if(!tarbuf){ fprintf(stderr,"read tar failed\n"); return 1; }
        unsigned char* gz=(unsigned char*)malloc(ntar*2+4096); size_t ngz=gzip_compress_fixed(tarbuf,ntar,gz,ntar*2+4096); free(tarbuf); if(ngz==0){ fprintf(stderr,"gzip failed\n"); free(gz); return 1; }
        if(encrypt){ aes256ctr_t ctx; aes256ctr_init(&ctx,key,nonce); aes256ctr_xor(&ctx,gz,ngz); }
        if(fs_write_all(out_file,gz,ngz)!=0){ fprintf(stderr,"write output failed\n"); free(gz); return 1; }
        fprintf(stderr,"[CREATE] wrote %s (%zu bytes)\n", out_file, (size_t)ngz);
        #pragma omp parallel sections
        {
            #pragma omp section
            { if(split_mb>0 && n_usb>0) split_file_round_robin(out_file, split_mb*1024*1024ULL, (const char**)usb_dirs, n_usb); }
            #pragma omp section
            { if(http_url) if(http_put_file(http_url,out_file)!=0) fprintf(stderr,"[CLOUD] upload failed\n"); }
        }
        free(gz); return 0;
    } else if(strcmp(argv[1],"restore")==0){
        int i=2; const char* input=NULL; char** parts=NULL; size_t nparts=0; const char* dest=NULL; unsigned char key[32]={0}, nonce[16]={0}; int have_key=0;
        while(i<argc){
            const char* k=argv[i++];
            if(strcmp(k,"--input")==0) input=argv[i++];
            else if(strcmp(k,"--fragments")==0){ while(i<argc && argv[i][0]!='-'){ parts=(char**)realloc(parts,(nparts+1)*sizeof(char*)); parts[nparts++]=argv[i++]; } }
            else if(strcmp(k,"--dest")==0) dest=argv[i++];
            else if(strcmp(k,"--key")==0){ if(parse_hex(argv[i++],key,32)!=0){ fprintf(stderr,"Invalid --key\n"); return 1;} have_key=1; }
            else if(strcmp(k,"--nonce")==0){ if(parse_hex(argv[i++],nonce,16)!=0){ fprintf(stderr,"Invalid --nonce\n"); return 1;} }
            else { fprintf(stderr,"Unknown %s\n", k); return 1; }
        }
        if(!dest || (!input && nparts==0)){ usage(); return 1; }
        int rc=restore_flow(input,(const char**)parts,nparts,have_key?key:NULL,nonce,dest); if(rc!=0){ fprintf(stderr,"restore failed\n"); return 1; }
        return 0;
    } else { usage(); return 1; }
}
