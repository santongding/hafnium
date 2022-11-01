set -e
make

cd ./driver/usr-driver
make
cd ../..
# cp ~/Documents/arm-develop/hafnium/out/reference/secure_aem_v8a_fvp_vm_clang/obj/usr-vms/hello/hello.bin ./initrd/hello
# cp ~/Documents/arm-develop/hafnium/out/reference/secure_aem_v8a_fvp_vm_clang/obj/usr-vms/dead-loop/dead-loop.bin ./initrd/dead-loop
dtc -I dts -O dtb -o ./initrd/manifest.dtb ./dts/manifest.dts
# rm ./initrd/manifest.dtb
dtc -I dts -O dtb -o ./dts/spmc.dtb ./dts/spmc.dts
dtc -I dts -O dtb -o ./dts/ns-manifest.dtb ./dts/ns-manifest.dts
dtc -I dts -O dtb -o ./dts/ns-hyper-manifest.dtb ./dts/ns-hyper-manifest.dts
make -f image.mk
cd ../busybox/_install
cp /home/os/Documents/arm-develop/optee/out-br/target/usr/bin/santongding_hello .
# cp ../../hafnium/driver/linux/hafnium.ko .
# cp ../../hafnium/driver/usr-driver/hf-usr-driver.ko .
# cp ../../optee/optee_client/out/export/usr/sbin/tee-supplicant .
# cp -r ../../optee/optee_client/out/export/usr/lib .
# cp /home/os/Documents/arm-develop/optee/out-br/target/lib/libc.so.6 ./lib
# cp /home/os/Documents/arm-develop/optee/toolchains/aarch64/aarch64-none-linux-gnu/libc/sbin/ldconfig ./sbin
# cp -r ../../optee/optee_client/tmp/optee_client/include/ ./bin/teec
# cp -r /home/os/Documents/arm-develop/optee/optee_client/out/export/usr/lib .
# cp -r ../../hafnium/prebuilts/linux-aarch64/musl/lib .
rm -rf ./usr-test && rm -rf ./usr_test
cp -r ../../hafnium/usr-src/out ./usr-test
cp ../../hafnium/out/reference/aarch64_linux_clang/ffa-client ./usr-test/ffa-client
find . | cpio -o -H newc | gzip > ../initrd.img
cp ../initrd.img ../../hafnium/initrd
cd ../../hafnium/initrd
find . | cpio -o > ../initrd.img; 
# cd ../dts
# ./convert.sh
