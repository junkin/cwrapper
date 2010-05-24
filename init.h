#ifndef __AOL_INIT__
#define __AOL_INIT__

#include "transport.h"


typedef struct requestval {
    credentials c;
    char ** emc_headers;
    char * path;
    char * method;
    char * range;
    char * content_type;
    char * date;
    char * uid;
    char * signature;
} request;

typedef struct ACLval {
    char **permissions;
} acl;

//metavalues are max size 1k
typedef struct Metaval {
    char key[1024];
    char value[1024];
    int listable;
} meta;




//Namespace
const char* create_ns(credentials *c, char * path, acl *acl, meta *meta);
const char* list_ns(credentials *c, char * path);
const char* update_ns(credentials *c, char * path, acl *acl, void* data, meta *meta);
int delete_ns(credentials *c, char *object_id);
int set_meta_ns(credentials *c, const char *object_name, const char *key, const char *val);
int get_meta_ns(credentials *c,const char *object_name);

//Object


//atmos specific helpers
credentials* init_ws(const char *user_id, const char *key, const char *endpoint);
meta create_meta(char* key, char* val, int listable);


//generic ´helper functions
int build_hash_string (char *hash_string, const http_method method, const char *content_type, const char *range,const char *date, const char *path, char **emc_sorted_headers, const int header_count);
char*  sign(char *hash_string, const char *key);
void get_date(char *formated);


///////// const30
/////

#endif
