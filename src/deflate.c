#include "deflate.h"
static int put_fixed_ll(bitwriter_t* bw, int sym){
    if(sym>=0 && sym<=143) return bw_put(bw, (unsigned)(sym+0x30), 8);
    else if(sym>=144 && sym<=255) return bw_put(bw, (unsigned)(sym-144+0x190), 9);
    else if(sym>=256 && sym<=279) return bw_put(bw, (unsigned)(sym-256), 7);
    else if(sym>=280 && sym<=287) return bw_put(bw, (unsigned)(sym-280+0xC0), 8);
    return 0;
}
static int put_dist_code(bitwriter_t* bw, int code){ return bw_put(bw, (unsigned)code, 5); }
int deflate_len_to_code(int L, int* ebits, int* eval){
    if(L<3||L>258) return -1;
    static const int base[29]={3,4,5,6,7,8,9,10,11,13,15,17,19,23,27,31,35,43,51,59,67,83,99,115,131,163,195,227,258};
    static const int eb[29]={0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0};
    for(int i=0;i<29;i++){ int b=base[i], next=(i==28)?259:base[i+1]; if(L>=b && L<next){ *ebits=eb[i]; *eval=L-b; return 257+i; } }
    return -1;
}
int deflate_dist_to_code(int D, int* ebits, int* eval){
    if(D<1||D>32768) return -1;
    static const int base[30]={1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577};
    static const int eb[30]={0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13};
    for(int i=0;i<30;i++){ int b=base[i], next=(i==29)?32769:base[i+1]; if(D>=b && D<next){ *ebits=eb[i]; *eval=D-b; return i; } }
    return -1;
}
size_t deflate_fixed_block(const lz77_item_t* items, size_t nitems, unsigned char* outbuf, size_t outcap){
    bitwriter_t bw; bw_init(&bw, outbuf, outcap);
    if(!bw_put(&bw,1,1)) return 0; /* BFINAL=1 */
    if(!bw_put(&bw,1,2)) return 0; /* BTYPE=01 */
    for(size_t i=0;i<nitems;i++){
        if(items[i].len==0){
            if(!put_fixed_ll(&bw, (int)items[i].lit)) return 0;
        }else{
            int ebL=0, evL=0, code=deflate_len_to_code(items[i].len,&ebL,&evL);
            if(code<0) return 0;
            if(!put_fixed_ll(&bw, code)) return 0;
            if(ebL) if(!bw_put(&bw,(unsigned)evL,ebL)) return 0;
            int ebD=0, evD=0, dcode=deflate_dist_to_code(items[i].dist,&ebD,&evD);
            if(dcode<0) return 0;
            if(!put_dist_code(&bw, dcode)) return 0;
            if(ebD) if(!bw_put(&bw,(unsigned)evD,ebD)) return 0;
        }
    }
    if(!put_fixed_ll(&bw,256)) return 0; /* end */
    if(!bw_flush(&bw)) return 0;
    return bw_size(&bw);
}
