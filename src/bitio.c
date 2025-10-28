#include "bitio.h"
void bw_init(bitwriter_t* bw, unsigned char* buf, size_t cap){ bw->buf=buf; bw->cap=cap; bw->pos=0; bw->bitbuf=0; bw->bitcount=0; }
int  bw_put(bitwriter_t* bw, unsigned v, int nb){
    for(int i=0;i<nb;i++){ bw->bitbuf |= ((v>>i)&1u) << bw->bitcount; bw->bitcount++; if(bw->bitcount==8){ if(bw->pos>=bw->cap) return 0; bw->buf[bw->pos++]=(unsigned char)(bw->bitbuf&0xFFu); bw->bitbuf=0; bw->bitcount=0; } }
    return 1;
}
int  bw_flush(bitwriter_t* bw){ if(bw->bitcount>0){ if(bw->pos>=bw->cap) return 0; bw->buf[bw->pos++]=(unsigned char)(bw->bitbuf&0xFFu); bw->bitbuf=0; bw->bitcount=0; } return 1; }
size_t bw_size(const bitwriter_t* bw){ return bw->pos; }
void br_init(bitreader_t* br, const unsigned char* buf, size_t len){ br->buf=buf; br->len=len; br->pos=0; br->bitbuf=0; br->bitcount=0; }
int  br_need(bitreader_t* br, int nbits){ while(br->bitcount<nbits){ if(br->pos>=br->len) return 0; br->bitbuf |= ((unsigned)br->buf[br->pos++]) << br->bitcount; br->bitcount+=8; } return 1; }
unsigned br_get(bitreader_t* br, int nbits){ unsigned v= br->bitbuf & ((1u<<nbits)-1u); br->bitbuf >>= nbits; br->bitcount -= nbits; return v; }
