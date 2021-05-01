#include <lcom/lcf.h>

/** @defgroup rtc rtc
 * @{
 *
 * Functions that deal with the rtc.
 */


/**
 * @brief Subscribes real time clock's interruptions
 * 
 * @param bit_no Bit number pointer
 * @return int Returns 1 if an error ocurred else returns 0
 */


int (rtc_subscribe_int)(uint8_t *bit_no);
/**
 * @brief Unsubscribes real time clock's interruptions
 * 
 * @return int Returns 1 if an error ocurred else returns 0
 */

int (rtc_unsubscribe_int)();
/**
 * @brief Activates the real time clock in periodical mode
 * 
 */
void activate_rtc();
/**
 * @brief Deactivates real time clock
 * 
 */
void deactivate_rtc();
/**
 * @brief Reads real time clock's c register
 * 
 */
void readRegC();
/**
 * @brief Reads to 3 diferent variables the seconds, minutes and hours of the real time clock
 * 
 * @param seconds Variable that saves the real time clock's current seconds
 * @param minutes Variable that saves the real time clock's current minutes
 * @param hours Variable that saves the real time clock's current hours
 */
void readAll(uint8_t *seconds,uint8_t *minutes,uint8_t *hours);
/**
 * @brief checks if the real time clock is in binary mode or not
 * 
 * @return int Returns 0 if not in binary 4 if its in binary
 */
int rtc_in_bin();
/**
 * @brief Converts the value to binary (used in the code after calling the function "rtc_in_bin()")
 * 
 * @param value Value to be converted to binary
 * @return unsigned long Binary representation of Value
 */
unsigned long convert_to_bin(unsigned long value);

/**@}*/
