CC     ?= gcc
WARN   ?= -pedantic -Wextra -Wdisabled-optimization -Wundef -Wmissing-braces \
		  -Wformat=2 -Waggregate-return -Wunreachable-code -Wcast-align \
		  -Wcast-qual -Wpointer-arith -Wredundant-decls -Wundef \
		  -Wstrict-aliasing=2 -Wno-unused-parameter
CFLAGS ?= -std=c11 -g $(WARN)

OBJS =\
	main.o\

DEPS =\
	fontstash.h\
	glstash.h\

LIBS = -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit -framework CoreFoundation

test: $(OBJS) $(DEPS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

clean:
	rm -rf test $(OBJS)
