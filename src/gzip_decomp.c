#include "gzip.h"
#include "inflate.h"
#include "crc32.h"
#include "bitio.h"
#include <string.h>
#include <stdlib.h>
size_t gzip_decompress_fixed(const unsigned char* in, size_t n, unsigned char* out, size_t outcap){
    if(n<18) return 0; if(!(in[0]==0x1f && in[1]==0x8b && in[2]==8)) return 0;
    unsigned flg=in[3]; size_t pos=10;
    if(flg&4){ if(pos+2>n) return 0; unsigned xlen=in[pos]|(in[pos+1]<<8); pos+=2+xlen; }
    if(flg&8){ while(pos<n && in[pos]!=0) pos++; pos++; }
    if(flg&16){ while(pos<n && in[pos]!=0) pos++; pos++; }
    if(flg&2){ pos+=2; }
    if(n<8) return 0; size_t end=n-8;
    uint32_t expect_crc=in[end]|(in[end+1]<<8)|(in[end+2]<<16)|(in[end+3]<<24);
    uint32_t expect_isize=in[end+4]|(in[end+5]<<8)|(in[end+6]<<16)|(in[end+7]<<24);
    bitreader_t br; br_init(&br,in+pos,end-pos);
    if(!br_need(&br,3)) return 0; unsigned bfinal=br_get(&br,1); unsigned btype=br_get(&br,2); (void)bfinal; if(btype!=1) return 0;
    size_t wrote=inflate_fixed_block(&br,out,outcap); if(wrote==0) return 0;
    uint32_t crc=crc32_update(0xFFFFFFFFu,out,wrote); if(crc!=expect_crc) return 0;
    if((wrote & 0xFFFFFFFFu)!=expect_isize) return 0; return wrote;
}
