/*
  7-seg drv

	PB5 - DP-SEG
	PA2 - A-SEG
	PA6 - B-SEG
	PB4 - C-SEG
	PB6 - D-SEG
	PB7 - E-SEG
	PA3 - F-SEG
	PB3 - G-SEG

	PA1 - SELL 1
	PA4 - SELL 2
	PA5 - SELL 3
	PA15- SELL 4
	PA7 - SELL 5
*/
#include "stm32f10x.h"

#define dig1_on   GPIOA->BSRR=GPIO_BSRR_BR1;
#define dig1_off  GPIOA->BSRR=GPIO_BSRR_BS1;
#define dig2_on   GPIOA->BSRR=GPIO_BSRR_BR4;
#define dig2_off  GPIOA->BSRR=GPIO_BSRR_BS4;
#define dig3_on   GPIOA->BSRR=GPIO_BSRR_BR5;
#define dig3_off  GPIOA->BSRR=GPIO_BSRR_BS5;
#define dig4_on   GPIOA->BSRR=GPIO_BSRR_BR15;
#define dig4_off  GPIOA->BSRR=GPIO_BSRR_BS15;
#define dig5_on   GPIOA->BSRR=GPIO_BSRR_BR7;
#define dig5_off  GPIOA->BSRR=GPIO_BSRR_BS7;

#define DP_0 GPIOB->BSRR=GPIO_BSRR_BR5; //DP ON
#define DP_1 GPIOB->BSRR=GPIO_BSRR_BS5; //DP OFF
#define A_1  GPIOA->BSRR=GPIO_BSRR_BS2; //A seg
#define B_1  GPIOA->BSRR=GPIO_BSRR_BS6; //B seg
#define C_1  GPIOB->BSRR=GPIO_BSRR_BS4; //C seg
#define D_1  GPIOB->BSRR=GPIO_BSRR_BS6; //D seg
#define E_1  GPIOB->BSRR=GPIO_BSRR_BS7; //E seg
#define F_1  GPIOA->BSRR=GPIO_BSRR_BS3; //F seg
#define G_1  GPIOB->BSRR=GPIO_BSRR_BS3; //G seg
#define A_0  GPIOA->BSRR=GPIO_BSRR_BR2; //A seg
#define B_0  GPIOA->BSRR=GPIO_BSRR_BR6; //B seg
#define C_0  GPIOB->BSRR=GPIO_BSRR_BR4; //C seg
#define D_0  GPIOB->BSRR=GPIO_BSRR_BR6; //D seg
#define E_0  GPIOB->BSRR=GPIO_BSRR_BR7; //E seg
#define F_0  GPIOA->BSRR=GPIO_BSRR_BR3; //F seg
#define G_0  GPIOB->BSRR=GPIO_BSRR_BR3; //G seg

void seg7InitPins(void);
void IntTo7seg(int num);
void handler_led(void);

