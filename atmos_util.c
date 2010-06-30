#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "atmos_util.h"
static const char *methods[] = {"POST", "GET","PUT", "DELETE","HEAD","OPTIONS"};
int cstring_cmp(const void *a, const void *b)
{
    return strcmp(* (char * const *) a, * (char * const *) b);
}


int build_hash_string (char *hash_string, http_method method, const char *content_type, const char *range,const char *date, const char *uri, char **emc_sorted_headers, const int header_count) 
{
    char *req_ptr=hash_string;
    char *loweruri = malloc(strlen(uri)+1);
    //all lowercase BEFORE entering sort..
    
    int is = 0;
	int i = 0;	
	int length = 0;
    for(is = 0; is < header_count; is++) {
	lowercaseheader(emc_sorted_headers[is]);
    }
	
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

    strcpy(loweruri, uri);
    lowercase(loweruri);
    req_ptr+=sprintf(req_ptr,"%s\n",loweruri);
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
    length = (int)(req_ptr-hash_string);
    //printf("length %d", length);
    //printf("%s\n", hash_string);
    free(loweruri);
    return length;

}



void get_date(char *formated_time)
{
    //strftime adds a leading 0 to the day...
    time_t t = time(NULL);
    struct tm *a = gmtime(&t);

    strftime(formated_time, 256, "%a, %d %b %Y %H:%M:%S GMT", a);
    
}

void lowercaseheader(char *s) {
    int i = 0;
    for( ; s[i] != ':'; i++)
	s[i] = tolower(s[i]);
}

void lowercase(char *s) {
    int i = 0;
    for( ; s[i] != '\0'; i++)
	s[i] = tolower(s[i]);
}
