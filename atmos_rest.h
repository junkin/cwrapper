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

#define TIMESIZE 40
#define UIDSIZE  44
#define POLICYSIZE  44
#define GIDSIZE  44
#define OBJECTIDSIZE  44

typedef struct System_meta {
    char atime[TIMESIZE];
    char mtime[TIMESIZE];
    char ctime[TIMESIZE];
    char itime[TIMESIZE];
    char type[1024];
    char uid[UIDSIZE];
    char gid[GIDSIZE];
    char objectid[OBJECTIDSIZE];
    char objectname[1024];
    int size;
    int nlink;
    char policyname[POLICYSIZE];
} system_meta;


//metavalues are max size 1k
typedef struct Metaval {
    char key[1024];
    char value[1024];
    int listable;
    void *next;
} user_meta;

typedef struct listing {
    char name[256];
    char type[128];
} listing;


//Namespace
void create_ns(credentials *c, char * uri, char *content_type, acl *acl, user_meta *meta, void *ws_result);
void list_ns(credentials *c, char * uri, void* ws_result);
void update_ns(credentials *c, char * uri, char *content_type, acl *acl, postdata* data, user_meta *meta, void *ws_result);
int delete_ns(credentials *c, char *object_id, void *ws_result);
int set_meta_ns(credentials *c, const char *object_name, const char *key, const char *val);
int get_meta_ns(credentials *c,const char *object_name);

//namespace metadata
int user_meta_ns(credentials *c, const char *uri, char * content_type, user_meta *meta, void * ws_result);
//Object


//atmos specific helpers
credentials* init_ws(const char *user_id, const char *key, const char *endpoint);
void create_meta(char* key, char* val, int listable, user_meta*);


//generic ´helper functions
int build_hash_string (char *hash_string, const http_method method, const char *content_type, const char *range,const char *date, const char *uri, char **emc_sorted_headers, const int header_count);
char*  sign(char *hash_string, const char *key);
void get_date(char *formated);
void split(char *s1, char c1, char **ar1, int *index) ;

///////// const30
/////

#endif
