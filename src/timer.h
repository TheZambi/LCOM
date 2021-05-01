
/** @defgroup enemy enemy
 * @{
 *
 * Functions that deal with the timer.
 */



/**
 * @brief Subscribes timer interrupts
 * 
 * @param bit_no Bit number pointer
 * @return int Returns 1 if an error ocurred else returns 0
 */

int (timer_subscribe_int)(uint8_t *bit_no);
/**
 * @brief Unsubscribes timer interrupts
 * 
 * @return int Returns 1 if an error ocurred else returns 0
 */

int (timer_unsubscribe_int)();
/**
 * @brief Handles timer interrupts(add 1 to a variable every timer interruption)
 * 
 */
void (timer_int_handler)();

/**@}*/
