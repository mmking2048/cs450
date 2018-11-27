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
  return 0;
}

int sys_threadjoin(void)
{
  return 0;
}

int sys_mtxcreate(void)
{
  int locked;
  argint(0, &locked);
  
  struct proc *curproc = myproc();
  struct spinlock lock;
  int lockid = curproc->lockid;
  curproc->lockid++;

  if (lockid > NOLOCK)
    // too many locks
    return -1;

  initlock(&lock, "mutex");
  lock.locked = locked;
  curproc->locks[lockid] = &lock;
  return lockid;
}

int sys_mtxlock(void)
{
  int lock_id;
  argint(0, &lock_id);

  struct proc *curproc = myproc();
  if (lock_id > curproc->lockid)
    // invalid lock number
    return -1;

  struct spinlock *lock = curproc->locks[lock_id];
  acquire(lock);
  return 0;
}

int sys_mtxunlock(void)
{
  int lock_id;
  argint(0, &lock_id);

  struct proc *curproc = myproc();
  if (lock_id > curproc->lockid)
    // invalid lock number
    return -1;

  struct spinlock *lock = curproc->locks[lock_id];
  release(lock);
  return 0;
}
