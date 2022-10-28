from audioop import add
import sys, os
sys.path.append('/home/os/Documents/arm-develop/fvp/Base_RevC_AEMvA_pkg/Iris/Python')
import iris.debug
model = iris.debug.NetworkModel("localhost",7100)
brk_map = dict()
def read_int32(cpu, addr):
    mem = cpu.read_memory(addr, size = 4, count = 1)
    ans = int(0)
    for i in range(4):
        ans = ans | mem[-(i+1)] << (8*i)
    return ans
def get_disass(cpu, addr):
    disass = cpu.disassemble(addr, count = 1)
    for v in disass:
        return v["disass"]
def printregs(cpu, bg = 0, ed = 4):
    ans = []
    for i in range(bg,ed):
        ans.append("x{}: {:08x}".format(i,cpu.read_register("X{}".format(i))))
    print(", ".join([str(x) for x in ans]))
def printreg(cpu, name_list):
    ans = []
    for name in name_list:
        ans.append("{}: {:8x}".format(name,cpu.read_register(name)))
    print(" ".join(ans))

def handle_start(cpu, pc):
    # print(cpu._memory_spaces_by_name["Current"])
    # print("entering breakpoint {}, mem:{}".format(pc, read_int32(cpu, pc)))

    for i in range(100):
        disass = get_disass(cpu,pc)
        print("pc:{:08x}, value:{:08x} disass:{}".format(int(pc), read_int32(cpu, pc), disass))
        printregs(cpu)
        printreg(cpu, ["FAR_EL1", "ESR_EL1"],)

        if "NOP" in disass:
            break
        model.step(count=1)
        pc = cpu.read_register("PC")
        # print(cpu.get_steps(unit = 'instruction'))
    pass
def set_brk(cpu, addr, handler):
    for space in cpu._memory_spaces_by_name.keys():
        try:
            cpu.add_bpt_prog(addr, memory_space=space)
        except:
            pass
            # print(space, "can not set bnp")
    
    brk_map[addr] = handler


print(model.get_cpus()[0]._memory_spaces_by_name.keys())
for cc in model.get_cpus():
    print(cc.read_register("PC"))
    set_brk(cc, 0x63820f0, handle_start)

for k,v in brk_map.items():
    print(type(k),k)
model.run()
# model.step(5000000)

for cc in model.get_cpus():
    pc = int(cc.read_register("PC"))
    if pc in brk_map:
        brk_map[pc](cc, pc) 
    else:
        print("addr {} not in map".format(pc))
model.stop()