#include <stdlib.h>
#include <string.h>

#include "curl/curl.h"
#include "transport.h"

static const char *namespace_uri = "/rest/namespace";
static const char *object_uri = "/rest/objects";

size_t writefunc(void *ptr, size_t size, size_t nmemb, void *stream)
{
    ws_result *ws = (ws_result*)stream;

    int data_offset = ws->body_size;
    int mem_required = size*nmemb;
    
    if(ws->body_size== 0) {
	ws->body_size+=mem_required;
	ws->response_body = malloc(ws->body_size);
	
    } else {
	ws->body_size+= mem_required;
	ws->response_body = reallocf(ws->response_body, ws->body_size);
    }

    memcpy(ws->response_body+data_offset,ptr, mem_required);

    return size*nmemb;
}

size_t headerfunc(void *ptr, size_t size, size_t nmemb, void *stream)
{
    ws_result *ws = (ws_result*)stream;
    int data_offset = ws->header_size;
    int mem_required = size*nmemb;

    if(ws->header_size== 0) {
	ws->header_size+=mem_required;
	ws->headers = malloc(ws->header_size);
	
    } else {
	ws->header_size+= mem_required;
	ws->headers = reallocf(ws->headers, ws->header_size);
    }
    memcpy(ws->headers+data_offset,ptr, mem_required);
    return size*nmemb;

}


void result_init(ws_result *result) {
    result->return_code = -1;
    result->response_body = NULL;
    result->body_size = 0;
    result->headers = NULL;
    result->header_size=0;

}

void result_deinit(ws_result* result) {
    free(result->response_body);
    free(result->headers);
}
const char *http_request_ns(credentials *c, http_method method, char *uri,char *content_type, char **headers, int header_count, postdata * data, ws_result* ws_result) {

    char *ns_uri = (char*)malloc(strlen(uri)+strlen(namespace_uri));
    sprintf(ns_uri,"%s%s",namespace_uri, uri);
    http_request(c, method, ns_uri, content_type, headers, header_count, data, ws_result);    
    free((char*)ns_uri);
}

const char *http_request(credentials *c, http_method method, char *uri, char *content_type, char **headers, int header_count, postdata *data, ws_result* ws_result) 
{
    char end_url[256];
    CURL  *curl = curl_easy_init();
    CURLcode result_code;
    struct curl_httppost *formpost=NULL;
    const int connect_timeout = 200;
    char date[256];
    get_date(date);
    char dateheader[1024];
    snprintf(dateheader,1024,"X-Emc-Date:%s", date);
    char uidheader[1024];
    snprintf(uidheader,1024,"X-Emc-Uid:%s",c->tokenid);    
    char groupaclheader[1024];
    snprintf(groupaclheader,1024,"X-Emc-groupacl:other=NONE");    
    
    headers[header_count++] = dateheader;
    //FIXME groupacl headers breaks sig string
    //headers[header_count++] = groupaclheader;
    headers[header_count++] = uidheader;

    if (!content_type) {
	content_type = "application/octet-stream";
    }
    
    char *endpoint_url;
    int endpoint_size = strlen(c->accesspoint)+strlen(uri) +1;
    endpoint_url = (char*)malloc(endpoint_size);
    snprintf(endpoint_url, endpoint_size, "%s%s", c->accesspoint, uri);

    char errorbuffer[1024*1024];
    // set up flags this should move into transport layer
    if (curl) {

	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 0);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, connect_timeout);
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorbuffer);
	curl_easy_setopt(curl, CURLOPT_URL, endpoint_url);
	curl_easy_setopt(curl, CURLOPT_FAILONERROR, false);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &writefunc);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &headerfunc);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, ws_result);
	curl_easy_setopt(curl, CURLOPT_WRITEHEADER, ws_result);

	switch(method) {

	case POST:
	    curl_easy_setopt(curl, CURLOPT_POST, 1l);	
	    break;
	case PUT:
	    curl_easy_setopt(curl, CURLOPT_PUT, 1L); 
	    break;
	case DELETE:
	    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE"); 
	    break;
	case HEAD:

	    break;
	}
	
	char hash_string[1024];
	int hash_length = build_hash_string(hash_string, method, content_type,NULL,NULL,uri, headers,header_count);
	printf("\n\n%s\n\n", hash_string);
	char signature[1024];
	char content_type_header[1024];
	struct curl_slist *chunk = NULL;
	int i;
	for(i=0;i<header_count; i++) {
	    chunk = curl_slist_append(chunk, headers[i]);	
	}
	snprintf(signature,1024,"X-Emc-Signature:%s",sign(hash_string,c->secret));
	snprintf(content_type_header, 1024,"content-type:%s", content_type); 
	curl_slist_append(chunk,"Expect:");
	result_code = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
	curl_slist_append(chunk, content_type_header);
	curl_slist_append(chunk,signature);
	result_code = curl_easy_perform(curl);
	ws_result->return_code = result_code;
	curl_easy_cleanup(curl);
    }
    free(endpoint_url);
}
