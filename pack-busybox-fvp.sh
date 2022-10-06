set -e
make
cd ./driver/linux
make
cd ../usr-driver
make
cd ../..
cp ~/Documents/arm-develop/hafnium/out/reference/aem_v8a_fvp_vm_clang/obj/usr-vms/hello/hello.bin ./initrd/hello
dtc -I dts -O dtb -o ./initrd/manifest.dtb ./dts/manifest.dts
# rm ./initrd/manifest.dtb
dtc -I dts -O dtb -o ./dts/spmc.dtb ./dts/spmc.dts
dtc -I dts -O dtb -o ./dts/ns-manifest.dtb ./dts/ns-manifest.dts
cd ../busybox/_install
cp ../../hafnium/driver/linux/hafnium.ko .
cp ../../hafnium/driver/usr-driver/hf-usr-driver.ko .
# cp -r ../../hafnium/prebuilts/linux-aarch64/musl/lib .
rm -rf ./usr-test && rm -rf ./usr_test
cp -r ../../hafnium/usr-src/out ./usr-test
cp ../../hafnium/out/reference/aarch64_linux_clang/ffa-client ./usr-test/ffa-client
find . | cpio -o -H newc | gzip > ../initrd.img
cp ../initrd.img ../../hafnium/initrd
cd ../../hafnium/initrd
find . | cpio -o > ../initrd.img; 
cd ../dts
./convert.sh
