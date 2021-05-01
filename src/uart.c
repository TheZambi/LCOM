#include "uart.h"
#include "fireball.h"
#include "player.h"
#include "queue.h"
#include <lcom/lcf.h>

int serial_hook_id = 0;
fila *send_queue;
fila *receive_queue;
extern bool multiPlayer;
extern bool mainM;
extern bool wait;
extern bool connect;
struct Fireball fireball2;



int serial_subscribe_int(uint8_t *bit_no) {
  uint8_t aux;
  serial_hook_id = *bit_no;
  if (sys_irqsetpolicy(IRQ_UART_LINE, IRQ_EXCLUSIVE | IRQ_REENABLE, &serial_hook_id))
    return 1;
  sys_outb(FIFO_CONTROL_REG, BIT(FCR_ENABLE_FIFO) | BIT(FCR_CLEAR_RECEIVE_FIFO) | BIT(FCR_CLEAR_TRANSMIT_FIFO));

  util_sys_inb(INTERRUPT_ENABLE_REGISTER, &aux);

  aux = aux & 0xF0; 

  sys_outb(INTERRUPT_ENABLE_REGISTER, aux);

  util_sys_inb(INTERRUPT_ID_REG, &aux);

  sys_irqdisable(&serial_hook_id);

 

  clear_fifo_receiver();
  clear_fifo_transmitter();
  send_queue = fila_nova();
  receive_queue = fila_nova();
  return 0;
}

int serial_unsubscribe_int() {
  uint8_t aux;
  util_sys_inb(INTERRUPT_ENABLE_REGISTER, &aux);
  aux &=(BIT(7) | BIT(6) | BIT(5) | BIT(4) | BIT(3));
  sys_outb(INTERRUPT_ENABLE_REGISTER, aux);

  util_sys_inb(FIFO_CONTROL_REG, &aux);
  aux &=(BIT(7) | BIT(6) | BIT(5) | BIT(4) | BIT(3));
  sys_outb(FIFO_CONTROL_REG, aux);

  fila_apaga(send_queue);
  fila_apaga(receive_queue);
  clear_fifo_receiver();
  clear_fifo_transmitter();

  if (sys_irqrmpolicy(&serial_hook_id) != OK)
    return 1;

  return 0;
}


int setup_port() {
  uint8_t aux;
  uint32_t rate = 1;

  if (util_sys_inb(LINE_CONTROL_REG, &aux)) {
    return -1;
  }

  aux &= BIT(LCR_BREAK_CONTROL_BIT);
  aux |= BIT(LCR_NUM_STOP_BITS_BIT) | BIT(LCR_DLAB_BIT)| (BIT(6)) | (BIT(8)) ;

  if (sys_outb(LINE_CONTROL_REG, aux)) {
    return 1;
  }
  if (sys_outb(INTERRUPT_ENABLE_REGISTER, ((rate >> 16) & 0x00ff)) != 0) {
    return 1;
  }
  if (sys_outb(BASE_REG_COM1, (rate & 0x00ff)) != 0) {
    return 1;
  }
  if (sys_outb(LINE_CONTROL_REG, aux & ~BIT(LCR_DLAB_BIT)) != 0) {
    return 1;
  }
  return 0;
}

extern bool fireball2Thrown;
extern bool spaceIsStillPressed2;
bool shotLeft2 = false;
bool shotRight2 = false;
extern struct Player player2;

int read_queue(unsigned char aux) {
  uint8_t scancode;
  uint8_t fireballspeedY;
  uint8_t fireballspeedX;
  uint8_t statusChecker;
  int count = 0;
  if (aux == 'W') {
    connect = true;
    wait = true;
  }
  else if (!connect && wait && aux == 'C') {
    connect = true;
    wait = false;
  }
  else if (aux == 'f') {
    while (count < 2) {
      if (util_sys_inb(LINE_STATUS_REG, &statusChecker)) {
        return 1;
      }
      if (statusChecker & BIT(LSR_RECEIVER_DATA_BIT)) {
        if (count == 0) {

          if (util_sys_inb(BASE_REG_COM1, &fireballspeedX)) {
            return 1;
          }
          count++;
        }
        else if (count== 1) {
          if (util_sys_inb(BASE_REG_COM1, &fireballspeedY)) {
            return 1;
          }
          count++;
        }
      }
    }
    count = 0;
    fireball2.x = player2.x;
    fireball2.y = player2.y;
    fireball2.speedX = -(uint8_t) fireballspeedX + 10;
    fireball2.speedY = (uint8_t) fireballspeedY - 10;
    if(fireball2.speedX <0){
      shotLeft2= true;
      shotRight2 = false;
    }
    else{
      shotRight2 = false;
      shotLeft2 = true;
      fireball2.x = player2.x + player2.width;
    }
    fireball2Thrown = true;
  }
  else if (aux == 'k') {
    if (util_sys_inb(BASE_REG_COM1, &scancode)) {
      return 1;
    }
    if (scancode == 0x20 || scancode == 0xA0) {
      moveRight2(scancode & BIT(7)); // 1 if break
    }
    else if (scancode == 0x1e || scancode == 0x9e) {
      moveLeft2(scancode & BIT(7)); // 0 if make
    }
    else if (scancode == 0x39 && !player2.jump && !spaceIsStillPressed2) {
      player2.speedY = -23;
      player2.jump = true;
      spaceIsStillPressed2 = true;
    }
    else if (scancode == 0xB9 && spaceIsStillPressed2) {
      spaceIsStillPressed2 = false;
    }
  }
  return 0;
}

int transmit_queue() {
  uint8_t aux = 0;
  uint8_t read;
  unsigned char *word = (unsigned char *) malloc(sizeof(unsigned char));
  while ((fila_tamanho(send_queue) > 0)) {
    if (util_sys_inb(LINE_STATUS_REG, &read)) {
      return 1;
    }

    if ((read & BIT(5)) != 0) {
      word = fila_front(send_queue);
      aux = *word;
      if (sys_outb(BASE_REG_COM1, aux)) {
        return 1;
      }
      fila_pop(send_queue);
    }
  }
  return 0;
}

int receives_queue() {
  uint8_t auxIn;
  uint8_t statusChecker;
  unsigned char aux = 0;
  util_sys_inb(LINE_STATUS_REG, &statusChecker);

  while (statusChecker & BIT(LSR_RECEIVER_DATA_BIT)) {
    if (statusChecker & (BIT(LSR_OVERRUN_ERROR_BIT) | BIT(LSR_PARITY_ERROR_BIT) | BIT(LSR_FRAMING_ERROR_BIT))) {
      clear_fifo_receiver();
      clear_fifo_transmitter();
      return 1;
    }
    util_sys_inb(BASE_REG_COM1, &auxIn);
    aux = (unsigned char) auxIn;
    read_queue(aux);
    util_sys_inb(LINE_STATUS_REG, &statusChecker);
  }
  return 1;
}

void clear_fifo_receiver() {
  uint8_t aux;
  util_sys_inb(FIFO_CONTROL_REG, &aux);
  aux |=  BIT(1);
  sys_outb(FIFO_CONTROL_REG, aux);
  return;
}

void clear_fifo_transmitter() {
  uint8_t aux;
  util_sys_inb(FIFO_CONTROL_REG, &aux);
  aux |= BIT(2);
  sys_outb(FIFO_CONTROL_REG, aux);
  return;
}
