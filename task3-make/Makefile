CC 	= gcc
CFLAGS	=
LDFLAGS	=
LIBFLAG =

PROGRAM	= guess

deps	=
objs	= $(PROGRAM).o game.o

ifeq ($(LIBFLAG),static)
	objs = $(PROGRAM).o libgame.a
endif

ifeq ($(LIBFLAG),shared)
	CFLAGS += -fPIC
	LDFLAGS += -lgame -L ./
	objs = $(PROGRAM).c
	deps = game.o libgame.so
endif

.PHONY: all clean


all: $(PROGRAM)

$(PROGRAM): $(objs) $(deps)
	$(CC) $(objs) $(LDFLAGS) -o $(PROGRAM)

%.o: %.c
	$(CC) $(CFLAGS) -c $^

lib%.a: %.o
	ar rcs $@ $^

lib%.so: %.o
	$(CC) -shared -Wl,-soname,$@.1 -o $@.1.0.1 $< -lc
	ln -s $@.1.0.1 $@.1
	ln -s $@.1 $@

clean:
	rm -f $(PROGRAM) *.o *.a *.so*