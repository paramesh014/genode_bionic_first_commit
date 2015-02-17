# Compile Noise Source as user space application

CC=gcc
# The linker flags are to enable RELRO
CFLAGS +=-pedantic -Wall -fPIC -Wl,-z,relro,-z,now

NAME := jitterentropy
#C_SRCS := $(wildcard *.c) 
C_SRCS := jitterentropy-base.c
C_OBJS := ${C_SRCS:.c=.o}
OBJS := $(C_OBJS)

INCLUDE_DIRS :=
LIBRARY_DIRS :=
LIBRARIES := rt

CFLAGS += $(foreach includedir,$(INCLUDE_DIRS),-I$(includedir))
LDFLAGS += $(foreach librarydir,$(LIBRARY_DIRS),-L$(librarydir))
LDFLAGS += $(foreach library,$(LIBRARIES),-l$(library))

.PHONY: all clean distclean

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) -shared -Wl,-soname,lib$(NAME).so.1 -o lib$(NAME).so.1.0 $(OBJS) $(LDFLAGS)

clean:
	@- $(RM) $(NAME)
	@- $(RM) $(OBJS)
	@- $(RM) lib$(NAME).so*

distclean: clean
