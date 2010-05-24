LIBS= -lssl -lcrypto -lcurl
SRC= init.c test.c util.c transport.c

atmostest: $(SRC)
	gcc -g $(SRC) $(LIBS)
