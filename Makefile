LIBDIR = -L/usr/local/lib
LIBS= -lssl -lcrypto -lcurl
SRC= atmos_rest.c util.c transport.c
OBJ= atmos_rest.o util.o transport.o
FLAGS = -Wall -Wextra -fPIC -g -c 

TESTSRC = test.c
TESTLIBS = -latmos
VERSION = 1
LIBNAME = libatmos.so.$(VERSION)
SOFLAGS = -shared -Wl,-soname,$(LIBNAME) -o $(LIBNAME)


objects: $(SRC)
	gcc $(FLAGS) $(SRC) $(LIBS) $(LIBDIR)	
lib: $(OBJ)
	gcc $(SOFLAGS) $(OBJ) 
test: $(TESTSRC)
	gcc $(TESTSRC) $(TESTLIBS) $(LIBS) $(LIBDIR) -Wl,-rpath,/home/sjunkin/cwrapper/