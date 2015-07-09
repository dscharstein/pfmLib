CC = g++
CPPFLAGS = -O2 -W -Wall
LDLIBS = -lopencv_core -lopencv_highgui -lopencv_imgproc 

BIN = ImageIOpfm
all: $(BIN)

clean:
	rm -f $(BIN) *.o core*