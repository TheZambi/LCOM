#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>

#include <minix/sysutil.h>

#include "i8254.h"
#include "kbd.h"
#include "utils.h"

int(kbd_subscribe_int)(uint8_t *bit_no) {
  kbd_hook_id = *bit_no;
  if (sys_irqsetpolicy(KBC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbd_hook_id))
    return 1;
  //  if(sys_irqenable(&kbd_hook_id))
  //    return 1;
  return 0;
}

int(kbd_unsubscribe_int)() {
  if (sys_irqrmpolicy(&kbd_hook_id))
    return 1;
  return 0;
}

void(kbc_ih)() {
  if (!util_sys_inb(STAT_REG, &statusChecker)) {
    if (!((statusChecker & ERR_PAR) || (statusChecker & ERR_TIMEOUT)))
      if (statusChecker & STAT_OUTBUF_FULL) {
        util_sys_inb(OUT_BUF, &scanCode);
      }
  }
}

int(issueCmd)(uint8_t cmd) {
  int n = 0;
  sys_outb(STAT_REG,0x60);
  while (n < 4) {
    util_sys_inb(STAT_REG, &statusChecker); /* assuming it returns OK */
    /* loop while 8042 input buffer is not empty */
    n++;
    if ((statusChecker & IBF) == 0) {
      sys_outb(OUT_BUF, cmd); /* no args command */
      return 0;
    }
    tickdelay(micros_to_ticks(DELAY_US));
  }
  return 0;
}
