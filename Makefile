
CC=g++
CFLAGS=-g
CXXFLAGS=-g

OBJS=\
	main.o\

DEPS=\
	fontstash.h\
	glstash.h\

CXXFLAGS+=$(shell pkg-config --cflags glfw3)
LIBS=-framework Cocoa -framework OpenGL -framework IOKit -framework CoreFoundation $(shell pkg-config --libs glfw3)

test: $(OBJS) $(DEPS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LIBS)

clean:
	rm -rf test $(OBJS)
