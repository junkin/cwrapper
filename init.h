#ifndef __AOL_INIT__
#define __AOL_INIT__

typedef struct requestval {
    char ** emc_headers;
    char * path;
    char * method;
    char * range;
    char * content_type;
    char * date;
    char * uid;
    char * signature;
}request;

typedef enum HTTP_METHODval
    {
	POST,
	GET,
	PUT,
	DELETE
    } HTTP_METHOD;

typedef struct ACLval {
    char **permissions;
} ACL;

typedef struct Listableval {
    char **key_values;
} Listable;

typedef struct Metaval {
    char **key_values;
} Meta;

	
int init(const char *user_id, const char *key, const char *endpoint);
const char* create_object(ACL *acl, Listable *listable, Meta *meta);
const char* list_objects();
int delete_object(const char *object_id);
const char *http_request(HTTP_METHOD method, char **headers, int header_count) ;
int build_hash_string (char *hash_string, const char *method, const char *content_type, const char *range,const char *date, const char *path, char **emc_sorted_headers, const int header_count);

int do_request(request);

//MetaData operations
int object_set_listable_meta(const char *object_name, const char *key, const char *val);
int object_get_listable_meta(const char *object_name);
char*  sign(char *hash_string, const char *key);
void get_date(char *formated);


///////// const30
/////

#endif
