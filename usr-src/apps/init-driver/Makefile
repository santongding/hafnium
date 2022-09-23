O ?=../../out
CROSS_COMPILE ?= aarch64-linux-gnu-
inc_folder = ../../../prebuilts/linux-aarch64/musl/include
lib_folder = $(abspath ../../../prebuilts/linux-aarch64/musl/lib)
# $(info $(lib_folder))
# libs := $(wildcard $(lib_folder)/*.o)
libs := $(wildcard $(lib_folder)/*.a)
# libs += $(wildcard $(lib_folder)/crt*.o)

file ?= init-driver
# $(info $(libs))
# libs := $(addprefix  , $(libs))
ifndef CROSS_COMPILE
$(error CROSS_COMPILE not found)
else
CC		:= $(CROSS_COMPILE)gcc
endif

all:main.o
	$(CC) -g3 -o $(O)/$(file) main.o -I ${inc_folder} ${libs} -lgcc --static
	du $(O)/$(file)
asm:main.o
	$(CC) -S -o $(O)/main.s main.o -I ${inc_folder} -Wno-undef --static -nostdinc -nostdlib $(libs)

main.o:main.c
	$(CC) -g3 -c main.c
