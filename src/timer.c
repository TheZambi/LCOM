#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "timer.h"

#include "i8254.h"

int nInterrupts=0; 
int hook_id;


int (timer_subscribe_int)(uint8_t *bit_no) {
  hook_id=*bit_no;
  if(sys_irqsetpolicy(TIMER0_IRQ,IRQ_REENABLE,&hook_id))
    return 1;
  return 0;
}

int (timer_unsubscribe_int)() {
  if(sys_irqrmpolicy(&hook_id))
    return 1;
  return 0;
}

void (timer_int_handler)() {
  nInterrupts++;
}
