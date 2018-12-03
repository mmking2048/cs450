#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int
sys_getcount(void)
{
  int n;
  argint(0, &n);
  struct proc *curproc = myproc();
  return curproc->count[n];
}

int sys_v2p(void)
{
  int va;
  pde_t *pde;
  pte_t *pgtab;
  pte_t *pte;

  argint(0, &va);
  struct proc *curproc = myproc();

  // lookup pde from page directory
  pde = &(curproc->pgdir)[PDX(va)];
  if(*pde & PTE_P){
    // get page table from pde
    pgtab = (pte_t*)P2V(PTE_ADDR(*pde));
    // lookup pte from page table
    pte = (pte_t*)&pgtab[PTX(va)];
    // get physical address from pte
    return (int)&pte[OFFSET(va)];
  } else {
    // page table was not allocated
    return -1;
  }
}

int sys_threadcreate(void)
{
  void (*tmain)(void *);
  void *stack, *arg;
  argptr(0, (void*)&tmain, sizeof(tmain));
  argptr(1, (void*)&stack, sizeof(stack));
  argptr(2, (void*)&arg, sizeof(arg));
  return thread_create(tmain, stack, arg);
}

int sys_threadjoin(void)
{
  void **stack;
  argptr(0, (void*)&stack, sizeof(stack));
  return thread_join(stack);
}

int sys_mtxcreate(void)
{
  int locked;
  argint(0, &locked);
  return mtx_create(locked);
}

int sys_mtxlock(void)
{
  int lock_id;
  argint(0, &lock_id);
  return mtx_lock(lock_id);
}

int sys_mtxunlock(void)
{
  int lock_id;
  argint(0, &lock_id);
  return mtx_unlock(lock_id);
}

int sys_semcreate(void)
{
  struct semaphore *s;
  int val;
  argptr(0, (void *)&s, sizeof(s));
  argint(1, &val);
  initsemaphore(s, val);
  return 0;
}

int sys_semacquire(void)
{
  struct semaphore *s;
  argptr(0, (void *)&s, sizeof(s));
  acquiresemaphore(s);
  return 0;
}

int sys_semrelease(void)
{
  struct semaphore *s;
  argptr(0, (void *)&s, sizeof(s));
  releasesemaphore(s);
  return 0;
}
