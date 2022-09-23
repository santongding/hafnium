set -e
cd usr-src && make && cd ..
bash pack-busybox.sh
qemu-system-aarch64 -M virt,gic_version=3 -cpu cortex-a57 -nographic -machine virtualization=true -kernel out/reference/qemu_aarch64_clang/hafnium.bin -initrd initrd.img -append "rdinit=/sbin/init" -smp 1,cores=1,threads=1,sockets=1 $1
