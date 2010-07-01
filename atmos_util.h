#ifndef _ATMOS_UTIL_H_
#define _ATMOS_UTIL_H_

typedef enum  {
	POST,
	GET,
	PUT,
	aDELETE,
	HEAD,
	OPTIONS
} http_method;


void get_date(char *formated);
void lowercase(char *s);
void lowercaseheader(char *s);
int build_hash_string (char *hash_string, const http_method method, const char *content_type, const char *range,const char *date, const char *uri, char **emc_sorted_headers, const int header_count);

#if WIN32
#define snprintf sprintf_s
#define strtok_r strtok_s
#endif
#endif
