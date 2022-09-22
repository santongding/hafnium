#include "stdio.h"
//#include "syscall.h"
void dfs(int num){
    if(num%100==0){
        printf("dfs: %d\n",num);
    }
    dfs(num+1);
}
int _start(){
    printf("hello, hafnium!\n");
    dfs(1);
    exit(0);
}