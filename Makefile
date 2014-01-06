CC=gcc
#CFLAGS=-W -Wall -ansi -pedantic -g
CFLAGS=-W -Wall -g -fPIC
LDFLAGS=
EXEC=oui

# SHARED OBJECT FILE
liboui: oui.o
	$(CC) $(CFLAGS) -shared -o lib$(EXEC).so $(EXEC).o

oui: oui.o
	$(CC) -o $(EXEC) $(EXEC).o $(LDFLAGS)

# LIBRARY OBJECT FILE
oui.o: oui.c oui.h
	$(CC) -o $(EXEC).o -c $(EXEC).c $(CFLAGS)

test: test_oui.c liboui
	$(CC) -o test_oui test_oui.c -g -L. -l$(EXEC)
	LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:. ./test_oui

clean:
	rm -f $(EXEC) $(EXEC).o lib$(EXEC).so test_$(EXEC)
