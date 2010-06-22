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

    //all lowercase BEFORE entering sort..
    
    int is;
    for(is = 0; is < header_count; is++) {
	lowercase(emc_sorted_headers[is]);
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



void get_date(char *formated_time)
{
    //strftime adds a leading 0 to the day...
    time_t t = time(NULL);
    struct tm *a = gmtime(&t);

    strftime(formated_time, 256, "%a, %d %b %Y %H:%M:%S GMT", a);
    
}

void lowercase(char *s) {
    int i = 0;
    for( ; s[i]!=':'; i++)
	s[i] = tolower(s[i]);
}

//split s1 into an array delimited on c1
void split(char *s1, char c1, char **ar1, int *array_size) {
  printf("%s\n", s1);
    size_t i =0;
    size_t last = 0;
    size_t index =0;
    for(; i <= strlen(s1); i++) {
	if(s1[i] == c1) {
	    int size = i-last+1;
	    ar1[index] = malloc(size);
	    memcpy(ar1[index], s1+last, i-last);
	    ar1[size] = '\0';// null terminate our own strings..
	    index++;
	    last = i+1;
	}
    }

    //get that last piece
    int size = strlen(s1) - last+1;
    ar1[index] = malloc(size);
    bzero(ar1[index], size);
    memcpy(ar1[index], s1+last, size-1);
    ar1[size] = '\0';// null terminate our own strings..
    index++;
    last = i+1;
    
    *array_size = index;
}
