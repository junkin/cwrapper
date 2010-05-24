#include "curl/curl.h"
#include "transport.h"

size_t writefunc(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    printf("%s\n", (char*)ptr);
}

size_t headerfunc(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    printf("headerfunc CB: %s\n", (char*)ptr);
}


const char *http_request(credentials *c, http_method method, char *uri, char **headers, int header_count, postdata *data) 
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
    headers[header_count++] = uidheader;
    //headers[header_count++] = groupaclheader;
    //headers[header_count++] = groupaclheader;    

    char errorbuffer[1024*1024];
    // set up flags this should move into transport layer
    if (curl) {
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, connect_timeout);
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorbuffer);
	curl_easy_setopt(curl, CURLOPT_URL, "http://accesspoint.emccis.com/rest/namespace/");
	curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &writefunc);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &headerfunc);
	FILE *f = fopen("target.txt", "wb");
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, f);

	char request[1024*1024];//hehe


	char hash_string[1024];
	int hash_length = build_hash_string(hash_string, method, "application/octet-stream",NULL,NULL,uri, headers,header_count);
	char sig[1024];
	char signature[1024];

	struct curl_slist *chunk = NULL;
	int i;
	for(i=0;i<header_count; i++) {
	    chunk = curl_slist_append(chunk, headers[i]);	
	}
	
	char raw_date_header[1024];
	sprintf(raw_date_header, "date:%s",date);
	snprintf(signature,1024,"X-Emc-Signature:%s",sign(hash_string,c->secret));
	curl_slist_append(chunk, "content-type:application/octet-stream");
	curl_slist_append(chunk,signature);
	result_code = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
	result_code = curl_easy_perform(curl);

	if (result_code){ 
	    printf("code:%d\nerror:%s\n", result_code, errorbuffer);
	}
	curl_easy_cleanup(curl);
    }
}
