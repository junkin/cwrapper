#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "atmos_util.h"
#include "atmos_rest.h"





credentials* init_ws(const char *user_id, const char *key, const char *endpoint) 
{

    credentials *c = malloc(sizeof(credentials));
    strcpy(c->tokenid, user_id);
    strcpy(c->secret, key);
    strcpy(c->accesspoint, endpoint);
    return c;
}


void create_ns(credentials *c, char * uri, char *content_type ,acl *acl,user_meta *meta, ws_result *ws)
{
    
    http_method method = POST;
    char *headers[20];
    if(acl) {
	//add acl headers
    }
    if(meta) {
	//add meta headers on create
    }
    http_request_ns(c, method,uri,content_type, headers,0, NULL, ws);
    
}

void  update_ns (credentials *c, char * uri, char *content_type, acl *acl, postdata *data,user_meta *meta, ws_result *ws) 
{
    char *headers[20];  
    http_method method = PUT;    
    if(acl) {
	//add acl headers
    }
    if(meta) {
	//add meta headers on create
    }
    
    http_request_ns(c, method, uri, content_type,headers, 0, (void*)data, ws);

}

void list_ns(credentials *c,char * uri, ws_result *ws) 
{
    http_method method =GET;
    char *headers[20];    
    
    http_request_ns (c, method, uri, NULL, headers, 0, NULL, ws);
    
}


int delete_ns(credentials *c, char *uri, ws_result *ws) 
{
    http_method method = DELETE;
    char * headers[20];
    http_request_ns (c, method, uri,NULL, headers, 0, NULL, ws);
    return ws->return_code;
}

/*
  HTTP/1.1 200 OK

  Date: Mon, 21 Jun 2010 14:39:44 GMT

  Server: Apache

  x-emc-policy: default

  Content-Length: 0

  x-emc-groupacl: other=NONE

  x-emc-useracl: EMC007A49DEEA84C837E=FULL_CONTROL

  x-emc-meta: atime=2010-06-21T14:37:37Z, mtime=2010-06-21T14:37:37Z, ctime=2010-06-21T14:37:37Z, itime=2010-06-21T14:37:37Z, type=regular, uid=EMC007A49DEEA84C837E, gid=apache, objectid=4980cdb2a310105404bc7d8ca4d51e04c1f7931a51ae, objname=capi_5th, size=0, nlink=1, policyname=default

  X-Cnection: close

  Content-Type: application/octet-stream

*/



void parse_headers(ws_result* ws, system_meta* sm, user_meta** head_ptr_um) {
    int i = 0;
    user_meta *ptr_um=NULL;
    for(; i < ws->header_count; i++) {
	if(0==strncmp(ws->headers[i], EMC_META_HDR_STR, strlen(EMC_META_HDR_STR))) {
	    char *meta_ptr = ws->headers[i] + strlen(EMC_META_HDR_STR);
	    char delims[] = ",";
	    char *result = NULL;
	    result = strtok( meta_ptr, delims);
	    while (result != NULL) {
		
		//trim leading whitespace
		int offset = 0;
		for (;result[offset] == ' '; offset++) ;
		result+=offset;
		    
		if(0 == strncmp(result,atime, strlen(atime)) ) {	
		    strcpy(sm->atime, result+strlen(atime)+1);
		} else if (0 == strncmp(result, mtime, strlen(mtime))) {
		    strcpy(sm->mtime, result+strlen(mtime)+1);
		} else if (0 == strncmp(result, emc_ctime, strlen(emc_ctime))) {
		    strcpy(sm->ctime, result+strlen(emc_ctime)+1);
		}else if (0 == strncmp(result, itime, strlen(itime))) {
		    strcpy(sm->itime, result+strlen(itime)+1);
		}else if (0 == strncmp(result, type, strlen(type))) {
		    strcpy(sm->type, result+strlen(type)+1);
		}else if (0 == strncmp(result, uid, strlen(uid))) {
		    strcpy(sm->uid, result+strlen(uid)+1);
		}else if (0 == strncmp(result, gid, strlen(gid))) {
		    strcpy(sm->gid, result+strlen(gid)+1);
		}else if (0 == strncmp(result, objectid, strlen(objectid))) {
		    strcpy(sm->objectid, result+strlen(objectid)+1);
		}else if (0 == strncmp(result, objname, strlen(objname))) {
		    strcpy(sm->objname, result+strlen(objname)+1);
		}else if (0 == strncmp(result, size, strlen(size))) {
		    sm->size = atoi(result+strlen(size+1));
		}else if (0 == strncmp(result, nlink,strlen(nlink))){
		    sm->nlink = atoi(result+strlen(nlink+1));
		}else if (0 == strncmp(result, policyname, strlen(policyname))) {
		    strcpy(sm->policyname, result+strlen(policyname)+1);
		} else {

		    if(ptr_um) {
			ptr_um->next = malloc(sizeof(user_meta));
			ptr_um = ptr_um->next;
		    } else {
			*head_ptr_um = malloc(sizeof(user_meta));
			ptr_um = *head_ptr_um;
		    }
		    bzero(ptr_um, sizeof(user_meta));
		    ptr_um->listable = false;	
		    char *key_value[2];
		    int kv_count = 0;
		    split(result, '=', key_value, &kv_count);
		    if(kv_count ==2) {	
			strcpy(ptr_um->key,key_value[0]);
			strcpy(ptr_um->value,key_value[1]);

		    } else {
			printf("meta data parse error!\n");
		    }
		    int k;
		    for(k=0; k<kv_count; k++) {
			free(key_value[k]);
		    }
		    
		}
		result = strtok(NULL, delims);
	    }
	} else if(0==strncmp(ws->headers[i], EMC_USER_HDR_STR, strlen(EMC_USER_HDR_STR))) {
	    printf("USERACL: %s\n", ws->headers[i]);
	    
	} else if(0 == strncmp(ws->headers[i], EMC_LISTABLE_META_HDR_STR, strlen(EMC_LISTABLE_META_HDR_STR))) {
	    //listable x-emc-listable-meta: meta_test=meta_pass
	    char *pairs[1024];
	    int count = 0;
	    split(ws->headers[i]+strlen(EMC_LISTABLE_META_HDR_STR), ',', pairs, &count);
	    int index = 0;


	    for(;index < count; index++) {
		if(ptr_um) {
		    ptr_um->next = malloc(sizeof(user_meta));
		    ptr_um = ptr_um->next;
		} else {
		    *head_ptr_um = malloc(sizeof(user_meta));
		    ptr_um = *head_ptr_um;
		}
		bzero(ptr_um, sizeof(user_meta));

		ptr_um->listable = true;	
		char *key_value[2];

		int kv_count = 0;
		split(pairs[index], '=', key_value, &kv_count);
		if(kv_count ==2) {	
		    strcpy(ptr_um->key,key_value[0]);
		    strcpy(ptr_um->value,key_value[1]);
		} else {
		    printf("meta data parse error!\n");
		}
		int k;
		for(k=0; k<kv_count; k++) {
		    free(key_value[k]);
		}
	    }
	    int free_pairs = 0;
	    for(free_pairs=0; free_pairs<count; free_pairs++) {
		free(pairs[free_pairs]);
	    }
	} else if(0 == strncmp(ws->headers[i], EMC_META_HDR_STR, strlen(EMC_META_HDR_STR))) {
	    ptr_um->listable=false;
      
	}
    }
}




//x-emc-meta: atime=2010-06-09T12:46:18Z, mtime=2010-06-09T12:46:17Z, ctime=2010-06-09T12:46:17Z, itime=2010-06-09T12:46:17Z,
// type=regular, uid=EMC007A49DEEA84C837E, gid=apache, objectid=4980cdb2a510105804bfc45d19680d04c0f8d1a1b587, objname=capi_5th, size=0, nlink=1, policyname=default

void get_system_meta(ws_result *ws, system_meta *meta) {
    if(ws && meta) {
	;
    }
    //char *meta_string = ws->headers;
    //printf("%s\n", meta_string);
    
}



//metaData operations
//FIXME:bound counts and sizes

int user_meta_ns(credentials *c, const char *uri, char * content_type, user_meta *meta, ws_result * ws) 
{
    if(meta) {
	static const char* user_meta_uri = "?metadata/user";
	char *meta_uri = (char*)malloc(strlen(uri)+strlen(user_meta_uri)+1);
	sprintf(meta_uri, "%s%s", uri, user_meta_uri);
	http_method method =POST;
	char *headers[20];    
	int header_count =0;
	
	char emc_meta[8192]; //FIXME is 8k the header limit?
	int emc_meta_loc = 0;
	char emc_listable[8192]; //FIXME is 8k the header limit?
	int emc_listable_loc = 0;
	
	user_meta *index = meta;
	int meta_count = 0;
	int meta_listable_count = 0;
	
	
	for( ; index !=NULL;  index=index->next) {
	    if(index->listable == false) {
		if(meta_count > 0) {
		    emc_meta_loc+=sprintf(emc_meta+emc_meta_loc, ",%s=%s", index->key, index->value);
		}
		else {
		    headers[header_count++] = emc_meta;
		    emc_meta_loc += sprintf(emc_meta+emc_meta_loc, "X-Emc-Meta:%s=%s", index->key, index->value);
		    
		}
		meta_count++;
	    } else if(index->listable == true) {
		if(meta_listable_count > 0) {
		    emc_listable_loc+=sprintf(emc_listable+emc_listable_loc, ",%s=%s", index->key, index->value);
		}
		else {
		    headers[header_count++] = emc_listable;
		    emc_listable_loc += sprintf(emc_listable+emc_listable_loc, "X-Emc-Listable-meta:%s=%s", index->key, index->value);
		}
		meta_listable_count++;
	    }
	}
	
	http_request_ns (c, method, meta_uri, content_type, headers, header_count, NULL, ws);
	free(meta_uri);
    }
    return ws->return_code;
}
//int object_get_listable_meta(const char *object_name) 
//{
  
//}

