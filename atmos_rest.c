#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "atmos_util.h"
#include "atmos_rest.h"



#define _CRT_SECURE_NO_WARNINGS 1

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
    char **headers = calloc(20,sizeof(char*));
    if(acl) {
	//add acl headers
    }
    if(meta) {
	//add meta headers on create
    }
    http_request_ns(c, method,uri,content_type, headers,0, NULL, ws);
    free(headers);    
}

void  update_ns (credentials *c, char * uri, char *content_type, acl *acl, postdata *data,user_meta *meta, ws_result *ws) 
{

    char **headers = calloc(20,sizeof(char*));
    http_method method = PUT;    
    if(acl) {
	//add acl headers
    }
    if(meta) {
	//add meta headers on create
    }
    
    http_request_ns(c, method, uri, content_type,headers, 0, data, ws);
    free(headers);
}

//need offset, size and x-emc-limit..
void list_ns(credentials *c,char * uri, postdata *pd, int limit, ws_result *ws) 
{
    http_method method =GET;
    char **headers = calloc(20,sizeof(char*));
    int count = 0;
    if (limit) {
      sprintf(headers[count++], "x-emc-limit: %d", limit);
    }
    http_request_ns (c, method, uri, NULL, headers, count, pd, ws);
    free(headers);    
}


int delete_ns(credentials *c, char *uri, ws_result *ws) 
{
    http_method method = aDELETE;
    char **headers = calloc(20,sizeof(char*));
    http_request_ns (c, method, uri,NULL, headers, 0, NULL, ws);
    free(headers);
    return ws->return_code;
}

void parse_headers(ws_result* ws, system_meta* sm, user_meta** head_ptr_um) {
    int i = 0;
    user_meta *ptr_um=NULL;
    for(; i < ws->header_count; i++) {
	if(0==strncmp(ws->headers[i], EMC_META_HDR_STR, strlen(EMC_META_HDR_STR))) {
	    char *meta_ptr = ws->headers[i] + strlen(EMC_META_HDR_STR);
	    char delims[] = ",";
	    char *result = NULL;
	    char *hdr_context;
	    result = strtok_r( meta_ptr, delims, &hdr_context);
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
			char um_delims[] = "=";
			char *meta_context;
		    char *um_result = NULL;
			 int meta_index = 0;
		    if(ptr_um) {
			ptr_um->next = malloc(sizeof(user_meta));
			ptr_um = ptr_um->next;
		    } else {
				*head_ptr_um = malloc(sizeof(user_meta));
			ptr_um = *head_ptr_um;
		    }
		    memset(ptr_um, 0,sizeof(user_meta));
		    ptr_um->listable = false;	  
		    um_result = strtok_r(result, um_delims, &meta_context);
		    while (um_result != NULL) {
		      if(0==meta_index) {
			strcpy(ptr_um->key,um_result);		  
		      } else {
			strcpy(ptr_um->value,um_result);
		      }
		      meta_index++;
		      um_result = strtok_r(NULL, um_delims, &meta_context);
		    }
		}
		result = strtok_r(NULL, delims, &hdr_context);
	    }
	} else if(0==strncmp(ws->headers[i], EMC_USER_HDR_STR, strlen(EMC_USER_HDR_STR))) {
	  ;
	    
	} else if(0 == strncmp(ws->headers[i], EMC_LISTABLE_META_HDR_STR, strlen(EMC_LISTABLE_META_HDR_STR))) {
	  //listable x-emc-listable-meta: meta_test=meta_pass
	  char hdr_delims[] = ",";
	  char *hdr_result = NULL;
	  char *hdr_context = NULL;
	  hdr_result = strtok_r(ws->headers[i]+strlen(EMC_LISTABLE_META_HDR_STR), hdr_delims, &hdr_context);
	  while (hdr_result != NULL) {
	    char delims[] = "=";
	    char *result = NULL;
	    char *inner_context;
	int meta_index = 0;
		  if(ptr_um) {
	      ptr_um->next = malloc(sizeof(user_meta));
	      ptr_um = ptr_um->next;
	    } else {
	      *head_ptr_um = malloc(sizeof(user_meta));
	      ptr_um = *head_ptr_um;
	    }
	    memset(ptr_um,0, sizeof(user_meta));
	    
	    ptr_um->listable = true;	
	    result = strtok_r(hdr_result, delims, &inner_context);
	    while (result != NULL) {
	      if(0==meta_index) {
		strcpy(ptr_um->key,result);		  
	      } else {
		strcpy(ptr_um->value,result);
	      }
	      meta_index++;
	      result = strtok_r(NULL, delims, &inner_context);
	    }
	    hdr_result = strtok_r(NULL, hdr_delims, &hdr_context);
	  }
	} else if(0 == strncmp(ws->headers[i], EMC_META_HDR_STR, strlen(EMC_META_HDR_STR))) {
	  ptr_um->listable=false;
	  
	}
    }
}


int user_meta_ns(credentials *c, const char *uri, char * content_type, user_meta *meta, ws_result * ws) 
{
    if(meta) {
	static const char* user_meta_uri = "?metadata/user";
	char *meta_uri = (char*)malloc(strlen(uri)+strlen(user_meta_uri)+1);
	char **headers = calloc(20,sizeof(char*));
	int header_count =0;
	
	char emc_meta[8192]; //FIXME is 8k the header limit?
	int emc_meta_loc = 0;
	char emc_listable[8192]; //FIXME is 8k the header limit?
	int emc_listable_loc = 0;
	
	user_meta *index = meta;
	int meta_count = 0;
	int meta_listable_count = 0;
	
	sprintf(meta_uri, "%s%s", uri, user_meta_uri);
	//http_method method =POST;
	
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
	
	http_request_ns (c, POST, meta_uri, content_type, headers, header_count, NULL, ws);
	free(meta_uri);
	free(headers);
    }
    return ws->return_code;
}
//int object_get_listable_meta(const char *object_name) 
//{
  
//}

