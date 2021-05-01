#include "utils.h"
#include "i8254.h"

#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>

int numSysinbCalls = 0;

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if (lsb == NULL)
    return 1;
  val = val << 8;
  val = val >> 8;
  *lsb = (uint8_t) val;
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  if (msb == NULL)
    return 1;
  val = val >> 8;
  *msb = (uint8_t) val;
  return 0;
}

int(util_sys_inb)(int port, uint8_t *value) {
  uint32_t temp;
  if (sys_inb(port, &temp))
    return 1;
  numSysinbCalls++;
  *value = (uint8_t) temp;
  return 0;
}

uint16_t (convert8to16)(uint8_t toConv, bool isNeg)
{
  if(isNeg)
    return (BIT(15) | BIT(14) | BIT(13) | BIT(12) | BIT(11) | BIT(10) | BIT(9) | BIT(8) | toConv);
  else
  {
    return toConv;
  }
  
}

