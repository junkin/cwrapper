LIBDIR = -L/usr/local/lib
LIBS= -lssl -lcrypto -lcurl
SRC= atmos_rest.c crypto.c transport.c atmos_util.c
OBJ= atmos_rest.o crypto.o transport.o atmos_util.o
FLAGS = -Wall -Wextra -fPIC -g -c

TESTSRC = test.c
TESTLIBS = -latmos
VERSION = 1
LIBNAME = libatmos.so
SOFLAGS = -shared -Wl,-soname,$(LIBNAME) -o $(LIBNAME)

all: objects lib test

objects: $(SRC)
	gcc $(FLAGS) $(SRC)
lib: $(OBJ)
	gcc $(SOFLAGS) $(OBJ) 
test: $(TESTSRC)
	gcc -pg -g -o atmostest ${OBJ} $(TESTSRC) $(LIBS) $(LIBDIR)

clean:
	rm *.o
	rm *.so
