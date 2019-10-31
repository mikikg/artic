/*

ARTIC - Alfa Robotica Timer and Counter v1.0
(c) 2017 - 2018

Bare metal za BluePill plocicu sa STM32F103C8
Koristi se samo osnovni CMSIS core bez dodatnih drajvera
Pokrenuto kroz Keil uVision 5

Kontoler poseduje:
- 5 x 7-SEG LED displej
- 1 ulaz PB9
- 1 izlaz PB8
- taster + PA8
- taster - PA9
- prekidac Run/Stop PA10
- prekidac Edit/View PA11
- prekidac Signal/Pause PA12

*/

#include "main.h"
#include "stm32f10x.h"

#define BUDUCNOST 15058997920000L //TS u buducnosti 2087. iskazan u 10us

#define bf My7segBUFF
#define delay1ms(x) while (SW_timers[SWT_1msec] < x) {}; SW_timers[SWT_1msec]=0; //cekaj x milisec - blocking
#define fillBuff(a,b,c,d,e) bf[0]=a;	bf[1]=b; bf[2]=c; bf[3]=d; bf[4]=e;

	
extern char My7segBUFF[6];
extern int digit_current_dp; //DP 0=nema, 1=prva levo,2=sredina 3=desno

enum {KB_plus, KB_minus, KBS_run, KBS_edit, KBS_s_p};
uint64_t myKBtime[5]; //integrator za tastaturu
uint64_t myKBflag[5]; //flagovi za ivice za tastaturu
int kbIN;

uint64_t previous_edit_flag; //ivica za snimanje u memoriju

uint64_t TIC_signal_setpoint = 45000; //setpoint za glavni timer za SIGNAL
uint64_t TIC_signal_cnt; //brojac za glavni timer za SIGNAL
uint64_t TIC_pause_setpoint = 150; //setpoint za glavni timer za PAUZU
uint64_t TIC_pause_cnt; //brojac za glavni timer za PAUZU
uint64_t TIC_total_cnt; //brojac za glavni timer za CIKLUSA
uint64_t TIC_INPUT_time; //brojac za filter inputa, svaki prolaz kroz loop
	
//SW_timers - baza 1kHz (utrimovano u Hz!)
enum {SWT_1msec, SWT_LED, SWT_KBD, SWT_kscan_1, SWT_kscan_2, SWT_saved, XX_SWT_3CHAR, SWT_1min, XX_unused2}; //nazivi tajmera (SW_timers indexi u array 0-7)

int main(void) {
	
		MYSYS_INIT();
	
	//fabricki reset :)
			//	FR_SPI2_set32bitWord(NV_BOOT_CNT, 0);
			//FR_SPI2_set32bitWord(NV_TOTAL_CNT, 0);

	
		fillBuff('A','r','t','i','c'); 									delay1ms(2000);
		fillBuff(' ', 2,  0,  1 , 8 ); 									delay1ms(1000);
		fillBuff('B','o','o','t','S'); 									delay1ms(1000);
		IntTo7seg(FR_SPI2_get32bitWord (NV_BOOT_CNT)); 	delay1ms(1000);
		fillBuff('H','o','u','r','S'); 									delay1ms(1000);
		IntTo7seg(FR_SPI2_get32bitWord (NV_MINUTE_CNT)/60);delay1ms(1000);
		fillBuff('C','o','u','n','t'); 									delay1ms(1000);
		IntTo7seg(FR_SPI2_get32bitWord (NV_TOTAL_CNT)); delay1ms(1000);

		if (FR_SPI2_get32bitWord(NV_TOTAL_CNT) >= 99999) {
			FR_SPI2_set32bitWord(NV_TOTAL_CNT, 0);
		}
	
		//start ...
		fillBuff('S','t','A','r','t'); 									delay1ms(2000);
		
		digit_current_dp = 2;

		//ucitaj setpoint-e iz FRAM
		TIC_signal_setpoint = FR_SPI2_get32bitWord (NV_SIGNAL_VAL);
		TIC_pause_setpoint = FR_SPI2_get32bitWord (NV_PAUSE_VAL);
	
	  while (1){
			
			//--------
			//Toggle PC13 LED svakih 500ms, signalizira da je MCU ziv (baza 1ms)
			if (SW_timers[SWT_LED] > 500 / MY_TIM3_TIME_SCALE) {
				SW_timers[SWT_LED] = 0;
				GPIOC->ODR ^= GPIO_ODR_ODR13; // Toggle LED which connected to PC13
				
			
				//ako smo u edit modu (
				if(myKBtime[KBS_edit] > 200) {
					//blinbkaj decimal-point da se zna :)
					if (digit_current_dp == 2) digit_current_dp = 0; else digit_current_dp = 2;
				}	else {
					if (myKBtime[KBS_run] == 0) digit_current_dp = 2;
				}
				
			}

			//--------
			//inkrementuj broj radnih minuta u FRAM svakih 60 sekundi
			if (SW_timers[SWT_1min] > (1000 * 60)) {
				SW_timers[SWT_1min] = 0;
				FR_SPI2_inc32bitWord (NV_MINUTE_CNT);
			}
			
			//--------
			//skeniraj tastaturu svake 1 ms i pozovi komparator
			if (SW_timers[SWT_KBD] > 0) {
				SW_timers[SWT_KBD] = 0;
				
				//procitaj PortA
				kbIN = GPIOA->IDR;
				//inkrementuj vrednost, inkrementuj na svako uzastopno visoko stanje
				if ((kbIN & GPIO_IDR_IDR8) != 0) myKBtime[KB_plus]++; else myKBtime[KB_plus]=myKBflag[KB_plus]=0; //+
				if ((kbIN & GPIO_IDR_IDR9) != 0) myKBtime[KB_minus]++; else myKBtime[KB_minus]=myKBflag[KB_minus]=0;  //-
				if ((kbIN & GPIO_IDR_IDR10) != 0) myKBtime[KBS_run]++; else myKBtime[KBS_run]=myKBflag[KBS_run]=0; //run/stop
				if ((kbIN & GPIO_IDR_IDR11) != 0) myKBtime[KBS_edit]++; else myKBtime[KBS_edit]=myKBflag[KBS_edit]=0; //edit/view
				if ((kbIN & GPIO_IDR_IDR12) != 0) myKBtime[KBS_s_p]++; else myKBtime[KBS_s_p]=myKBflag[KBS_s_p]=0; //signal/pause
				
				//...
				//akcije
				
				//ako smo u RUN i prodje 1sec, pozovi funkciju za hendlovanje izlaza prema vremencima (racunamo na 1ms rezoluciju)
				if (myKBtime[KBS_run] > 1000) {
										
					//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> <<<<<<<<<<<<<<<<<<<<<<<<<<
					//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> <<<<<<<<<<<<<<<<<<<<<<<<<<
					//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> <<<<<<<<<<<<<<<<<<<<<<<<<<
					//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> <<<<<<<<<<<<<<<<<<<<<<<<<<
					komparator();
				}
				
				if (myKBtime[KBS_run] == 0) { 
					//kada smo u STOP, izlaz se drzi nisko
					GPIOB->BSRR = GPIO_BSRR_BR8; //PB8
					TIC_signal_cnt = 0;
					TIC_pause_cnt = 0;
				}

				//cim se stisne/ukljuci RUN
				if (myKBtime[KBS_run] == 10) {		
					//ako je pauza=0 onda overflovuj namerno signal da bi se izbegao prvi impuls, 
					//resetovace ga HW input a ne moze da overflovuje variabla jer je 64bit, mozda za 150 godina :)
					if (TIC_pause_setpoint == 0) {
						TIC_signal_cnt = TIC_signal_setpoint + 1;
						
						digit_current_dp = 5;
					}
				}

			}
			
	
			//ako smo u edit modu (
			if(myKBtime[KBS_edit] > 200) {
								
				previous_edit_flag = myKBtime[KBS_edit];
				
				//logika tastature 
				//+
				if (myKBtime[KB_plus] > 20 && myKBtime[KB_plus] < 1000 && myKBflag[KB_plus] == 0) { //pojedinacan klik
					if (myKBtime[KBS_s_p] > 20) TIC_signal_setpoint++; else TIC_pause_setpoint++;	//sta menjamo, signal ili pauzu
					myKBflag[KB_plus]=1;
				}
				if (SW_timers[SWT_kscan_1] > 20) {
					SW_timers[SWT_kscan_1]=0;
					if (myKBtime[KB_plus] > 1000 && myKBtime[KB_plus] < 7000 && myKBflag[KB_plus] >= 1) {//brzo posle 1 sec
						if (myKBtime[KBS_s_p] > 20) TIC_signal_setpoint++; else TIC_pause_setpoint++;	//sta menjamo, signal ili pauzu
					}
					if (myKBtime[KB_plus] > 7000 && myKBflag[KB_plus] >= 1) {//jos brze posle 5 sec
						if (myKBtime[KBS_s_p] > 20) TIC_signal_setpoint+=10; else TIC_pause_setpoint+=10;	//sta menjamo, signal ili pauzu
					}
				}
					
				//logika tastature 
				//-
				if (myKBtime[KB_minus] > 20 && myKBtime[KB_minus] < 1000 && myKBflag[KB_minus] == 0) { //pojedinacan klik
					if (myKBtime[KBS_s_p] > 20) TIC_signal_setpoint--; else TIC_pause_setpoint--;	//sta menjamo, signal ili pauzu
					myKBflag[KB_minus]=1;
				}
				if (SW_timers[SWT_kscan_2] > 20) {
					SW_timers[SWT_kscan_2]=0;
					if (myKBtime[KB_minus] > 1000 && myKBtime[KB_minus] < 7000 && myKBflag[KB_minus] >= 1) {//brzo posle 1 sec
						if (myKBtime[KBS_s_p] > 20) TIC_signal_setpoint--; else TIC_pause_setpoint--;	//sta menjamo, signal ili pauzu
					}
					if (myKBtime[KB_minus] > 7000 && myKBflag[KB_minus] >= 1) {//jos brze posle 7 sec
						if (myKBtime[KBS_s_p] > 20) TIC_signal_setpoint-=10; else TIC_pause_setpoint-=10;	//sta menjamo, signal ili pauzu
					}
				}
				
				//limitiraj min/max vrednosti (unsigned int!)
				if (TIC_signal_setpoint == 0) TIC_signal_setpoint = 1;
				if (TIC_signal_setpoint > 1000000) TIC_signal_setpoint = 1;
				if (TIC_signal_setpoint > 99999) TIC_signal_setpoint = 99999;
				if (TIC_pause_setpoint > 1000000) TIC_pause_setpoint = 0;
				if (TIC_pause_setpoint > 99999) TIC_pause_setpoint = 99999;
			}
			
			//ivica za SAVE u FRAM
			if (previous_edit_flag > 1000 && myKBtime[KBS_edit] == 0) {
				previous_edit_flag = 0;
				
				//snimi u FRAM sad
				FR_SPI2_set32bitWord (NV_SIGNAL_VAL, TIC_signal_setpoint);
				FR_SPI2_set32bitWord (NV_PAUSE_VAL, TIC_pause_setpoint);
				
				SW_timers[SWT_saved] = BUDUCNOST;
			}
			
			if (SW_timers[SWT_saved] > BUDUCNOST + 1000) {
				//vrati timer u sadasnjost
				SW_timers[SWT_saved]=0;
			}
			
			//##################################
			//ispisivanje na LED displej
			if (SW_timers[SWT_saved] > BUDUCNOST && SW_timers[SWT_saved] < BUDUCNOST + 1000) {
  			//prikazi privremeno SAVE	
				fillBuff('S','A','U','e',' ');
				digit_current_dp = 0;
			} else {
				
				if (myKBtime[KBS_run] > 200) {
					
					//jel edit mode 
					if (myKBtime[KBS_edit] > 200) {
						if (myKBtime[KBS_s_p] > 20) IntTo7seg(TIC_signal_setpoint); else IntTo7seg(TIC_pause_setpoint);
					
					} else {
						//ako smo u run, prikazi broj ciklusa
						//digit_current_dp = 5;
						IntTo7seg(TIC_total_cnt);
					}
					
				} else {
					//ako smo u stop
					//prikaz signala ili pauze, zavisno sta izabrano na prekidacu s_p
					
					
					//kada smo u stop i stisne se + ili - tad prikazi Count na displeju, suprotno je signal/pauza
					if ((myKBtime[KB_plus] > 100 || myKBtime[KB_minus] > 100) && myKBtime[KBS_edit] ==0) {
						IntTo7seg(TIC_total_cnt);
						digit_current_dp = 0;

					} else {
						if (myKBtime[KBS_s_p] > 20) IntTo7seg(TIC_signal_setpoint); else IntTo7seg(TIC_pause_setpoint);
					}
				}
			}
		
			//reset brojaca ciklusa, ako smo u RUN ili u stop bez edita i drzi se taster + i - duze od jedne sekunde
			if (myKBtime[KB_plus] > 1000 && myKBtime[KB_minus] > 1000) {
				//if (myKBtime[KBS_run] > 200) {
					//uradi reset
					TIC_total_cnt = 0;
				//}
			}
		
			//--- INPUT ---
			if ((GPIOB->IDR & GPIO_IDR_IDR9) == (0xffff & GPIO_IDR_IDR9)) {
				TIC_INPUT_time ++;
			} else {
				TIC_INPUT_time = 0;
			}
			
			
			//ovo je ivica od inputa
			if (TIC_INPUT_time > 1000 && TIC_INPUT_time < BUDUCNOST) {//vece od X prolaza kroz main/loop
				TIC_INPUT_time = BUDUCNOST;
				
				//ignorise se reset dok ne prodje vreme (ne-resutujuci vremenac)
					if (TIC_signal_cnt > TIC_signal_setpoint) {

						TIC_signal_cnt = 0;
						TIC_pause_cnt = 0;
						TIC_total_cnt ++;
						FR_SPI2_inc32bitWord(NV_TOTAL_CNT);
					}
			}
			
			//brzina prolaza petlje -O3 ~~~~~~~~~~~~~~~~~~
			//oko 150 bez interupta, worst case 100khz sa interaptom
			//				GPIOC->ODR ^= GPIO_ODR_ODR13; 

			
		}// end while(1)

}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> <<<<<<<<<<<<<<<<<<<<<<<<<<
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> <<<<<<<<<<<<<<<<<<<<<<<<<<
//zove se svakih 1ms u RUN modu ....
void komparator (void) {
	TIC_signal_cnt++;
	TIC_pause_cnt++;

	//kada ima pauza (vrti u krug)
	if (TIC_pause_setpoint > 0) {

		if (TIC_signal_cnt < TIC_signal_setpoint) {
			//out HI - signal
			GPIOB->BSRR = GPIO_BSRR_BS8; //PB8
			if (myKBtime[KBS_edit]==0) digit_current_dp = 5;
		}
		
		if (TIC_signal_cnt > TIC_signal_setpoint && (TIC_pause_cnt) <= (TIC_pause_setpoint+TIC_signal_setpoint)) {
			//out LO - pauza
			GPIOB->BSRR = GPIO_BSRR_BR8; //PB8
			if (myKBtime[KBS_edit]==0) digit_current_dp = 0;
		}

		// *****   reset  ******
		if (TIC_pause_cnt >= (TIC_pause_setpoint+TIC_signal_setpoint)) {
			TIC_signal_cnt = 0;
			TIC_pause_cnt = 0;
			TIC_total_cnt ++;
			FR_SPI2_inc32bitWord(NV_TOTAL_CNT);
		}
	} else {

		//kada ima samo signal a resetuje se na input

		if (TIC_signal_cnt-1 < TIC_signal_setpoint) {
			//out HI - signal
			GPIOB->BSRR = GPIO_BSRR_BS8; //PB8
			if (myKBtime[KBS_edit]==0) digit_current_dp = 5;
		}
		
		if (TIC_signal_cnt-1 == TIC_signal_setpoint) {
			//out LO - pauza
			GPIOB->BSRR = GPIO_BSRR_BR8; //PB8
			if (myKBtime[KBS_edit]==0) digit_current_dp = 0;
		}
		
	}
	

}


