#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "stdio.h"
#include "atmos_rest.h"
#include "util.h"


void testhmac() {
    printf("testhmac\n");
    const char *teststring="POST\napplication/octet-stream\n\nThu, 05 Jun 2008 16:38:19 GMT\n/rest/objects\nx-emc-date:Thu, 05 Jun 2008 16:38:19 GMT\nx-emc-groupacl:other=NONE\nx-emc-listable-meta:part4/part7/part8=quick\nx-emc-meta:part1=buy\nx-emc-uid: 6039ac182f194e15b9261d73ce044939/user1\nx-emc-useracl:john=FULL_CONTROL,mary=WRITE";
    const char *testkey="LJLuryj6zs8ste6Y3jTGQp71xq0=";
    const char *testresult="gk5BXkLISd0x5uXw5uIE80XzhVY=";
    assert(strcmp(HMACSHA1((const unsigned char*)teststring, (char*)testkey, strlen(testkey)),testresult)==0);
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

void testjava() {

    //const char *teststring = "GET\n\n\nSun, 3 Jan 2010 20:22:56 GMT\n/rest/objects\nx-emc-uid:0e069767430c4d37997853b058eb0af8/EMC007A49DEEA84C837E";
    //const char *teststring = "GET\n\n\nSun, 3 Jan 2010 21:10:01 GMT\n/rest/namespace/\nx-emc-uid: 0e069767430c4d37997853b058eb0af8/EMC007A49DEEA84C837E";
    //const char *teststring = "GET\napplication/octet-stream\n\nSun, 3 Jan 2010 23:36:00 GMT\n/rest/namespace/\nx-emc-date:Sun, 3 Jan 2010 23:36:00 GMT\nx-emc-uid: 0e069767430c4d37997853b058eb0af8/EMC007A49DEEA84C837E";
    const char *teststring = "GET\napplication/octet-stream\n\n\n/rest/namespace/\nx-emc-date:Mon, 04 Jan 2010 00:08:49 GMT\nx-emc-uid:0e069767430c4d37997853b058eb0af8/EMC007A49DEEA84C837E";
    const char *testkey="YlVdJFb03nYtXZk0lk0KjQplVcI=";;
    const char *testresult="tMLm11NnS1zsj7laFgmsa2Y08ZA=";
    
    //assert(strcmp(HMACSHA1((const unsigned char*)teststring, (char*)testkey, strlen(testkey)),testresult)==0);
    //char *r = HMACSHA1((const unsigned char*)teststring, (char*)testkey, strlen(testkey));
    printf("*** testing signing **\n");
    char *key = sign((char*)teststring, (char*)testkey);
    //printf("%s\n",teststring);
    printf(":%s=%s\n", key, testresult);
}

int api_testing(){

    const char *user_id = "0e069767430c4d37997853b058eb0af8/EMC007A49DEEA84C837E";
    const char *key = "YlVdJFb03nYtXZk0lk0KjQplVcI=";
    const char *endpoint = "accesspoint.emccis.com";

    credentials *c = init_ws(user_id, key, endpoint);
    ws_result result;
    char *testdir = "/capi_5th";

    //*** Create
    result_init(&result);
    create_ns(c, testdir, NULL,NULL,  NULL, &result);
    printf("code: %d\n", result.return_code);
    result_deinit(&result);

    //*** LIST
    result_init(&result);
    list_ns(c, testdir,&result);    
    printf("datum%d: %s\n", result.body_size,(char*)result.response_body);
    printf("code: %d\n", result.return_code);
    printf("headers : %s\n", result.headers);
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
    char data[] = "This is testing data for update:";
    postdata d;
    d.data=data;
    d.body_size = strlen(data);
    update_ns(c, testdir,NULL, NULL, &d, NULL,&result);    
    printf("code: %d\n", result.return_code);
    result_deinit(&result);
    //*** LIST
    result_init(&result);
    list_ns(c, testdir,&result);    
    printf("datum%d: %s\n", result.body_size,(char*)result.response_body);
    printf("code: %d\n", result.return_code);

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
}

void set_meta_data() {
    const char *user_id = "0e069767430c4d37997853b058eb0af8/EMC007A49DEEA84C837E";
    const char *key = "YlVdJFb03nYtXZk0lk0KjQplVcI=";
    const char *endpoint = "accesspoint.emccis.com";

    credentials *c = init_ws(user_id, key, endpoint);
    ws_result result;
    char *testdir = "/capi_5th";

    //*** Create
    result_init(&result);
    create_ns(c, testdir, NULL,NULL,  NULL, &result);
    printf("code: %d\n", result.return_code);
    result_deinit(&result);
    
    //** update_meta
    user_meta meta,meta1;
    bzero(&meta, sizeof(user_meta));
    bzero(&meta1, sizeof(user_meta));
    strcpy(meta.key, "meta_test");
    strcpy(meta.value, "meta_pass");
    meta.listable=true;
    strcpy(meta1.key, "1_test");
    strcpy(meta1.value, "1_pass");
    result_init(&result);
    meta.next=&meta1;
    user_meta_ns(c, testdir, NULL, &meta, &result);
}


int main() { 
    
    //testbuildhashstring();
    //testhmac();
    api_testing();
    set_meta_data();
}
