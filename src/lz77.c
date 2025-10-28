#include "lz77.h"
#include <string.h>
#define WSIZE 32768
#define MAXLEN 258
#define MINMATCH 3
size_t lz77_greedy(const unsigned char* in, size_t n, lz77_item_t* out){
    size_t i=0,k=0;
    while(i<n){
        size_t best_len=0,best_dist=0;
        size_t wstart = (i>WSIZE)?(i-WSIZE):0;
        size_t maxlen = (n-i)>MAXLEN?MAXLEN:(n-i);
        for(size_t j=i;j>wstart;--j){
            size_t dist = i-(j-1), l=0;
            while(l<maxlen && in[j-1+l]==in[i+l]) l++;
            if(l>=MINMATCH && l>best_len){ best_len=l; best_dist=dist; if(l==MAXLEN) break; }
        }
        if(best_len>=MINMATCH){ out[k].pos=i; out[k].len=(int)best_len; out[k].dist=(int)best_dist; out[k].lit=0; i+=best_len; }
        else { out[k].pos=i; out[k].len=0; out[k].dist=0; out[k].lit=in[i]; i+=1; }
        k++;
    }
    return k;
}
