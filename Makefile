
CC=g++
CFLAGS=-g
CXXFLAGS=-g

OBJS=\
	main.o\

DEPS=\
	fontstash.h\
	glstash.h\

LIBS=-lglfw3 -framework Cocoa -framework OpenGL -framework IOKit -framework CoreFoundation

test: $(OBJS) $(DEPS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LIBS)

clean:
	rm -rf test $(OBJS)
