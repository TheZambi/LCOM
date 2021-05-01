#include "mouse.h"
#include "vbe.h"
#include "objects.h"
#include "player.h"
#include "fireball.h"

int mouse_hook_id = 0;
int state = 0;
bool hadEvent = false;

uint8_t mousePack;
uint8_t statusChecker;
extern uint8_t * background;
struct mouse_ev event;
int totalMovX = 0;
int totalMovY = 0;
extern bool fireballThrown;
extern struct Cursor cursor;
extern struct Fireball fireball;
extern vbe_mode_info_t vbe;
extern struct Player player1;
bool shotLeft=false;
bool shotRight=false;
extern bool mainM;
extern bool sent;

void clearMouse(uint8_t *buff)
{
  for (int row = 0; row < cursor.height; row++) ///// BACKGROUND
  {
    for (int col = 0; col < cursor.width; col++)
    {
      
      drawPixel(&(background[((cursor.x+col)+(cursor.y+row)*vbe.XResolution)*((vbe.BitsPerPixel+7)/8)]),buff,cursor.y+row,cursor.x+col);
    }
  }
}


int(mouse_subscribe_int)(uint8_t *bit_no) {
  mouse_hook_id = *bit_no;
  if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_hook_id))
    return 1;

  return 0;
}

int(mouse_unsubscribe_int)() {
  if (sys_irqrmpolicy(&mouse_hook_id))
    return 1;
  return 0;
}

void(mouse_ih)() {
  if (!util_sys_inb(STAT_REG, &statusChecker)) {
    if (!((statusChecker & ERR_PAR) || (statusChecker & ERR_TIMEOUT)))
      if (statusChecker & STAT_OUTBUF_FULL) {
        util_sys_inb(OUT_BUF, &mousePack);
      }
  }
}

int(mouse_command)(uint8_t cmd) {
  int n = 0;
  uint8_t test = 0;

  while (n < 4) {
    util_sys_inb(STAT_REG, &statusChecker); /* assuming it returns OK */
    /* loop while 8042 input buffer is not empty */
    if ((statusChecker & IBF) == 0) {
      sys_outb(STAT_REG, 0xD4); /* no args command */
      break;
    }
    tickdelay(micros_to_ticks(DELAY_US));
    n++;
  }
  if (n == 4)
    return 1;
  n = 0;

  while (n < 4) {
    util_sys_inb(STAT_REG, &statusChecker); /* assuming it returns OK */
    /* loop while 8042 input buffer is not empty */
    n++;
    if ((statusChecker & IBF) == 0) {
      sys_outb(OUT_BUF, cmd); /* no args command */
      break;
    }
    tickdelay(micros_to_ticks(DELAY_US));
  }
  if (n == 4)
    return 1;
  n = 0;

  while (n < 4) {
    util_sys_inb(STAT_REG, &statusChecker); /* assuming it returns OK */
    /* loop while 8042 input buffer is not empty */
    n++;
    if ((statusChecker & OBF) && (!((statusChecker & ERR_PAR) || (statusChecker & ERR_TIMEOUT)))) {
      util_sys_inb(OUT_BUF, &test); /* no args command */
      break;
    }
    tickdelay(micros_to_ticks(DELAY_US));
  }
  if (n == 4)
    return 1;
  if (test == ERR_ACK || test == NACK)
    return 1;
  return 0;
}

int(write_cmd)(uint8_t cmd) {
  int n = 0;
  sys_outb(STAT_REG, 0x60);
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

int(read_cmd)(uint8_t *cmd) {
  sys_outb(STAT_REG, 0x20);
  util_sys_inb(OUT_BUF, cmd); /* no args command */
  return 0;
}

void handleEvent(struct packet packet) {
  static struct packet copyOfLastPacket;
  if (copyOfLastPacket.lb != packet.lb) {
    if (packet.lb == 1) {
      event.type = LB_PRESSED;
      hadEvent = true;
    }
    else {
      event.type = LB_RELEASED;
      hadEvent = true;
    }
  }
  else if (copyOfLastPacket.rb != packet.rb) {
    if (packet.rb == 1) {
      event.type = RB_PRESSED;
      hadEvent = true;
    }
    else {
      event.type = RB_RELEASED;
      hadEvent = true;
    }
  }
  else if ((packet.delta_x != 0) || (packet.delta_y != 0)) {
    event.type = MOUSE_MOV;
    hadEvent = true;
  }
  if (packet.mb + packet.rb + packet.lb > 1) {
    event.type = BUTTON_EV;
    hadEvent = true;
  }
  copyOfLastPacket = packet;
}

void stateMachine() {
  static int initialX = 0;
  static int initialY = 0;
  if (mainM == true)
    return;
  switch (state) {
    case 0:
      if (event.type == LB_PRESSED) {
        initialX = cursor.x;
        initialY = cursor.y;
        state++;
      }
      break;
    case 1:
      if (event.type == LB_RELEASED) {
        totalMovX = cursor.x - initialX;
        totalMovY = cursor.y - initialY;
        if (cursor.x - initialX != 0 || cursor.y - initialY != 0) {
          if (!fireballThrown) {
            sent = false;
            fireballThrown = true;
            fireballAngle();
            if (totalMovX < 0) {
              shotLeft = true;
              fireball.x = player1.x;
              fireball.y = player1.y;
            }
            else {
              shotRight = true;
              fireball.x = player1.x + player1.width - 30;
              fireball.y = player1.y;
            }
            state = 0;
          }
        }
      }
      break;
  }
}
