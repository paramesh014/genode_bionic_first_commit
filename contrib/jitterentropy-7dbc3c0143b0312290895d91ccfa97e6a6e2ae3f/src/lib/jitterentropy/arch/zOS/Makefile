# Compile Noise Source as user space application

CC=c89
CFLAGS +=

program_NAME := jitterentropy
#program_C_SRCS := $(wildcard *.c) 
program_C_SRCS := jitterentropy-base.c jitterentropy-main-user.c
program_ASM_SRC := gettime.s               
program_SRCS := ${program_C_SRCS}          
program_C_OBJS := ${program_C_SRCS:.c=.o}          
program_ASM_OBJ := gettime.o
program_OBJS := $(program_C_OBJS) ${program_ASM_OBJ} 

program_INCLUDE_DIRS :=
program_LIBRARY_DIRS :=
program_LIBRARIES :=

CPPFLAGS += $(foreach includedir,$(program_INCLUDE_DIRS),-I$(includedir))
LDFLAGS += $(foreach librarydir,$(program_LIBRARY_DIRS),-L$(librarydir))
LDFLAGS += $(foreach library,$(program_LIBRARIES),-l$(library))

.PHONY: all clean distclean

all: $(program_NAME)

$(program_NAME): $(program_OBJS)
	$(CC) $(LDFLAGS) -Wa,LIST -o $(program_ASM_OBJ) ${program_ASM_SRC}
	$(CC) $(LDFLAGS) -o $(program_NAME) $(program_OBJS)  

clean:
	@- $(RM) $(program_NAME)
	@- $(RM) $(program_OBJS)

distclean: clean
