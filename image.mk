
GLIBC_LIBS_LIB = \
	ld*.so.* libanl.so.* libc.so.* libcrypt.so.* libdl.so.* libgcc_s.so.* \
	libm.so.* libpthread.so.* libresolv.so.* librt.so.* \
	libutil.so.* libnss_files.so.* libnss_dns.so.* libmvec.so.*
INSTALL_PATH ?= ../busybox/_install
TMP_INSTALL_PATH ?= /tmp/_glibc_install
GLIBC_BUILD_DIR ?= ~/Documents/arm-develop/glibc-2.33/build
NEED_COPY_FILES = ../optee/optee_client/out/export/usr/sbin/tee-supplicant ./driver/usr-driver/hf-usr-driver.ko
TEEC_LIBS = /home/os/Documents/arm-develop/optee/optee_client/out/export/usr/lib


all:install_glibc install_teec_lib copy_files
build_glibc:
	$(MAKE) -C ${GLIBC_BUILD_DIR} -j 8
	$(MAKE) -C ${GLIBC_BUILD_DIR} install install_root=${TMP_INSTALL_PATH}
install_glibc:
	rm -rf $(INSTALL_PATH)/lib
	mkdir $(INSTALL_PATH)/lib
	cp $(foreach file,$(GLIBC_LIBS_LIB),$(wildcard $(TMP_INSTALL_PATH)/lib/${file})) $(INSTALL_PATH)/lib
	cp /tmp/_glibc_install/sbin/ldconfig $(INSTALL_PATH)/sbin

install_teec_lib:
	cp -r $(TEEC_LIBS) $(INSTALL_PATH)
copy_files: $(NEED_COPY_FILES)
	cp $(NEED_COPY_FILES) $(INSTALL_PATH)
clean:
	rm -rf $(TMP_INSTALL_PATH)