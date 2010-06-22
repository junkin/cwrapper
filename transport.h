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
    size_t body_size;
    size_t bytes_remaining;
    size_t bytes_written;

} postdata;

typedef struct hdrval {
    void *header_data;
    size_t header_size;
    void *next_header;
} hdr;

#define MAX_HEADERS 1024

typedef struct ws_result {
    int return_code;
    void *response_body;
    int body_size;
    char *headers[MAX_HEADERS];
    int header_count;
} ws_result;

#define true 1
#define false 0

#define HTTP_HDR_SIZE 4096
const char *http_request(credentials *c,http_method method, char *uri,char * content_type, char **headers, int header_count, postdata* a, ws_result* ws_result) ;
void get_date(char *formated);
const char *http_request_ns(credentials *c,http_method method, char *uri, char *content_type, char **headers, int header_count, postdata* a, ws_result *ws_result) ;
void ws_init(ws_result*);
void ws_deinit(ws_result*);

int build_hash_string (char *hash_string, const http_method method, const char *content_type, const char *range,const char *date, const char *uri, char **emc_sorted_headers, const int header_count);
#endif

