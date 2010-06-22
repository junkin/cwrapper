#ifndef _ATMOS_UTIL_H_
#define _ATMOS_UTIL_H_

typedef enum http_methodval   {
	POST,
	GET,
	PUT,
	DELETE,
	HEAD,
	OPTIONS
} http_method;


void get_date(char *formated);
void lowercase(char *s);
void split(char *s1, char c1, char **ar1, int *index) ;
int build_hash_string (char *hash_string, const http_method method, const char *content_type, const char *range,const char *date, const char *uri, char **emc_sorted_headers, const int header_count);


#endif
