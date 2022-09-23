#include "stdio.h"
#include "syscall.h"
#include <fcntl.h>
static int finit_module(int fd, const char *param_values, int flags)
{
	return (int)syscall(SYS_finit_module, fd, param_values, flags);
}
static void err(char * s){
    puts(s);
    exit(1);
}


int main(int argc, char *argv[])
{
	if (argc != 2) {
		err("error args");
	}
	char *driver_path = argv[1];
	int id = fork();
	if (id == 0) {
		int module_file = open(driver_path, O_RDONLY);
		if (module_file < 0) {
			sprintf("Failed to load Hafnium kernel module from %s\n", driver_path);
			exit(1);
		}
		if(finit_module(module_file, "", 0)!= 0){
            err("fnint module failed");
        }
		close(module_file);
	} else {
		exit(0);
	}
}