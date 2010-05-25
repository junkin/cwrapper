#ifndef _AOL_TRANSPORT_H
#define _AOL_TRANSPORT_H


typedef struct credentialsval {
    char tokenid[120];
    char secret[120];
    char accesspoint[120];
} credentials;

typedef enum http_methodval   {
	POST,
	GET,
	PUT,
	DELETE,
	HEAD,
	OPTIONS
} http_method;

typedef struct PD {
    void *data;
    int datsize;
} postdata;

typedef struct hdrval {
    void *header_data;
    size_t header_size;
    void *next_header;
} hdr;

#define true 1
#define false 0

#define HTTP_HDR_SIZE 4096
const char *http_request(credentials *c,http_method method, char *path,char **headers, int header_count, postdata* a) ;
const char *http_request_ns(credentials *c,http_method method, char *path,char **headers, int header_count, postdata* a) ;

#endif
