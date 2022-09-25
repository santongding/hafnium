
ifndef CROSS_COMPILE
$(error CROSS_COMPILE not found)
else
CC		:= $(CROSS_COMPILE)gcc
endif

all:main.o
	$(CC) -o $(O)/$(file) main.o $(ldflags)
	du $(O)/$(file)

main.o:main.c
	$(CC) -c main.c $(cflags)
