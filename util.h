#ifndef _AOL_UTIL_H_
#define _AOL_UTIL_H_

char *base64decode(char *base64encoded, int length);
char *base64encode(char *normal, int length);
char *HMACSHA1(const unsigned char *hash_string, void *key, int key_len) ;
#endif
