O ?=../out
CROSS_COMPILE ?= aarch64-linux-gnu-
inc_folder = ../../prebuilts/linux-aarch64/musl/include
lib_folder = $(abspath ../../prebuilts/linux-aarch64/musl/lib)
# $(info $(lib_folder))
libs := $(wildcard $(lib_folder)/*.o)
libs := $(wildcard $(lib_folder)/*.a)
# $(info $(libs))
# libs := $(addprefix  , $(libs))
ifndef CROSS_COMPILE
$(error CROSS_COMPILE not found)
else
CC		:= $(CROSS_COMPILE)gcc
endif

all:hello.o
	$(CC) -o $(O)/hello hello.o -I ${inc_folder} -nostdinc -nostdlib $(libs) -lgcc
asm:hello.o
	$(CC) -S -o $(O)/hello.s hello.o -I ${inc_folder} -Wno-undef --static -nostdinc -nostdlib $(libs)

hello.o:hello.c
	$(CC) -c hello.c
