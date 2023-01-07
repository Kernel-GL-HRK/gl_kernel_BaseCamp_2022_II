# Default compiler
CC=gcc
# Default executable name
PROGRAM = main.out
# All sources and headers in project directory (non recursive)
SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
# Object files for all sources
OBJS = $(addsuffix .o, $(basename $(SOURCES)))

COMPILE.c   = $(CC) -c
LINK.c = $(CC)

RM = rm -f

.PHONY: all clean objs show target

target:
	@echo Provide a target name to make

all: $(PROGRAM)

objs:$(OBJS)

%.o:%.c
	$(COMPILE.c) $< -o $@


$(PROGRAM):$(OBJS)
	$(LINK.c)   $(OBJS) -o $@
	@echo Type ./$@ to execute the program.

clean:
	$(RM) $(OBJS) $(PROGRAM)
# Target for makefile debug purpose
show:
	@echo 'PROGRAM     :' $(PROGRAM)
	@echo 'HEADERS     :' $(HEADERS)
	@echo 'SOURCES     :' $(SOURCES)
	@echo 'OBJS        :' $(OBJS)
	@echo 'COMPILE.c   :' $(COMPILE.c)
	@echo 'link.c      :' $(LINK.c)


