#include "mono.h"
#include "aarch64.h"
#include "printk.h"
#include "proc.h"
#include "trap.h"
#include "string.h"

#define PAGESIZE 4096

struct proc proctable[NPROC];
/* proctable[0] == kernel proc */

struct proc *curproc = NULL;
struct proc kproc;

#define MEMBASE 0x44000000llu
void *allocpage() {
  static int i = 0;
  return (void *)(MEMBASE + (i++) * PAGESIZE);
}

void trapret(void);

void forkret() {
  printk("forkret");
  trapret();
}

/* FIXME: tmp */
pid_t newproc(void (*fn)(void)) {
  static pid_t pid = 1;
  struct proc *p;

  for(int i = 1; i < NPROC; i++) {
    p = &proctable[i];
    if(p->state == UNUSED)
      goto found;
  }

  return -1;

found:
  p->pid = pid++;
  memset(&p->context, 0, sizeof(p->context));

  char *kstack = allocpage();

  char *sp = kstack + PAGESIZE;
  sp -= sizeof(struct trapframe);
  p->tf = (struct trapframe *)sp;
  memset(p->tf, 0, sizeof(struct trapframe));

  char *ustack = allocpage();
  char *usp = ustack + PAGESIZE;

  p->tf->elr = (u64)fn; // `eret` jump to elr(== fn)
  p->tf->spsr = 0x0;    // switch EL1 to EL0
  p->tf->sp = (u64)usp; // sp_el0

  printk("neeeeeeeeewproc %p %p %p\n", fn, kstack, ustack);

  p->stack = kstack;
  p->context.lr = (u64)forkret;
  p->context.sp = (u64)sp;    //== p->tf

  p->state = RUNNABLE;

  return pid;
}

void schedule() {
  printk("schhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhed\n");

  for(;;) {
    enable_irq();

    for(int i = 1; i < NPROC; i++) {
      struct proc *p = &proctable[i];

      if(p->state == RUNNABLE) {
        printk("found runnable proc %d\n", i);
        printk("daiffffff %p\n", daif());
        p->state = RUNNING;
        curproc = p;

        cswitch(&kproc.context, &curproc->context);

        curproc = NULL;
        printk("okaeriiiiiiiii %p\n", daif());
      }
    }
  }
}

void yield() {
  printk("yyyyyyyield\n");
  if(!curproc) {
    panic("bad yield");
  }
  curproc->state = RUNNABLE;
  cswitch(&curproc->context, &kproc.context);
  printk("konnnnnnnnnnnnichiah????\n");
}

void curproc_dump() {
  ;
}

static void init_firstproc() {
  struct proc *first = &proctable[1];
  first->pid = 1;
  memset(&first->context, 0, sizeof(first->context));

  char *kstack = allocpage();

  char *sp = kstack + PAGESIZE;
  sp -= sizeof(struct trapframe);
  first->tf = (struct trapframe *)sp;
  memset(first->tf, 0, sizeof(struct trapframe));

  first->tf->elr = 0x0; // FIXME
  first->tf->spsr = 0x0;  // switch to EL0

  first->stack = kstack;
  first->context.lr = (u64)forkret;
  first->context.sp = (u64)sp;

  first->state = RUNNABLE;
}

void proc_init() {
  memset(&kproc, 0, sizeof(kproc));
  kproc.state = RUNNABLE;
  proctable[0] = kproc;
  curproc = &kproc;
}

