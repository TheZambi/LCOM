#include <lcom/lcf.h>
#include "utils.h"
#include <stdint.h>
#include <stdio.h>
#pragma once

int mouse_hook_id;

/** @defgroup mouse mouse
 * @{
 *
 * Module imported from labs.
 */


uint8_t mousePack;
uint8_t statusChecker;
int state;

#define OUT_BUF 0x60 /**< @brief Out buffer register */
#define STAT_REG 0x64 /**< @brief status register */
#define DELAY_US 20000 /**< @brief standard delay */
#define MOUSE_IRQ 12  /**< @brief mouse irq line */
#define ESC_SCANCODE 0x81 /**< @brief ESC key scan code */
#define KBC_READ_CMD 0x20 /**< @brief cmd to send to stat reg to read a cmg */
#define ERR_PAR BIT(7) /**< @brief parity error bit */
#define ERR_TIMEOUT BIT(6) /**< @brief timeout error bit */
#define STAT_OUTBUF_FULL BIT(0) /**< @brief out buffer full bit */
#define IBF BIT(1) /**< @brief input buffer bit */
#define OBF BIT(0) /**< @brief Out buffer bit */
#define ERR_ACK 0xFC  /**< @brief acknowledge error */
#define NACK 0xFE /**< @brief no acknowledge byte */

/**
 * @brief clears the cursor from the screen
 * 
 * @param buff Pointer to video memory
 */
void clearMouse(uint8_t *buff);
/**
 * @brief Subscribes mouse interrupts
 * 
 * @param bit_no Bit number pointer
 * @return int 
 */
int mouse_subscribe_int(uint8_t *bit_no);
/**
 * @brief Unsubscribes mouse interrupts
 * 
 * @return int 
 */
int mouse_unsubscribe_int();
/**
 * @brief Handles mouse interrupts
 * 
 */
void (mouse_ih)();
/**
 * @brief Issues a command to the mouse
 * 
 * @param cmd command to be sent
 * @return int 
 */
int mouse_command(uint8_t cmd);
/**
 * @brief Writes a command to the kbc
 * 
 * @param cmd command to be sent
 * @return int 
 */
int write_cmd(uint8_t cmd);
/**
 * @brief Reads a command from the kbc
 * 
 * @param cmd will be changed with the command read
 */
int read_cmd(uint8_t *cmd);
/**
 * @brief handles mouse events
 * 
 * @param packet packet to be handled
 */
void handleEvent(struct packet packet);
/**
 * @brief State machine that handles fireball throwing. State 1 is left click press, state 2 is movement and release
 * 
 */
void stateMachine();

/**@}*/
