#include "rtc.h"
#include "utils.h"

#define RTC_IRQ 8
#define RTC_DATA 0x71
#define RTC_ADDR 0x70
#define REG_A 10
#define REG_B 11
#define REG_C 12
#define REG_D 13

int rtc_hook_id;


int (rtc_subscribe_int)(uint8_t *bit_no) {
  rtc_hook_id=*bit_no;
  if(sys_irqsetpolicy(RTC_IRQ,IRQ_REENABLE,&rtc_hook_id))
    return 1;
  return 0;
}

int (rtc_unsubscribe_int)() {
  if(sys_irqrmpolicy(&rtc_hook_id))
    return 1;
  return 0;
}

void activate_rtc(){
  uint8_t reg_a,reg_b;

  sys_outb(RTC_ADDR,REG_A);
  util_sys_inb(RTC_DATA,&reg_a);
  reg_a = reg_a | BIT(3) | BIT(2) | BIT(1) | BIT(0);
  sys_outb(RTC_ADDR,REG_A);
  sys_outb(RTC_DATA,reg_a);


  sys_outb(RTC_ADDR,REG_B);
  util_sys_inb(RTC_DATA,&reg_b);
  reg_b = reg_b | BIT(6) | BIT(1);
  sys_outb(RTC_ADDR,REG_B);
  sys_outb(RTC_DATA,reg_b);
}

void deactivate_rtc(){
uint8_t reg_b, mask = BIT(7) | BIT(5) | BIT(4)| BIT(3)| BIT(2)| BIT(1)| BIT(0);

  sys_outb(RTC_ADDR,REG_B);
  util_sys_inb(RTC_DATA,&reg_b);
  reg_b = reg_b & mask;
  sys_outb(RTC_ADDR,REG_B);
  sys_outb(RTC_DATA,reg_b);
}

void readRegC(){
  uint8_t reg_c;
  sys_outb(RTC_ADDR,REG_C);
  util_sys_inb(RTC_DATA,&reg_c);
}

int rtc_in_bin(){
	uint8_t result;
	sys_outb(RTC_ADDR, REG_B);
	util_sys_inb(RTC_DATA, &result);

	return (result & BIT(2));
}

unsigned long convert_to_bin(unsigned long value){
	return (value & 0x0F) + (((value & 0x0F0) >> 4) * 10);
}

void readAll(uint8_t *seconds,uint8_t *minutes,uint8_t *hours){
  uint8_t read;
  uint8_t reg_a;

  sys_outb(RTC_ADDR,REG_A);
  util_sys_inb(RTC_DATA,&reg_a);

  while(BIT(7) & reg_a){ }
  sys_outb(RTC_ADDR,0);
  util_sys_inb(RTC_DATA,&read);
  *seconds = read;
  sys_outb(RTC_ADDR,2);
  util_sys_inb(RTC_DATA,&read);
   *minutes = read;
  sys_outb(RTC_ADDR,4);
  util_sys_inb(RTC_DATA,&read);
   *hours = read;

if(!rtc_in_bin()){
		*seconds = convert_to_bin(*seconds);
		*minutes = convert_to_bin(*minutes);
		*hours = convert_to_bin(*hours);
	}


}
