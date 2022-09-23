O := $(abspath $(./out))
CROSS_COMPILE := aarch64-linux-gnu-

subdirs := $(wildcard apps/*)
$(info $(subdirs))

all: $(subdirs)
$(subdirs):
	echo $(lastword $@)
	$(MAKE) -C $@ file=$(lastword $(subst /, ,$@)) CROSS_COMPILE=$(CROSS_COMPILE)

.PHONY: all $(subdirs)