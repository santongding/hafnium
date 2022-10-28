#include "stdio.h"
//#include "syscall.h"
void dfs(int num){
    if(num%100==0){
        printf("dfs: %d\n",num);
    }
    dfs(num+1);
}
__uint64_t read_el(){


    __uint64_t __val;                        
    asm volatile("mrs %0, currentEL" : "=r" (__val));    
    return __val;                            \
}
int main(){
    printf("hello, hafnium!\n");
    // printf("%x\n",read_el());
    //dfs(1);
    return 0;
}