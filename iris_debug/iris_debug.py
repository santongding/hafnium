import sys, os
sys.path.append('/home/os/Documents/arm-develop/fvp/Base_RevC_AEMvA_pkg/Iris/Python')
import iris.debug
model = iris.debug.NetworkModel("localhost",7101)
cpu = model.get_cpus()[0]
for cc in model.get_cpus():
    cc.add_bpt_reg("SCR_EL3")
model.run()