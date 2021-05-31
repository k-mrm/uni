#include "mono.h"
#include "arm.h"
#include "printk.h"
#include "proc.h"
#include "string.h"

#define PAGESIZE 4096

struct proc proctable[NPROC];
/* proctable[0] == kernel proc */

struct proc *curproc = NULL;
struct proc kproc;

void proc_init() {
  memset(&kproc, 0, sizeof(kproc));
  kproc.state = RUNNING;
  proctable[0] = kproc;
}

#define MEMBASE 0x200000llu
void *allocpage() {
  static int i = 0;
  return (void *)(MEMBASE + (i++) * PAGESIZE);
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

  char *stack = allocpage();

  printk("neeeeeeeeewproc %p %p\n", fn, stack);

  p->context.x0 = (u64)fn;
  p->context.lr = (u64)forkret;
  p->context.sp = (u64)stack + PAGESIZE;

  p->state = RUNNABLE;

  return pid;
}

void schedule() {
  printk("schhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhed\n");
  for(;;) {
    for(int i = 1; i < NPROC; i++) {
      enable_irq();
      struct proc *p = &proctable[i];
      if(p->state == RUNNABLE) {
        printk("found runnable proc %d\n", i);
        printk("daiffffff %p\n", daif());
        p->state = RUNNING;
        curproc = p;
        cswitch(&kproc.context, &p->context);
        curproc = NULL;
        printk("okaeriiiiiiiii %d %p\n", i, daif());
      }
    }
  }
}

static void swtch_sched() {
  cswitch(&curproc->context, &kproc.context);
}

void yield() {
  printk("yyyyyyyield\n");
  if(!curproc) {
    panic("bad yield");
  }
  curproc->state = RUNNABLE;
  swtch_sched();
  printk("konnnnnnnnnnnnichiah????\n");
}

void curproc_dump() {
  ;
}
