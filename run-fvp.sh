PATH=$PWD/prebuilts/linux-x64/clang/bin:$PWD/prebuilts/linux-x64/dtc:$PATH
set -e
cd usr-src && make && cd ..
./pack-busybox-fvp.sh
/home/os/Documents/arm-develop/fvp/Base_RevC_AEMvA_pkg/models/Linux64_GCC-9.3/FVP_Base_RevC-2xAEMvA -C pci.pci_smmuv3.mmu.SMMU_AIDR=2 -C pci.pci_smmuv3.mmu.SMMU_IDR0=0x0046123B -C pci.pci_smmuv3.mmu.SMMU_IDR1=0x00600002 -C pci.pci_smmuv3.mmu.SMMU_IDR3=0x1714 -C pci.pci_smmuv3.mmu.SMMU_IDR5=0xFFFF0472 -C pci.pci_smmuv3.mmu.SMMU_S_IDR1=0xA0000002 -C pci.pci_smmuv3.mmu.SMMU_S_IDR2=0 -C pci.pci_smmuv3.mmu.SMMU_S_IDR3=0 -C pctl.startup=0.0.0.0 -C bp.secure_memory=1 -C cluster0.NUM_CORES=4 -C cluster1.NUM_CORES=4 -C cache_state_modelled=0 -C bp.vis.disable_visualisation=true -C bp.vis.rate_limit-enable=false -C bp.pl011_uart0.untimed_fifos=1 -C bp.pl011_uart0.unbuffered_output=1 -C cluster0.cpu0.RVBAR=67174400 -C cluster0.cpu1.RVBAR=67174400 -C cluster0.cpu2.RVBAR=67174400 -C cluster0.cpu3.RVBAR=67174400 -C cluster1.cpu0.RVBAR=67174400 -C cluster1.cpu1.RVBAR=67174400 -C cluster1.cpu2.RVBAR=67174400 -C cluster1.cpu3.RVBAR=67174400 --data cluster0.cpu0=/home/os/Documents/arm-develop/hafnium/prebuilts/linux-aarch64/trusted-firmware-a/fvp/bl31_spmd.bin@67174400 -C bp.ve_sysregs.mmbSiteDefault=0 -C cluster0.has_arm_v8-5=1 -C cluster1.has_arm_v8-5=1 -C cluster0.has_branch_target_exception=1 -C cluster1.has_branch_target_exception=1 -C cluster0.memory_tagging_support_level=2 -C cluster1.memory_tagging_support_level=2 -C bp.dram_metadata.is_enabled=1 -C cluster0.gicv3.extended-interrupt-range-support=1 -C cluster1.gicv3.extended-interrupt-range-support=1 -C gic_distributor.extended-ppi-count=64 -C gic_distributor.extended-spi-count=1024 -C gic_distributor.ARE-fixed-to-one=1 --data cluster0.cpu0=./dts/ns-manifest.dtb@0x82000000 --data cluster0.cpu0=./initrd/vmlinuz@0x88000000 --data cluster0.cpu0=./initrd/initrd.img@0x90000000  --data cluster0.cpu0=out/reference/kokoro_log/debug/hypervisor_and_spmc/ffa_msg_send_direct_req.succeeds_nwd_to_sp_echo_spmc.dtb@0x0403f000 --data cluster0.cpu0=out/reference/secure_aem_v8a_fvp_clang/hafnium.bin@0x6000000 --data cluster0.cpu0=/home/os/Documents/arm-develop/hafnium/out/reference/secure_aem_v8a_fvp_vm_clang/obj/test/vmapi/ffa_secure_partitions/service_sp_second_package/service_sp_second_package.img@0x6380000 --data cluster0.cpu0=/home/os/Documents/arm-develop/hafnium/out/reference/secure_aem_v8a_fvp_vm_clang/obj/test/vmapi/ffa_secure_partitions/service_sp_first_package/service_sp_first_package.img@0x6480000