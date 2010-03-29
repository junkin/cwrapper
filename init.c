#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#include "curl/curl.h"
#include "util.h"
#include "init.h"

#define true 1
#define false 0

static const char *user;
static const char *secret_key;
static const char *base_url;

static const char *hdr_listable_meta="x-emc-listable-meta:";
static const char *hdr_meta="x-emc-meta:";

int init(const char *user_id, const char *key, const char *endpoint) 
{
    user = user_id;
    secret_key = key;
    base_url = endpoint;
}


const char* create_object(ACL *acl, Listable *listable, Meta *meta)
{
    
    HTTP_METHOD method = POST;

    char *headers[20];
    http_request(method,headers,0);
    
}

const char* list_objects() 
//const char* list_objects(char *objectid) 
{
    HTTP_METHOD method =GET;
    char *headers[20];    
    
    http_request(method, headers,0);

}
void get_date(char *formated_time)
{
    //strftime adds a leading 0 to the day...
    time_t t = time(NULL);
    struct tm *a = gmtime(&t);
    int position =strftime(formated_time, 256, "%a, %d %b %Y %H:%M:%S GMT", a);
}


int delete_object(const char *object_id) 
{

}

const char *http_request(HTTP_METHOD method, char **headers, int header_count) 
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
    snprintf(uidheader,1024,"X-Emc-Uid:%s",user);    
    char groupaclheader[1024];
    snprintf(groupaclheader,1024,"X-Emc-groupacl:other=NONE");    
    headers[header_count++] = dateheader;
    headers[header_count++] = uidheader;
    //headers[header_count++] = groupaclheader;
    //headers[header_count++] = groupaclheader;    
    const char* path = "/rest/namespace/";
    char errorbuffer[1024*1024];
    // set up flags
    if (curl) {
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, connect_timeout);
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorbuffer);
	curl_easy_setopt(curl, CURLOPT_URL, "http://accesspoint.emccis.com/rest/namespace/");
	//curl_easy_setopt(curl, CURLOPT_URL, "192.168.1.50/rest/objects");
	curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);
	//curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
	//curl_easy_setopt(curl, CURLOPT_PORT, 8000);
	//curl_easy_setopt(curl, CURLOPT_USERAGENT, "fake ua");
	//curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
	//curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
	//the part with no emc-headers
	char request[1024*1024];//hehe
	char hash_string[1024];
	//int hash_length = build_hash_string(hash_string,"GET", "application/octet-stream",NULL,date,path, headers,header_count);
	int hash_length = build_hash_string(hash_string,"GET", "application/octet-stream",NULL,NULL,path, headers,header_count);
	//printf("last char of hash_string is :>%s<:\n",hash_string[hash_length] );

	char sig[1024];
	char signature[1024];

	struct curl_slist *chunk = NULL;
	int i;
	for(i=0;i<header_count; i++) {
	    chunk = curl_slist_append(chunk, headers[i]);	
	}
	
	char raw_date_header[1024];
	sprintf(raw_date_header, "date:%s",date);
	//curl_slist_append(chunk, raw_date_header);
	//printf("::::%s::::\n", hash_string);
	snprintf(signature,1024,"X-Emc-Signature:%s",sign(hash_string,secret_key));
	curl_slist_append(chunk, "content-type:application/octet-stream");
	//snprintf(signature,1024,"x-emc-signature:%s",s);
	curl_slist_append(chunk,signature);

	result_code = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

	
	result_code = curl_easy_perform(curl);
	
	if (result_code){ 
	    printf("code:%d\nerror:%s\n", result_code, errorbuffer);
	}
	curl_easy_cleanup(curl);
    }

}
int cstring_cmp(const void *a, const void *b)
{
    const char **ia = (const char **)a;
    const char **ib = (const char **)b;
    return strcmp(*ia, *ib);
    /* strcmp functions works exactly as expected from                                                                                                                                                            
       comparison function */
}

//take a string to lower case
void lowercase(char *s) {
    int i;
    for(i = 0; s[i]!=':'; i++)
	s[i] = tolower(s[i]);
}
int build_hash_string (char *hash_string, const char *method, const char *content_type, const char *range,const char *date, const char *path, char **emc_sorted_headers, const int header_count) 
{
    char *req_ptr=hash_string;

    qsort(emc_sorted_headers, header_count, sizeof(char*),cstring_cmp);

    req_ptr+=sprintf(req_ptr,"%s\n",method);

    if(content_type!=NULL) {
	req_ptr+=sprintf(req_ptr,"%s\n",content_type);
    } else{
	req_ptr+=sprintf(req_ptr,"\n");
    }

    if(range!=NULL) {
	req_ptr+=sprintf(req_ptr,"%s\n",range);
    } else{
	req_ptr+=sprintf(req_ptr,"\n");
    }
    
    if(date!=NULL) {
	req_ptr+=sprintf(req_ptr,"%s\n",date);
    } else{
	req_ptr+=sprintf(req_ptr,"\n");
    }

    req_ptr+=sprintf(req_ptr,"%s\n",path);
    int i;
    for(i = 0; i < header_count; i++) {
	lowercase(emc_sorted_headers[i]);
	if (i < header_count-1)
	    {
		req_ptr+=sprintf(req_ptr,"%s\n", emc_sorted_headers[i]);		
	    } 
	else 
	    {
		req_ptr+=sprintf(req_ptr,"%s", emc_sorted_headers[i]);
	    }
    }
    int length = (int)(req_ptr-hash_string);
    printf("string to sign(%d)\n%s:::::", length,hash_string);
    printf("\n");
    return length;
}

char *sign (char *hash_string, const char *key)
{

    //printf("*****SIGNED****\n%s\n%s\n", hash_string,key);

    return HMACSHA1((const unsigned char*)hash_string,(void*)key,strlen(key));
    

}

//MetaData operations
int object_set_listable_meta(const char *object_name, const char *key, const char *val) 
{

}
int object_get_listable_meta(const char *object_name) 
{

}
