#pragma once
#include "lcom/lcf.h"
#include "stdint.h"

/** @defgroup enemy enemy
 * @{
 *
 * Functions that deals with the uart for multiplayer.
 */


// ADRESSES
#define INTERRUPT_ENABLE_REGISTER 0x3F9 /**< @brief IER adress*/
#define INTERRUPT_ID_REG 0x3FA /**< @brief IIR adress*/
#define BASE_REG_COM1 0x3F8 /**< @brief COM1 adress*/
#define LINE_CONTROL_REG 0x3FB /**< @brief LCR adress*/
#define FIFO_CONTROL_REG 0x3FA /**< @brief FCR adress*/
#define IRQ_UART_LINE 4 /**< @brief COM1 irq line adress*/
#define LINE_STATUS_REG 0x3FD /**< @brief LSR adress*/

// FIFO 
#define FCR_ENABLE_FIFO 0 /**< @brief enable FIFO bit*/
#define FCR_CLEAR_RECEIVE_FIFO 1 /**< @brief clear FIFO receiver bit*/
#define FCR_CLEAR_TRANSMIT_FIFO 2 /**< @brief clear FIFO trasmitter bit*/

//  LINE CONTROL REGISTER(LCR)
#define LCR_NUM_STOP_BITS_BIT 2 /**< @brief LCR num of stop bits bit*/
#define LCR_BITS_PER_CHAR_BIT 0 /**< @brief LCR num of bits per char bit*/
#define LCR_DLAB_BIT 7 /**< @brief LCR DLAB bit*/
#define LCR_PARITY_CONTROL_BIT 3 /**< @brief LCR parity control bit*/
#define LCR_BREAK_CONTROL_BIT 6 /**< @brief LCR break control bit*/


// LINE STATUS REGISTER (LSR)
#define LSR_OVERRUN_ERROR_BIT 1 /**< @brief LSR overrun error bit*/
#define LSR_PARITY_ERROR_BIT 2 /**< @brief LSR parity error bit*/
#define LSR_RECEIVER_DATA_BIT 0 /**< @brief LSR Data receiver bit*/
#define LSR_FRAMING_ERROR_BIT 3 /**< @brief LSR framing error bit*/





/**
 * @brief subscirbes interrupts from the serial port and disables them so minix doesnt take them
 */
int serial_subscribe_int(uint8_t *bit_no);

/**
 * @brief unsubscribes interrupts from the serial port
 */
int serial_unsubscribe_int();
/**
 * @brief setup serial port config
 * 
 */
int setup_port();

/**
 * @brief Reads elements in receive_queue
 */
int read_queue(unsigned char aux);

/**
 * @brief send the transmit queue
 */
int transmit_queue();

/**
 * @brief receives the other pc's transmit queue and adds to receive queue
 */
int receives_queue();
 
/**
 * @brief Clean fifo receiver
 */
void clear_fifo_receiver();

/**
 * @brief Clean fifo transmitter
 * 
 */
void clear_fifo_transmitter();

/**@}*/
