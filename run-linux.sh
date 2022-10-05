PATH=$PWD/prebuilts/linux-x64/clang/bin:$PWD/prebuilts/linux-x64/dtc:$PATH
set -e
qemu-system-aarch64 -M virt,gic_version=3 -cpu cortex-a57 -nographic -machine virtualization=true -kernel ./initrd/vmlinuz -initrd ./initrd/initrd.img -append "rdinit=/sbin/init" -smp 1,cores=1,threads=1,sockets=1 $1