#include "stm32f10x.h"
//#include "MCU_init.h"
//#include "GPIO_init.h"
#include "FRAM_spi.h"
#include "7seg.h"
#include "TIM3_tbase.h"

extern volatile uint64_t SW_timers[8];

void komparator (void);

//Funkcija koja grupise sve init-e
void MYSYS_INIT (void) {

		//ukljuci clock za APB2, on je za sve GPIOx
		RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; //GPIOA
		RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; //GPIOB
		RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; //GPIOC

		//Alternate Function I/O clock enable - Treba da bi proradile GPIO na JTAG, EXTI, I2C
		RCC->APB2ENR |=	RCC_APB2ENR_AFIOEN;
    
		//oslobodi nozice od JTAG, koristimo samo SWD
		AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE; //JTAG-DP Disabled and SW-DP Enabled
	
    //konfiguracija za PC13
    //po default svi pinovi su Input
    //ima dva registra CRL za pinove 0-7 i CRH za pinove 8-15
    //definisi pin kao general purpose Output push-pull, 50MHz
    GPIOC->CRH &= ~GPIO_CRH_CNF13_0; //ocisti bit CNF13_0
    GPIOC->CRH &= ~GPIO_CRH_CNF13_1; //ocisti bit CNF13_1
    GPIOC->CRH |=  GPIO_CRH_MODE13_0; //setuj bit MODE13_0
    GPIOC->CRH |=  GPIO_CRH_MODE13_1; //setuj bit MODE13_1

		//--- TIC Output ---
    //PB8 general purpose Output push-pull, 50MHz
    GPIOB->CRH &= ~GPIO_CRH_CNF8_0; //ocisti bit CNF13_0
    GPIOB->CRH &= ~GPIO_CRH_CNF8_1; //ocisti bit CNF13_1
    GPIOB->CRH |=  GPIO_CRH_MODE8_0; //setuj bit MODE13_0
    GPIOB->CRH |=  GPIO_CRH_MODE8_1; //setuj bit MODE13_1
	  GPIOB->BSRR = GPIO_BSRR_BR8;

		//--- TIC Input ---
		//PB9 definisi ga kao INPUT, pull-up
		GPIOB->CRH &= ~GPIO_CRH_CNF9_0; //ocisti bit
		GPIOB->CRH |=  GPIO_CRH_CNF9_1; //setuj bit
		GPIOB->CRH	&= ~GPIO_CRH_MODE9_0; //ocisti bit
		GPIOB->CRH &= ~GPIO_CRH_MODE9_1; //ocisti bit
		GPIOB->BSRR =  GPIO_BSRR_BR9; //BS=1=pull-up, BR=0=pull-down 

		//--- KBD inputs ---
		//PA8 - (+) definisi ga kao INPUT
		GPIOA->CRH &= ~GPIO_CRH_CNF8_0; //ocisti bit
		GPIOA->CRH |=  GPIO_CRH_CNF8_1; //setuj bit
		GPIOA->CRH	&= ~GPIO_CRH_MODE8_0; //ocisti bit
		GPIOA->CRH &= ~GPIO_CRH_MODE8_1; //ocisti bit
		GPIOA->BSRR =  GPIO_BSRR_BR8; //BS=1=pull-up, BR=0=pull-down 
		//PA9 - (-) definisi ga kao INPUT
		GPIOA->CRH &= ~GPIO_CRH_CNF9_0; //ocisti bit
		GPIOA->CRH |=  GPIO_CRH_CNF9_1; //setuj bit
		GPIOA->CRH	&= ~GPIO_CRH_MODE9_0; //ocisti bit
		GPIOA->CRH &= ~GPIO_CRH_MODE9_1; //ocisti bit
		GPIOA->BSRR =  GPIO_BSRR_BR9; //BS=1=pull-up, BR=0=pull-down 
		//PA10 - (Run/Stop) definisi ga kao INPUT
		GPIOA->CRH &= ~GPIO_CRH_CNF10_0; //ocisti bit
		GPIOA->CRH |=  GPIO_CRH_CNF10_1; //setuj bit
		GPIOA->CRH	&= ~GPIO_CRH_MODE10_0; //ocisti bit
		GPIOA->CRH &= ~GPIO_CRH_MODE10_1; //ocisti bit
		GPIOA->BSRR =  GPIO_BSRR_BR10; //BS=1=pull-up, BR=0=pull-down 
		//PA11 - (Edit/View) definisi ga kao INPUT
		GPIOA->CRH &= ~GPIO_CRH_CNF11_0; //ocisti bit
		GPIOA->CRH |=  GPIO_CRH_CNF11_1; //setuj bit
		GPIOA->CRH	&= ~GPIO_CRH_MODE11_0; //ocisti bit
		GPIOA->CRH &= ~GPIO_CRH_MODE11_1; //ocisti bit
		GPIOA->BSRR =  GPIO_BSRR_BR11; //BS=1=pull-up, BR=0=pull-down 
		//PA12 - (Signal/Pause) definisi ga kao INPUT
		GPIOA->CRH &= ~GPIO_CRH_CNF12_0; //ocisti bit
		GPIOA->CRH |=  GPIO_CRH_CNF12_1; //setuj bit
		GPIOA->CRH	&= ~GPIO_CRH_MODE12_0; //ocisti bit
		GPIOA->CRH &= ~GPIO_CRH_MODE12_1; //ocisti bit
		GPIOA->BSRR =  GPIO_BSRR_BR12; //BS=1=pull-up, BR=0=pull-down 



	//---------- init --------- 
	//MCU_init (); //clock i ostalo
	//GPIO_init(); //GPIO
	
	seg7InitPins(); //7-seg LED
	FR_SPI2_init(); //FRAM
	
	TIM3_Setup_TBASE();
	
	//popuni memoriju FRAM -> RAM
	//for (int xx=0; xx<63; xx++) MyMemory[xx] = FR_SPI1_get32bitWord(xx);
	//MyEncoder[OFFSET] = MyMemory[25];
	
	//TIM1_Setup_OPM(); //OPM
	//TIM2_Setup_ENC(); //Encoder
	//USART1_init(); //Serial 1 RS-485
	//USART3_init(); //Serial 3 KBD
	//TIM3_Setup_TBASE(); //timer 100kHz
	//TIM4_Setup_PWM(); //PWM out 

};

