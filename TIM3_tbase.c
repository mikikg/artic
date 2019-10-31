//---------------------------------
// Konfiguracija za TIM3 da radi kao 1kHz (1ms) timer
// Iskoriscen ovaj HW tajmer za jos 8 64bit SW tajmera 
//

#include "stm32f10x.h"
#include "TIM3_tbase.h"
#include "7seg.h"

volatile uint64_t SW_timers[8];

void TIM3_Setup_TBASE (void) {
	//ukljuci clock za TIM3
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

	//konfiguracija za TIM3 kao timer
	TIM3->ARR 	= 36000 * MY_TIM3_TIME_SCALE + 2; //auto reload vrednost za 1kHz
	TIM3->PSC 	= 0x01; //preskaler
	TIM3->DIER 	|= TIM_DIER_UIE; //enable update interupt
	TIM3->SR = RESET; //clear all flags

	//Enable interrupt
	NVIC_EnableIRQ(TIM3_IRQn); //enable channel 
	NVIC_SetPriority(TIM3_IRQn, 3); //interupt priority

	//run
	TIM3->CR1		|= TIM_CR1_CEN; //ENABLE tim periferial
}

//---------- TIM3 IRQ 100kHz / 10us -----------------
void TIM3_IRQHandler (void){	
	
	if (TIM3->SR & TIM_SR_UIF) {
		TIM3->SR &=~ TIM_SR_UIF;//clear flag

		//GPIOC->ODR ^= GPIO_ODR_ODR13; // Toggle LED 

		//inkrementuj 8 64bit SW tajmera, svako po potrebi resetuje
		SW_timers[0] ++;
		SW_timers[1] ++;
		SW_timers[2] ++;
		SW_timers[3] ++;
		SW_timers[4] ++;
		SW_timers[5] ++;
		SW_timers[6] ++;
		SW_timers[7] ++;
		
		//GPIOC->ODR ^= GPIO_ODR_ODR13; // Toggle LED 
			
		handler_led();
	
	} 
}

