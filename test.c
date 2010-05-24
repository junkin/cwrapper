#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "stdio.h"
#include "init.h"
#include "util.h"


void testhmac() {

    const char *teststring="POST\napplication/octet-stream\n\nThu, 05 Jun 2008 16:38:19 GMT\n/rest/objects\nx-emc-date:Thu, 05 Jun 2008 16:38:19 GMT\nx-emc-groupacl:other=NONE\nx-emc-listable-meta:part4/part7/part8=quick\nx-emc-meta:part1=buy\nx-emc-uid: 6039ac182f194e15b9261d73ce044939/user1\nx-emc-useracl:john=FULL_CONTROL,mary=WRITE";
    const char *testkey="LJLuryj6zs8ste6Y3jTGQp71xq0=";
    const char *testresult="gk5BXkLISd0x5uXw5uIE80XzhVY=";
    assert(strcmp(HMACSHA1((const unsigned char*)teststring, (char*)testkey, strlen(testkey)),testresult)==0);
}

void testbuildhashstring() {
    
    const char *teststring="POST\napplication/octet-stream\n\nThu, 05 Jun 2008 16:38:19 GMT\n/rest/objects\nx-emc-date:Thu, 05 Jun 2008 16:38:19 GMT\nx-emc-groupacl:other=NONE\nx-emc-listable-meta:part4/part7/part8=quick\nx-emc-meta:part1=buy\nx-emc-uid: 6039ac182f194e15b9261d73ce044939/user1\nx-emc-useracl:john=FULL_CONTROL,mary=WRITE";
    
    int header_count=6;
    char *headers[6];
    headers[4] = "x-emc-date:Thu, 05 Jun 2008 16:38:19 GMT";
    headers[3] = "x-emc-groupacl:other=NONE";
    headers[2] = "x-emc-listable-meta:part4/part7/part8=quick";
    headers[1] = "x-emc-meta:part1=buy";
    headers[0] = "x-emc-uid: 6039ac182f194e15b9261d73ce044939/user1";
    headers[5] = "x-emc-useracl:john=FULL_CONTROL,mary=WRITE";

    char string[1024*1024];
    build_hash_string(string, POST,"application/octet-stream",NULL,"Thu, 05 Jun 2008 16:38:19 GMT", "/rest/objects",(char **)headers,6);
    assert(strcmp(string, teststring) == 0 );
   
    
}

void testjava() {

    
    //   const char *teststring = "GET\n\n\nSun, 3 Jan 2010 20:26:59 GMT\n/rest/namespace/\nx-emc-uid:0e069767430c4d37997853b058eb0af8/EMC007A49DEEA84C837E";
    //const char *teststring = "GET\n\n\nSun, 3 Jan 2010 20:22:56 GMT\n/rest/objects\nx-emc-uid:0e069767430c4d37997853b058eb0af8/EMC007A49DEEA84C837E";
    //const char *teststring = "GET\n\n\nSun, 3 Jan 2010 21:10:01 GMT\n/rest/namespace/\nx-emc-uid: 0e069767430c4d37997853b058eb0af8/EMC007A49DEEA84C837E";
    //const char *teststring = "GET\napplication/octet-stream\n\nSun, 3 Jan 2010 23:36:00 GMT\n/rest/namespace/\nx-emc-date:Sun, 3 Jan 2010 23:36:00 GMT\nx-emc-uid: 0e069767430c4d37997853b058eb0af8/EMC007A49DEEA84C837E";
    const char *teststring = "GET\napplication/octet-stream\n\n\n/rest/namespace/\nx-emc-date:Mon, 04 Jan 2010 00:08:49 GMT\nx-emc-uid:0e069767430c4d37997853b058eb0af8/EMC007A49DEEA84C837E";
    const char *testkey="YlVdJFb03nYtXZk0lk0KjQplVcI=";;
    const char *testresult="tMLm11NnS1zsj7laFgmsa2Y08ZA=";
    
    //assert(strcmp(HMACSHA1((const unsigned char*)teststring, (char*)testkey, strlen(testkey)),testresult)==0);
    //char *r = HMACSHA1((const unsigned char*)teststring, (char*)testkey, strlen(testkey));
    char *key = sign((char*)teststring, (char*)testkey);
    
    printf("%s\n",teststring);
    printf(":%s:\n", key);
}

void testjshashstring() {
    const char *user_id = "0e069767430c4d37997853b058eb0af8/EMC007A49DEEA84C837E";
    const char *testkey = "YlVdJFb03nYtXZk0lk0KjQplVcI=";
    
    //    char *key = sign((char*)teststring, (char*)testkey);    const char *teststring="POST\n\n\nThu, 15 Apr 2010 12:53:43 GMT\n/rest/objects/js/jsfile1\nx-emc-uid:Thu, 15 Apr 2010 12:53:43 GMT\nx-emc-uid:0e069767430c4d37997853b058eb0af8/EMC007A49DEEA84C837E";

   const char *teststring="x-emc-uid:0e069767430c4d37997853b058eb0af8/EMC007A49DEEA84C837E";
    char *key = sign((char*)teststring, (char*)testkey);
    printf("%s<><>\n%s\n",teststring, key);
}


int more();
int main() { 
    
    //    testbuildhashstring();
    //testhmac();
    //testjava();
    more();
    //   testjshashstring();
}


int more(){

    const char *user_id = "0e069767430c4d37997853b058eb0af8/EMC007A49DEEA84C837E";
    const char *key = "YlVdJFb03nYtXZk0lk0KjQplVcI=";

    const char *endpoint = "accesspoint.emccis.com";
    char *buf = base64encode((char*)user_id, strlen(user_id));
    char *buf2 = base64decode(buf,strlen(buf));
    
    //const char *object_id = create_object(NULL, NULL,NULL);
    credentials *c = init_ws(user_id, key, endpoint);
    // const char *object_id = list_ns(c, "/");
    // printf("created object with objectid %s\n", object_id);
    
    const char *testdir = "/capi_dior";
    printf ("creating dir %s\n", testdir);
    create_ns(c, testdir, NULL,  NULL);
    list_ns(c, testdir);    
    //delete_ns(c, testdir);
    //list_ns(c, testdir);    

}
