#include "util.h"
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <string.h>




char *base64decode(char *base64encoded, int length)
{
    BIO *b64, *bmem;
    char *buffer = (char *)malloc(length);

    memset(buffer, 0, length);
    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bmem = BIO_new_mem_buf(base64encoded, length);
    bmem = BIO_push(b64, bmem);
    BIO_read(bmem, buffer, length);
    BIO_free_all(bmem);

    b64= NULL;
    
    return buffer;
}

char *base64encode(char *normal, int length)
{
    BIO *bmem, *b64;
    BUF_MEM *bptr;

    b64 = BIO_new(BIO_f_base64());
    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, normal, length);
    BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bptr);

    char *buff = (char *)malloc(bptr->length);
    memcpy(buff, bptr->data, bptr->length-1);
    buff[bptr->length-1] = 0;

    BIO_free_all(b64);
    b64= NULL;
    return buff;
}
char *HMACSHA1(const unsigned char *hash_string, void *key, int key_len) {
const EVP_MD *evp_md = EVP_sha1();    
    unsigned int md_len;
    unsigned char md[EVP_MAX_MD_SIZE];
    char *newkey=base64decode((char*)key,key_len);
    HMAC(evp_md, newkey, key_len, hash_string, strlen((const char*)hash_string), md, &md_len);
    free(newkey);
    return base64encode((char*)md, md_len);

}

//Needs to be free*d
char *sign (char *hash_string, const char *key)
{
    //printf("string to sign :\n%s\n", hash_string);
    
    return HMACSHA1((const unsigned char*)hash_string,(void*)key,strlen(key));
}


