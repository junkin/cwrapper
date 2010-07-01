#ifndef _AOL_TRANSPORT_H
#define _AOL_TRANSPORT_H

#include "atmos_util.h"
typedef struct credentialsval {
    char tokenid[120];
    char secret[120];
    char accesspoint[120];
} credentials;


typedef struct PD {
    char *data;
    size_t body_size;
    size_t bytes_remaining;
    size_t offset;
    size_t bytes_written;
}postdata;

typedef struct hdrval {
    void *header_data;
    size_t header_size;
    void *next_header;
} hdr;

#define MAX_HEADERS 1024

typedef struct ws_result {
    int return_code;
    char *response_body;
    size_t body_size;
    char *headers[MAX_HEADERS];
    int header_count;
} ws_result;

#define true 1
#define false 0

#define HTTP_HDR_SIZE 4096
const char *http_request(credentials *c,http_method method, char *uri,char * content_type, char **headers, int header_count, postdata* a, ws_result* ws_result) ;
const char *http_request_ns(credentials *c,http_method method, char *uri, char *content_type, char **headers, int header_count, postdata* a, ws_result *ws_result) ;
void ws_init(ws_result*);
void ws_deinit(ws_result*);
void result_deinit(ws_result *result);
void result_init(ws_result *result);
#endif

