dtc -I dts -O dtb --out-version 17 -o ./initrd/manifest.dtb manifest.dts
cd ../busybox/_install
cp ../../hafnium/driver/linux/hafnium.ko .
cp ../../hafnium/out/reference/qemu_aarch64_vm_clang/obj/test/linux/linux_test_initrd/initrd/test_binary .
rm -rf ./lib
# cp -r ../../hafnium/prebuilts/linux-aarch64/musl/lib .
rm -rf ./usr-test && rm -rf ./usr_test
cp -r ../../hafnium/usr-src/out ./usr-test
find . | cpio -o -H newc | gzip > ../initrd.img
cp ../initrd.img ../../hafnium/initrd
cd ../../hafnium/initrd
find . | cpio -o > ../initrd.img; cd -
