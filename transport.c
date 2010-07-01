#include <stdlib.h>
#include <string.h>

#include "curl/curl.h"
#include "transport.h"
#include "atmos_util.h"
#include "crypto.h"

static const char *namespace_uri = "/rest/namespace";
//static const char *object_uri = "/rest/objects";


size_t readfunc(void *ptr, size_t size, size_t nmemb, void *stream)
{

    if(stream) {
	postdata *ud = (postdata*)stream;
	
	if(ud->bytes_remaining) {
	    if(ud->bytes_remaining > size*nmemb) {
		memcpy((char*)ptr, (char*)(ud->data+ud->bytes_written), size*nmemb);
		ud->bytes_written+=size+nmemb;
		ud->bytes_remaining -=size*nmemb;
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
	void *new_response = NULL;
	size_t data_offset = ws->body_size;
    size_t mem_required = size*nmemb;
    
    ws->body_size+= mem_required;
    new_response = realloc(ws->response_body, ws->body_size);
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
	size_t mem_required = size*nmemb-2;

    ws->headers[ws->header_count] = malloc(mem_required+1);
    memcpy(ws->headers[ws->header_count],ptr, mem_required);
    ws->headers[ws->header_count][mem_required] = '\0';
    ws->header_count++;
    //                            
    return size*nmemb;
}

void result_init(ws_result *result) {
    result->return_code = -1;
    result->response_body = NULL;
    result->body_size = 0;
   memset(result->headers, 0,sizeof(result->headers));
    result->header_count=0;
}

void result_deinit(ws_result* result) {
	int i = 0;
	free(result->response_body);
    for(; i < result->header_count; i++) {
	free(result->headers[i]);
    }
}

const char *http_request_ns(credentials *c, http_method method, char *uri,char *content_type, char **headers, int header_count, postdata * data, ws_result* ws_result) {
	
    char * ns_uri = NULL;
    ns_uri = (char*)malloc(strlen(uri)+strlen(namespace_uri)+1);    
    sprintf(ns_uri,"%s%s",namespace_uri, uri);
    http_request(c, method, ns_uri, content_type, headers, header_count, data, ws_result);    
    free((char*)ns_uri);
    return NULL;
}

const char *http_request(credentials *c, http_method method, char *uri, char *content_type, char **headers, int header_count, postdata *data, ws_result* ws_result) 
{

  //    if(!curl_code) {
  //;
  //} else {

      CURL  *curl = curl_easy_init();
    CURLcode result_code;
    //struct curl_httppost *formpost=NULL;
    const int connect_timeout = 200;
    char date[256];
    char uidheader[1024];
    char dateheader[1024];
    char groupaclheader[1024];
    char *endpoint_url = NULL;
    size_t endpoint_size;
    char errorbuffer[1024*1024];
    struct curl_slist *chunk = NULL;
    char hash_string[1024];
    char range[1024];
    char signature[1024];
    char content_type_header[1024];
    int i;
    char *signed_hash = NULL;
    char content_length_header[1024];
    char range_header[1024];
    //CURLcode curl_code = 
    curl_global_init(CURL_GLOBAL_ALL);

    memset(range, 0, 1024);
    get_date(date);
    snprintf(dateheader,1024,"X-Emc-Date:%s", date);
    snprintf(uidheader,1024,"X-Emc-Uid:%s",c->tokenid);    
    snprintf(groupaclheader,1024,"X-Emc-groupacl:other=NONE");    
    headers[header_count++] = dateheader;
    //FIXME groupacl headers breaks sig string
    //headers[header_count++] = groupaclheader;
    headers[header_count++] = uidheader;

    if (!content_type) {
	content_type = "application/octet-stream";
    }
    
    
    endpoint_size = strlen(c->accesspoint)+strlen(uri) +1;
    endpoint_url = (char*)malloc(endpoint_size);
    
    snprintf(endpoint_url, endpoint_size, "%s%s", c->accesspoint, uri);

    // set up flags this should move into transport layercyrk
    if (curl) {
	//curl_version_info_data *version_data = curl_version_info(CURLVERSION_NOW);

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
	case aDELETE:
	    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE"); 
	    break;
	case HEAD:

	    break;
	case GET:
	  break;
	case OPTIONS:
	  break;
	}

	if(data) {
	  if(data->offset) {
	    snprintf(range, 1024, 
		     "Bytes=%d-%d", data->offset,data->offset+data->body_size-1);
	  } else if(data->body_size) {
	    snprintf(range, 1024, "Bytes=0-%d", data->body_size-1);
	  }
	}
 
	build_hash_string(hash_string, method, content_type, range,NULL,uri, headers,header_count);
	if(data){
	  data->bytes_written=0;
	  data->bytes_remaining=data->body_size;
	  if(method != GET) {
	    snprintf(content_length_header,1024, "content-length: %zu", data->body_size);
	    headers[header_count++]=content_length_header;
	  }
	  
	  if(data->offset > 0) {
	    snprintf(range_header, 1024, "range: Bytes=%d-%d", data->offset,data->offset+data->body_size-1);
	    headers[header_count++] = range_header;
	  }
	}

	for(i=0;i<header_count; i++) {
	    chunk = curl_slist_append(chunk, headers[i]);	
	}
	

	signed_hash = (char*)sign(hash_string,c->secret);
	snprintf(signature,1024,"X-Emc-Signature:%s", signed_hash);
	free(signed_hash);
	snprintf(content_type_header, 1024,"content-type:%s", content_type); 
	curl_slist_append(chunk,"Expect:");
	curl_slist_append(chunk,"Transfer-Encoding:");
	curl_slist_append(chunk, content_type_header);
	curl_slist_append(chunk,signature);
	result_code = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
	result_code = curl_easy_perform(curl);
	
	
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &ws_result->return_code);
	curl_easy_cleanup(curl);
	curl_slist_free_all(chunk);
    }
    free(endpoint_url);
    return  NULL;
}
