CC     ?= gcc
WARN   ?= -pedantic -Wextra -Wdisabled-optimization -Wundef -Wmissing-braces \
		  -Wformat=2 -Waggregate-return -Wunreachable-code -Wcast-align \
		  -Wcast-qual -Wpointer-arith -Wredundant-decls -Wundef \
		  -Wstrict-aliasing=2 -Wno-unused-parameter
CFLAGS ?= -std=c11 -g $(WARN)

SCAN_BUILD ?= scan-build
SCAN_VIEW ?= scan-view

# scan-build is pretty picky about this, it wanted an actual path
ANALYZER ?= /usr/local/bin/clang

OBJS =\
	main.o\

DEPS =\
	fontstash.h\
	glstash.h\

LIBS = -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit -framework CoreFoundation

test: $(OBJS) $(DEPS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

analyze: clean
	$(eval SCAN_DIR := $(shell $(SCAN_BUILD) --use-analyzer=$(ANALYZER) make test 2>&1 | grep -Po "(?<='scan-view ).*(?=')|No bugs found"))
ifneq (,$(findstring 'found',"$(SCAN_DIR)"))
	@echo 'Potential bugs found, opening viewer'
	$(SCAN_VIEW) $(SCAN_DIR)
else
	@echo 'No bugs found'
endif

clean:
	rm -rf test $(OBJS)

.PHONY: analyze clean
