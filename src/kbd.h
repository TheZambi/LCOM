#include <lcom/lcf.h>
#pragma once

int kbd_hook_id;

uint8_t scanCode;
uint8_t statusChecker;

#define OUT_BUF 0x60 /**< @brief Out buffer register */
#define STAT_REG 0x64 /**< @brief status register */
#define DELAY_US 20000 /**< @brief standard delay */
#define KBC_IRQ 0x01 /**< @brief irq line for kbc */
#define ESC_SCANCODE 0x81 /**< @brief ESC key scan code */
#define KBC_READ_CMD 0x20 /**< @brief cmd to send to stat reg to read a cmg */
#define ERR_PAR BIT(7) /**< @brief parity error bit */
#define ERR_TIMEOUT BIT(6) /**< @brief timeout error bit */
#define STAT_OUTBUF_FULL BIT(0) /**< @brief out buffer full bit */
#define IBF BIT(1) /**< @brief input buffer bit */
#define OBF BIT(0) /**< @brief Out buffer bit */

/**
 * @brief Subscribes keyboard interrupts
 * 
 * @param bit_no Bit number pointer
 * @return int 
 */
int kbd_subscribe_int(uint8_t *bit_no);
/**
 * @brief Unsubscribes keyboard interrupts
 * 
 * @return int 
 */
int kbd_unsubscribe_int();
/**
 * @brief handles keyboard interrupts
 * 
 */
void (kbc_ih)();
/**
 * @brief issues a command to the kbc
 * 
 * @param cmd command to be issued
 * @return int 
 */
int issueCmd(uint8_t cmd);

/**@}*/
