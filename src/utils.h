#ifndef UTILS_H
#define UTILS_H

#include <lcom/lcf.h>

#define BIT(n)  (1 << (n))

/** @defgroup utils utils
 * @{
 *
 * Functions that can prove usefull throughout the whole code.
 */


/**
 * @brief Fetches the least significative byte of a value
 * 
 * @param val Value with tow bytes from which the least significative byte will be copied
 * @param lsb Pointer to the variable which will hold the least significative byte
 * @return int Returns 1 if an error ocurred else returns 0
 */
int (util_get_LSB)(uint16_t val, uint8_t *lsb);

/**
 * @brief Fetches the most significative byte of a value
 * 
 * @param val Value with tow bytes from which the most significative byte will be copied
 * @param lsb Pointer to the variable which will hold the most significative byte
 * @return int Returns 1 if an error ocurred else returns 0
 */
int (util_get_MSB)(uint16_t val, uint8_t *msb);

/**
 * @brief Performs a sys_inb with a 8 bit value
 * 
 * @param port Address where the sys_inb will be made
 * @param value Variable which will hold the value gotten from the sys_inb
 * @return int Returns 1 if an error ocurred else returns 0
 */
int util_sys_inb(int port, uint8_t *value);

/**
 * @brief Converts a 8 bit value to the corresponding 16 bit value
 * 
 * @param toConv 8 bit value wich will be converted
 * @param isNeg A bool that indicates if the value is negative or not (fill with 1 if negative or 0 if positive to mantain the two's byte complement)
 * @return uint16_t Returns the 16 bit value
 */
uint16_t convert8to16(uint8_t toConv, bool isNeg);

/**@}*/

#endif
