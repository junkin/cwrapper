LIBDIR = -L/usr/local/lib
LIBS= -lssl -lcrypto -lcurl
SRC= atmos_rest.c test.c util.c transport.c
#FLAGS = -Wall -Wextra
atmostest: $(SRC)
	gcc -g $(FLAGS) $(SRC) $(LIBS) $(LIBDIR)
