
CC=gcc
CFLAGS=-g

OBJS=\
	main.o\

DEPS=\
	fontstash.h\
	glstash.h\

LIBS=-lglfw3 -framework Cocoa -framework OpenGL -framework IOKit -framework CoreFoundation

test: $(OBJS) $(DEPS)
	$(CC) $(CXXFLAGS) -o $@ $(OBJS) $(LIBS)

clean:
	rm -rf test $(OBJS)
