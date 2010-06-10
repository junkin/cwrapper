#include <stdlib.h>
#include <string.h>

#include "curl/curl.h"
#include "transport.h"

static const char *namespace_uri = "/rest/namespace";
static const char *object_uri = "/rest/objects";

size_t readfunc(void *ptr, size_t size, size_t nmemb, void *stream)
{

    if(stream) {
	postdata *ud = (postdata*)stream;
	
	if(ud->bytes_remaining) {
	    if(ud->body_size > size*nmemb) {
		memcpy(ptr, ud->data+ud->bytes_written, size*nmemb);
		ud->bytes_written+=size+nmemb;
		ud->bytes_remaining = ud->body_size-size*nmemb;
		return size*nmemb;
	    } else {
		memcpy(ptr, ud->data+ud->bytes_written, ud->bytes_remaining);
		ud->bytes_remaining=0;
		return size*nmemb;;
	    }
	}
    }
    return 0;
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, void *stream)
{
    ws_result *ws = (ws_result*)stream;

    int data_offset = ws->body_size;
    int mem_required = size*nmemb;
    
    ws->body_size+= mem_required;
    void * new_response = realloc(ws->response_body, ws->body_size);
    if(new_response) {
	ws->response_body = new_response;
    } else {
	printf("alloc error... ");
    }
    
    memcpy(ws->response_body+data_offset,ptr, mem_required);
    return size*nmemb;
}

size_t headerfunc(void *ptr, size_t size, size_t nmemb, void *stream)
{
    ws_result *ws = (ws_result*)stream;
    int data_offset = ws->header_size;
    int mem_required = size*nmemb;

    ws->header_size+= mem_required;
    void *new_response = realloc(ws->headers, ws->header_size);

    if(new_response) {
	ws->headers= new_response;
    } else {
	printf("alloc error... ");
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

    if(data){
	data->bytes_written=0;
	data->bytes_remaining=data->body_size;
    }
    char *ns_uri = (char*)malloc(strlen(uri)+strlen(namespace_uri)+1);
    
    sprintf(ns_uri,"%s%s",namespace_uri, uri);
    http_request(c, method, ns_uri, content_type, headers, header_count, data, ws_result);    
    free((char*)ns_uri);

}

const char *http_request(credentials *c, http_method method, char *uri, char *content_type, char **headers, int header_count, postdata *data, ws_result* ws_result) 
{
    char end_url[256];
    CURLcode curl_code = curl_global_init(CURL_GLOBAL_ALL);
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
    // set up flags this should move into transport layercyrk
    if (curl) {
	curl_version_info_data *version_data = curl_version_info(CURLVERSION_NOW);

	struct curl_slist *chunk = NULL;

	curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);
	curl_easy_setopt(curl, CURLOPT_URL, endpoint_url);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 0);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, connect_timeout);
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorbuffer);
	curl_easy_setopt(curl, CURLOPT_FAILONERROR, false);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &writefunc);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &headerfunc);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, ws_result);
	curl_easy_setopt(curl, CURLOPT_WRITEHEADER, ws_result);
	
	switch(method) {

	case POST:
	    curl_easy_setopt(curl, CURLOPT_HTTPPOST, 1l);	
	    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, 0L);
	    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, NULL);

	    break;
	case PUT:
	    curl_easy_setopt(curl, CURLOPT_PUT, 1L); 
	    curl_easy_setopt(curl, CURLOPT_READDATA, data);
	    curl_easy_setopt(curl, CURLOPT_READFUNCTION, readfunc);
	    
	    break;
	case DELETE:
	    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE"); 
	    break;
	case HEAD:

	    break;
	}
	
	char hash_string[1024];
	char * range = NULL;//FIXME
	int hash_length = build_hash_string(hash_string, method, content_type, range,NULL,uri, headers,header_count);
	char signature[1024];
	char content_type_header[1024];

	
	int i;
	for(i=0;i<header_count; i++) {
	    chunk = curl_slist_append(chunk, headers[i]);	
	}
	
	char *signed_hash = (char*)sign(hash_string,c->secret);
	snprintf(signature,1024,"X-Emc-Signature:%s", signed_hash);
	free(signed_hash);
	snprintf(content_type_header, 1024,"content-type:%s", content_type); 
	curl_slist_append(chunk,"Expect:");
	curl_slist_append(chunk,"Transfer-Encoding:");
	curl_slist_append(chunk, content_type_header);
	curl_slist_append(chunk,signature);


	if(data) {
	    char content_length_header[1024];
	    snprintf(content_length_header,1024, "content-length: %d", data->body_size);
	    curl_slist_append(chunk,content_length_header);
		
	}
	result_code = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
	result_code = curl_easy_perform(curl);
	
	int http_response_code = 0;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &ws_result->return_code);
	curl_easy_cleanup(curl);
	curl_slist_free_all(chunk);
    }
    free(endpoint_url);
}
