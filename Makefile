#Variables
CC= g++
CPPFLAGS= -O2 -W -c -Wall

pfmLib.a: ImageIOpfm.o
	ar rcs pfmLib.a ImageIOpfm.o

ImageIOpfm.o: ImageIOpfm.cpp 
	$(CC) $(CPPFLAGS) ImageIOpfm.cpp


clean:
	rm -f $(BIN) *.o core* *.a