#!/bin/bash

# Path to the input DTB
KERNEL_DTB=/home/os/Documents/arm-develop/optee/linux/arch/arm64/boot/dts/arm/fvp-base-revc.dtb
# Path to the output DTB
PATCHED_KERNEL_DTB=./ns-manifest-o.dtb
PATCHED_KERNEL_DTS=./ns-manifest-o.dts
# Base address of the ramdisk
INITRD_BASE=0x90000000
# Path to the ramdisk
INITRD=../initrd/initrd.img

# Skip uboot header (64 bytes)
INITRD_START=$(printf "0x%x" $((${INITRD_BASE} + 0x0)) )
stat -Lc %s ${INITRD}
INITRD_SIZE=$(stat -Lc %s ${INITRD})
INITRD_END=$(printf "0x%x" $((${INITRD_BASE} + ${INITRD_SIZE})) )

CHOSEN_NODE=$(echo                                        \
"/ {                                                      \
        chosen {                                          \
                linux,initrd-start = <${INITRD_START}>;   \
                linux,initrd-end = <${INITRD_END}>;       \
        };                                                \
};")

echo $(dtc -O dts -I dtb ${KERNEL_DTB}) ${CHOSEN_NODE} |  \
        dtc -O dtb -o ${PATCHED_KERNEL_DTB} -
echo $(dtc -O dts -I dtb ${KERNEL_DTB}) ${CHOSEN_NODE} |  \
        dtc -O dts -o ${PATCHED_KERNEL_DTS} -