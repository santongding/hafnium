O := $(abspath ./out)
CROSS_COMPILE := aarch64-linux-gnu-
CC = $(CROSS_COMPILE)gcc
inc_folder = $(abspath ../prebuilts/linux-aarch64/musl/include)
lib_folder = $(abspath ../prebuilts/linux-aarch64/musl/lib)
# hfinc_folder = $(abspath ../inc/)
# vmapiinc_folder = $(abspath ../inc/vmapi)
# archinc_folder = $(abspath ../src/arch/aarch64/inc)
# hfsrc_folder = $(abspath ../src)
# hfsrcs = $(abspath $(wildcard $(hfsrc_folder)/*.c))
# hfobjs = $(patsubst %.c,%.o,$(hfsrcs))
libs := $(wildcard $(lib_folder)/*.a)
# libs += $(lib_folder)/crt1.o
cflags = -g3
ldflags = -g3 -I ${inc_folder} ${libs}
$(info $(libs))

subdirs := $(wildcard apps/*)

$(info $(subdirs))

all: $(subdirs)
$(subdirs):
	echo $(lastword $@)
	$(MAKE) -C $@ file=$(lastword $(subst /, ,$@)) O=$(O) CROSS_COMPILE=$(CROSS_COMPILE) ldflags='$(ldflags)' cflags='$(cflags)'

.PHONY: all $(subdirs)
