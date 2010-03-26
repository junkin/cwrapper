LIBS= -lssl -lcrypto -lcurl
SRC= init.c test.c

atmostest: $(SRC)
	gcc  $(SRC) $(LIBS)
