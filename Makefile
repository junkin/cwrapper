LIBS= -lssl -lcrypto -lcurl
SRC= init.c test.c util.c

atmostest: $(SRC)
	gcc  $(SRC) $(LIBS)
