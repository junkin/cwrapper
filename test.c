#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "stdio.h"
#include "atmos_rest.h"
#include "crypto.h"

static const char *user_id = "0e069767430c4d37997853b058eb0af8/EMC007A49DEEA84C837E";
static const char *key = "YlVdJFb03nYtXZk0lk0KjQplVcI=";
static const char *endpoint = "accesspoint.emccis.com";


//hmac validater

void testhmac() {
    printf("testhmac\n");
    const char *teststring="POST\napplication/octet-stream\n\nThu, 05 Jun 2008 16:38:19 GMT\n/rest/objects\nx-emc-date:Thu, 05 Jun 2008 16:38:19 GMT\nx-emc-groupacl:other=NONE\nx-emc-listable-meta:part4/part7/part8=quick\nx-emc-meta:part1=buy\nx-emc-uid: 6039ac182f194e15b9261d73ce044939/user1\nx-emc-useracl:john=FULL_CONTROL,mary=WRITE";
    const char *testkey="LJLuryj6zs8ste6Y3jTGQp71xq0=";
    const char *testresult="gk5BXkLISd0x5uXw5uIE80XzhVY=";
    char *freeme = HMACSHA1((const unsigned char*)teststring, (char*)testkey, strlen(testkey));
    assert(strcmp(freeme,testresult)==0);
    free(freeme);
    printf("finished testhmac\n");
}

void testbuildhashstring() {
    printf("testbuild_hash_string!\n"); 
    const char *teststring="POST\napplication/octet-stream\n\nThu, 05 Jun 2008 16:38:19 GMT\n/rest/objects\nx-emc-date:Thu, 05 Jun 2008 16:38:19 GMT\nx-emc-groupacl:other=NONE\nx-emc-listable-meta:part4/part7/part8=quick\nx-emc-meta:part1=buy\nx-emc-uid: 6039ac182f194e15b9261d73ce044939/user1\nx-emc-useracl:john=FULL_CONTROL,mary=WRITE";
    
    int header_count=0;
    char *headers[6];
    char date[256];
    char acl[256];
    char meta[256];
    char uid[256];
    char useracl[256];
    char listable[256];

    strcpy(date, "x-emc-date:Thu, 05 Jun 2008 16:38:19 GMT");
    strcpy(acl,"x-emc-groupacl:other=NONE");
    strcpy(listable, "x-emc-listable-meta:part4/part7/part8=quick");
    strcpy(meta, "x-emc-meta:part1=buy");
    strcpy(uid,"x-emc-uid: 6039ac182f194e15b9261d73ce044939/user1");
    strcpy(useracl, "x-emc-useracl:john=FULL_CONTROL,mary=WRITE");

    headers[header_count++] = date;
    headers[header_count++] = acl;
    headers[header_count++] = listable;
    headers[header_count++] = meta;
    headers[header_count++] = uid;
    headers[header_count++] = useracl;

    char string[1024*1024];
    build_hash_string(string, POST,"application/octet-stream",NULL,"Thu, 05 Jun 2008 16:38:19 GMT", "/rest/objects",headers,header_count);
    assert(strcmp(string, teststring) == 0 );
    printf("%s\n", string);
    printf("Finished building hash string\n");
}

//cycle through series of tests creating,setting meta data updateing deleting and listing objects
int api_testing(){

    credentials *c = init_ws(user_id, key, endpoint);
    ws_result result;
    char *testdir = "/capi_test";

    //*** Create
    result_init(&result);
    create_ns(c, testdir, NULL,NULL,  NULL, &result);
    printf("code: %d\n", result.return_code);
    result_deinit(&result);


    //*** LIST
    result_init(&result);
    list_ns(c, testdir,&result);    
    //result body size is not null terminated.
    char *body = malloc(result.body_size+1);
    memcpy(body, result.response_body, result.body_size);
    body[result.body_size] = '\0';
    printf("datum%d:%s\n", result.body_size,body);
    printf("code: %d\n", result.return_code);
    free(body);
    int hc = 0;
    printf("heads %d\n", result.header_count);
    for(; hc < result.header_count; hc++) {
	printf("%s\n",result.headers[hc]);
    }
    system_meta smeta;
    get_system_meta(&result, &smeta);
    
    result_deinit(&result);

    //*** Delete
    result_init(&result);
    delete_ns(c, testdir, &result);
    printf("code: %d\n", result.return_code);
    result_deinit(&result);

    //*** LIST
    result_init(&result);
    list_ns(c, testdir, &result);    
    printf("code: %d\n", result.return_code);
    result_deinit(&result);

    //*** Create
    result_init(&result);
    create_ns(c, testdir, NULL,NULL,  NULL, &result);
    printf("code: %d\n", result.return_code);
    result_deinit(&result);

    //*** UPDATE
    result_init(&result);
    //char data[] = "This is testing data for update:";
    int bd_size = 1024*64+2;// force boundary condistions in readfunction
    char big_data[bd_size];
    char *data = big_data;
    bzero(big_data, bd_size);
    
    postdata d;
    d.data=data;
    d.body_size = bd_size;
    update_ns(c, testdir,NULL, NULL, &d, NULL,&result);    
    printf("code: %d\n", result.return_code);
    result_deinit(&result);

    //*** LIST
    result_init(&result);
    list_ns(c, testdir,&result);    
    
    char *body2 = malloc(result.body_size+1);
    memcpy(body2, result.response_body, result.body_size);
    body2[result.body_size] = '\0';
    printf("datum%d:%s\n", result.body_size,body2);
    free(body2);
    //printf("datum%d: %s\n", result.body_size,(char*)result.response_body);
    printf("code: %d\n", result.return_code);
    result_deinit(&result);

    //*** Delete
    result_init(&result);
    delete_ns(c, testdir, &result);
    printf("code: %d\n", result.return_code);
    result_deinit(&result);

    //*** LIST
    result_init(&result);
    list_ns(c, testdir, &result);    
    printf("code: %d\n", result.return_code);
    result_deinit(&result);

    //get_system_meta(&result);// vs
    //result.system_meta
    
    free(c);
}

void set_meta_data() {

    credentials *c = init_ws(user_id, key, endpoint);
    ws_result result;
    char *testdir = "/capi_5th";

    //*** Create
    result_init(&result);
    //    create_ns(c, testdir, NULL,NULL,  NULL, &result);
    printf("code: %d\n", result.return_code);
    result_deinit(&result);
    
    //** update_meta
    user_meta meta,meta1, meta2, meta3;
    bzero(&meta, sizeof(user_meta));
    bzero(&meta1, sizeof(user_meta));
    bzero(&meta2, sizeof(user_meta));
    bzero(&meta3, sizeof(user_meta));
    strcpy(meta.key, "meta_test");
    strcpy(meta.value, "meta_pass");
    meta.listable=true;
    strcpy(meta1.key, "1_test");
    strcpy(meta1.value, "1_pass");
    strcpy(meta2.key, "2_test");
    strcpy(meta2.value, "2_pass");
    strcpy(meta3.key, "3_test");
    strcpy(meta3.value, "3_pass");
    
    result_init(&result);

    meta.next=&meta1;
    meta1.next=&meta2;
    meta2.next=&meta3;
    user_meta_ns(c, testdir, NULL, &meta, &result);
    result_deinit(&result);

    system_meta sm ;
    user_meta *um = NULL;

    result_init(&result);
    list_ns(c, testdir, &result);    
    parse_headers(&result, &sm, &um);    
    
    result_deinit(&result);
    printf("%s\n", sm.objname);
    while(um != NULL) {
	user_meta *t = um->next;
	printf("%s=%s %d\n", um->key, um->value, um->listable);
	free(um);
	um=t;
    }
      
      
    free(c);
}


//make sure headers are properly parsed.
void header_test() {
    
    int count =0;
    int *ct_ptr = &count;
    //split("x-emc-meta: atime=2010-06-10T02:41:56Z, asdf,,2,3,4,5", ',', &rs.headers, &count);
    //split("x-emc-meta: atime=2010-06-10T02:41:56Z", ',', &rs.headers, &count);
    char *pairs[1024];
    split("x-emc-meta: atime=2010-06-10T02:41:56Z, mtime=2010-06-10T02:41:56Z, ctime=2010-06-10T02:41:56Z, itime=2010-06-10T01:", ',', pairs, &count);
    printf("%d\n", count);
    int i = 0; 
    for( ; i < count; i++) {
	printf("%d\t%s\n", i, pairs[i]);
	free(pairs[i]);
    }

}

//create -> list -> delete
void create_test() {

    credentials *c = init_ws(user_id, key, endpoint);
    ws_result result;
    char *testdir="/capi_testing";
    //*** Create
    result_init(&result);
    create_ns(c, testdir, NULL,NULL,  NULL, &result);
    list_ns(c, testdir, &result);
    system_meta sm;
    bzero(&sm, sizeof(sm));
    user_meta *um;
    parse_headers(&result, &sm, &um);
    result_deinit(&result);

    //*** Delete
    result_init(&result);
    delete_ns(c, testdir, &result);
    result_deinit(&result);

    free(c);
}
int main() { 
    create_test();
      testbuildhashstring();
      testhmac();
      api_testing();
    set_meta_data();
    header_test();
}
