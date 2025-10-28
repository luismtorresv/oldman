#include "cloud_http.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

static int parse_url(const char* url, char* host, size_t host_sz, int* port, char* path, size_t path_sz){
    if(strncmp(url,"http://",7)!=0) return -1; const char* p=url+7; const char* slash=strchr(p,'/'); if(!slash) return -1;
    const char* colon=strchr(p,':'); if(colon && colon<slash){ snprintf(host,host_sz,"%.*s",(int)(colon-p),p); *port=atoi(colon+1); }
    else { snprintf(host,host_sz,"%.*s",(int)(slash-p),p); *port=80; }
    snprintf(path,path_sz,"%s",slash); return 0;
}
int http_put_file(const char* url, const char* filepath){
    char host[256], path[1024]; int port;
    if(parse_url(url,host,sizeof(host),&port,path,sizeof(path))!=0){ fprintf(stderr,"[CLOUD] Only http:// URLs.\n"); return -1; }
    struct hostent* he=gethostbyname(host); if(!he) return -1;
    int sock=socket(AF_INET,SOCK_STREAM,0); if(sock<0) return -1;
    struct sockaddr_in addr; memset(&addr,0,sizeof(addr)); addr.sin_family=AF_INET; addr.sin_port=htons((uint16_t)port); memcpy(&addr.sin_addr,he->h_addr_list[0],he->h_length);
    if(connect(sock,(struct sockaddr*)&addr,sizeof(addr))!=0){ close(sock); return -1; }
    FILE* f=fopen(filepath,"rb"); if(!f){ close(sock); return -1; } fseek(f,0,SEEK_END); long sz=ftell(f); fseek(f,0,SEEK_SET);
    char header[2048]; int hdrlen=snprintf(header,sizeof(header),"PUT %s HTTP/1.1\r\nHost: %s\r\nContent-Length: %ld\r\nConnection: close\r\n\r\n",path,host,sz);
    send(sock,header,hdrlen,0);
    char buf[4096]; size_t r; while((r=fread(buf,1,sizeof(buf),f))>0){ send(sock,buf,r,0); } fclose(f);
    int n=recv(sock,buf,sizeof(buf)-1,0); if(n>0){ buf[n]=0; fprintf(stderr,"[CLOUD] Response: %s\n", buf); }
    close(sock); return 0;
}
