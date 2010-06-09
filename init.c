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


void create_ns(credentials *c, char * uri, char *content_type ,acl *acl,user_meta *meta, void *ws_result)
{
    
    http_method method = POST;
    char *headers[20];
    http_request_ns(c, method,uri,content_type, headers,0, NULL, ws_result);
    
}

void  update_ns (credentials *c, char * uri, char *content_type, acl *acllist, postdata *data,user_meta *metadata, void *ws_result) 
{
    char *headers[20];  
    http_method method = PUT;    
    
    http_request_ns(c, method, uri, content_type,headers, 0, (void*)data, ws_result);

}

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
}

void lowercase(char *s) {
    int i = 0;
    for( ; s[i]!=':'; i++)
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

    //all lowercase BEFORE entering sort..
    
    int is;
    for(is = 0; is < header_count; is++) {
	lowercase(emc_sorted_headers[is]);
    }
	
    qsort(emc_sorted_headers, header_count, sizeof(char*),cstring_cmp);
    int ir = 0;
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
    //printf("length %d", length);
    //printf("%s\n", hash_string);
    return length;

}
//needs to be free*d
char *sign (char *hash_string, const char *key)
{
    //printf("string to sign :\n%s\n", hash_string);
    return HMACSHA1((const unsigned char*)hash_string,(void*)key,strlen(key));
}

//x-emc-meta: atime=2010-06-09T12:46:18Z, mtime=2010-06-09T12:46:17Z, ctime=2010-06-09T12:46:17Z, itime=2010-06-09T12:46:17Z,
// type=regular, uid=EMC007A49DEEA84C837E, gid=apache, objectid=4980cdb2a510105804bfc45d19680d04c0f8d1a1b587, objname=capi_5th, size=0, nlink=1, policyname=default

void get_system_meta(ws_result *ws, system_meta *meta) {
    
    
}



//metaData operations
//FIXME:bound counts and sizes

int user_meta_ns(credentials *c, const char *uri, char * content_type, user_meta *meta, void * ws_result) 
{
    if(meta) {
	static const char* user_meta_uri = "?metadata/user";
	char *meta_uri = (char*)malloc(strlen(uri)+strlen(user_meta_uri));
	sprintf(meta_uri, "%s%s", uri, user_meta_uri);
	http_method method =POST;
	char *headers[20];    
	int header_count =0;
	
	char emc_meta[8192]; //FIXME is 8k the header limit?
	int emc_meta_loc = 0;
	char emc_listable[8192]; //FIXME is 8k the header limit?
	int emc_listable_loc = 0;
	
	

	user_meta * index = meta;
	int meta_count, meta_listable_count = 0;
	for( ; index !=NULL;  index= index->next) {
	    if(index->listable == false) {
		if(meta_count > 1) {
		    emc_meta_loc+=sprintf(emc_meta+emc_meta_loc, ",%s=%s", index->key, index->value);
		}
		else {
		    headers[header_count++] = emc_meta;
		    emc_meta_loc += sprintf(emc_meta+emc_meta_loc, "X-Emc-Meta:%s=%s", index->key, index->value);
		    
		}
		meta_count++;
	    } else if(index->listable == true) {
		if(meta_count > 1) {
		    emc_listable_loc+=sprintf(emc_listable+emc_listable_loc, ",%s=%s", index->key, index->value);
		}
		else {
		    headers[header_count++] = emc_listable;
		    emc_listable_loc += sprintf(emc_listable+emc_listable_loc, "X-Emc-Listable-meta:%s=%s", index->key, index->value);
		}
		meta_listable_count++;
	    }
	}
	
	http_request_ns (c, method, meta_uri, NULL, headers, header_count, NULL, ws_result);
	free(meta_uri);
    }
    
}
int object_get_listable_meta(const char *object_name) 
{

}

