#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "util.h"
#include "init.h"



static const char *hdr_listable_meta="x-emc-listable-meta:";
static const char *hdr_meta="x-emc-meta:";
static const char *methods[] = {"POST", "GET","PUT", "DELETE","HEAD","OPTIONS"};

credentials* init_ws(const char *user_id, const char *key, const char *endpoint) 
{

    credentials *c = malloc(sizeof(credentials));
    strcpy(c->tokenid, user_id);
    strcpy(c->secret, key);
    strcpy(c->accesspoint, endpoint);
    return c;
}


void create_ns(credentials *c, char * uri, char *content_type ,acl *acl, meta *meta, void *ws_result)
{
    
    http_method method = POST;
    char *headers[20];
    http_request_ns(c, method,uri,content_type, headers,0, NULL, ws_result);
    
}


void  update_ns (credentials *c, char * uri, char *content_type, acl *acllist, void *data, meta *metadata, void *ws_result) 
{
    char *headers[20];  
    http_method method = PUT;
    
    http_request_ns(c, method, uri, content_type,headers, 0, data, ws_result);

}

//what should this return ...
//
void list_ns(credentials *c,char * uri, void* ws_result) 
{
    http_method method =GET;
    char *headers[20];    
    
    http_request_ns (c, method, uri, NULL, headers, 0, NULL, ws_result);
    
}


int delete_ns(credentials *c, char *uri, void *ws_result) 
{
    http_method method = DELETE;
    char * headers[20];
    http_request_ns (c, method, uri,NULL, headers, 0, NULL, ws_result);
    
}

int cstring_cmp(const void *a, const void *b)
{
    return strcmp(* (char * const *) a, * (char * const *) b);

    /* strcmp functions works exactly as expected from                                                                                                                                                            
       comparison function */
}

//take a string to lower case
void lowercase(char *s) {
    int i;
    for(i = 0; s[i]!=':'; i++)
	s[i] = tolower(s[i]);
}

void get_date(char *formated_time)
{
    //strftime adds a leading 0 to the day...
    time_t t = time(NULL);
    struct tm *a = gmtime(&t);
    int position =strftime(formated_time, 256, "%a, %d %b %Y %H:%M:%S GMT", a);
}

int build_hash_string (char *hash_string, http_method method, const char *content_type, const char *range,const char *date, const char *uri, char **emc_sorted_headers, const int header_count) 
{
    char *req_ptr=hash_string;

    qsort(emc_sorted_headers, header_count, sizeof(char*),cstring_cmp);

    req_ptr+=sprintf(req_ptr,"%s\n",methods[method]);

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

    req_ptr+=sprintf(req_ptr,"%s\n",uri);
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
    return length;
}

char *sign (char *hash_string, const char *key)
{
    return HMACSHA1((const unsigned char*)hash_string,(void*)key,strlen(key));
}

//metaData operations
int object_set_listable_meta(const char *object_name, const char *key, const char *val) 
{

}
int object_get_listable_meta(const char *object_name) 
{

}
