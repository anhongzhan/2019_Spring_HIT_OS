#include <asm/segment.h>
#include <linux/kernel.h>
#include <unistd.h>
#include <string.h>
#include <linux/sched.h>

#define BufNum 5       //缓冲区只有10个
#define MaxNum 10      //向缓冲区写入的最大整数

typedef int key_t;

struct Myshm_tables
{
    key_t key;
  //  int flag;//0--地址还未分配  1--已经有内容  
    long addr;//地址
}Myshm[BufNum];

int get_shm(key_t key)
{
    int i;
    for(i=0;i<BufNum;i++){
        if(Myshm[i].key==key){
            return i;
        }
    }

    return -1;
}

int sys_shmget(key_t key){
    int i,shmid;
    if((shmid = get_shm(key)) != -1)
        return shmid;

    for(i=0;i<BufNum && Myshm[i].key == 0;i++){
        Myshm[i].key = key;
        Myshm[i].addr = get_free_page();
        return i;
    }
    return -1;
}

void *sys_shmat(int shmid){
    put_page(Myshm[shmid].addr, current->brk + current->start_code);
    return (void*)current->brk;
}