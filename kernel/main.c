#include "mono.h"
#include "arm.h"
#include "gpio.h"
#include "framebuffer.h"
#include "console.h"
#include "printk.h"
#include "systimer.h"
#include "exception.h"

int main(void) {
  fb_init();
  cs_init();
  systimer_init(500);

  printk("mono os for raspberry pi 4\n");
  printk("%d%%\n", 24000);
  printk("%d %u\n", -1, -1);
  printk("systime %d\n", systime());
  printk("systime %d\n", systime());
  printk("current EL: %d\n", cur_el());
  printk("%s%c\n", "Hello, raspi4", '!');
  printk("addr: %p\n", printk);
  printk("var_el1: %p\n", vbar_el1());

  for(;;) {}

  return 0;
}
