cd ../busybox/_install
cp ../../hafnium/driver/linux/hafnium.ko .
rm -rf ./lib
# cp -r ../../hafnium/prebuilts/linux-aarch64/musl/lib .
rm -rf ./usr_test
cp -r ../../hafnium/usr_src/out ./usr_test
find . | cpio -o -H newc | gzip > ../initrd.img
cp ../initrd.img ../../hafnium/initrd
cd ../../hafnium/initrd
find . | cpio -o > ../initrd.img; cd -
