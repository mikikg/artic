#include "7seg.h"


char My7segBUFF[6] = {'t', 'E', 'S', 't' , ' ' , 'u'};
int cnt = 0;
int digit_current_pos = 0; //0=prva levo,1=sredina 2=desno
int digit_current_dp = 0; //DP 0=nema, 1=prva levo,2=sredina 3=desno
int tmp_C0, tmp_C1, tmp_C2, tmp_C3, tmp_C4, tmp_C5;


void seg7InitPins(void) {

	/*
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
	
	//konfiguracija PB7, general purpose Output push-pull, 50MHz
  GPIOB->CRL &= ~GPIO_CRL_CNF7_0; //ocisti bit
  GPIOB->CRL &= ~GPIO_CRL_CNF7_1; //ocisti bit
  GPIOB->CRL |=  GPIO_CRL_MODE7_0; //setuj bit
  GPIOB->CRL |=  GPIO_CRL_MODE7_1; //setuj bit
	//konfiguracija PB6, general purpose Output push-pull, 50MHz
  GPIOB->CRL &= ~GPIO_CRL_CNF6_0; //ocisti bit
  GPIOB->CRL &= ~GPIO_CRL_CNF6_1; //ocisti bit
  GPIOB->CRL |=  GPIO_CRL_MODE6_0; //setuj bit
  GPIOB->CRL |=  GPIO_CRL_MODE6_1; //setuj bit
	//konfiguracija PB5, general purpose Output push-pull, 50MHz
  GPIOB->CRL &= ~GPIO_CRL_CNF5_0; //ocisti bit
  GPIOB->CRL &= ~GPIO_CRL_CNF5_1; //ocisti bit
  GPIOB->CRL |=  GPIO_CRL_MODE5_0; //setuj bit
  GPIOB->CRL |=  GPIO_CRL_MODE5_1; //setuj bit
	//konfiguracija PB4, general purpose Output push-pull, 50MHz
  GPIOB->CRL &= ~GPIO_CRL_CNF4_0; //ocisti bit
  GPIOB->CRL &= ~GPIO_CRL_CNF4_1; //ocisti bit
  GPIOB->CRL |=  GPIO_CRL_MODE4_0; //setuj bit
  GPIOB->CRL |=  GPIO_CRL_MODE4_1; //setuj bit
	//konfiguracija PB3, general purpose Output push-pull, 50MHz
  GPIOB->CRL &= ~GPIO_CRL_CNF3_0; //ocisti bit
  GPIOB->CRL &= ~GPIO_CRL_CNF3_1; //ocisti bit
  GPIOB->CRL |=  GPIO_CRL_MODE3_0; //setuj bit
  GPIOB->CRL |=  GPIO_CRL_MODE3_1; //setuj bit
	//konfiguracija PA6, general purpose Output push-pull, 50MHz
  GPIOA->CRL &= ~GPIO_CRL_CNF6_0; //ocisti bit
  GPIOA->CRL &= ~GPIO_CRL_CNF6_1; //ocisti bit
  GPIOA->CRL |=  GPIO_CRL_MODE6_0; //setuj bit
  GPIOA->CRL |=  GPIO_CRL_MODE6_1; //setuj bit
	//konfiguracija PA3, general purpose Output push-pull, 50MHz
  GPIOA->CRL &= ~GPIO_CRL_CNF3_0; //ocisti bit
  GPIOA->CRL &= ~GPIO_CRL_CNF3_1; //ocisti bit
  GPIOA->CRL |=  GPIO_CRL_MODE3_0; //setuj bit
  GPIOA->CRL |=  GPIO_CRL_MODE3_1; //setuj bit
	//konfiguracija PA2, general purpose Output push-pull, 50MHz
  GPIOA->CRL &= ~GPIO_CRL_CNF2_0; //ocisti bit
  GPIOA->CRL &= ~GPIO_CRL_CNF2_1; //ocisti bit
  GPIOA->CRL |=  GPIO_CRL_MODE2_0; //setuj bit
  GPIOA->CRL |=  GPIO_CRL_MODE2_1; //setuj bit
	
	//konfiguracija PA15, general purpose Output push-pull, 50MHz
  GPIOA->CRH &= ~GPIO_CRH_CNF15_0; //ocisti bit
  GPIOA->CRH	&= ~GPIO_CRH_CNF15_1; //ocisti bit
  GPIOA->CRH |=  GPIO_CRH_MODE15_0; //setuj bit
  GPIOA->CRH |=  GPIO_CRH_MODE15_1; //setuj bit		
	//konfiguracija PA7, general purpose Output push-pull, 50MHz
  GPIOA->CRL &= ~GPIO_CRL_CNF7_0; //ocisti bit
  GPIOA->CRL &= ~GPIO_CRL_CNF7_1; //ocisti bit
  GPIOA->CRL |=  GPIO_CRL_MODE7_0; //setuj bit
  GPIOA->CRL |=  GPIO_CRL_MODE7_1; //setuj bit		
	//konfiguracija PA5, general purpose Output push-pull, 50MHz
  GPIOA->CRL &= ~GPIO_CRL_CNF5_0; //ocisti bit
  GPIOA->CRL &= ~GPIO_CRL_CNF5_1; //ocisti bit
  GPIOA->CRL |=  GPIO_CRL_MODE5_0; //setuj bit
  GPIOA->CRL |=  GPIO_CRL_MODE5_1; //setuj bit		
	//konfiguracija PA4, general purpose Output push-pull, 50MHz
  GPIOA->CRL &= ~GPIO_CRL_CNF4_0; //ocisti bit
  GPIOA->CRL &= ~GPIO_CRL_CNF4_1; //ocisti bit
  GPIOA->CRL |=  GPIO_CRL_MODE4_0; //setuj bit
  GPIOA->CRL |=  GPIO_CRL_MODE4_1; //setuj bit		
	//konfiguracija PA1, general purpose Output push-pull, 50MHz
  GPIOA->CRL &= ~GPIO_CRL_CNF1_0; //ocisti bit
  GPIOA->CRL &= ~GPIO_CRL_CNF1_1; //ocisti bit
  GPIOA->CRL |=  GPIO_CRL_MODE1_0; //setuj bit
  GPIOA->CRL |=  GPIO_CRL_MODE1_1; //setuj bit	
	
	dig1_off;
	dig2_on;
	dig3_off;
	dig4_off;
	dig5_off;
	DP_1;
	A_1;
	B_1;
	C_1;
	D_1;
	E_1;
	F_1;
	G_1;
}

void IntTo7seg(int num) { //num=0-99999
  //1, 2, 3,....
  tmp_C0 = num / 10000;
  tmp_C1 = (num - tmp_C0 * 10000) / 1000;
  tmp_C2 = (num - tmp_C0 * 10000 - tmp_C1 * 1000) / 100;
  tmp_C3 = (num - tmp_C0 * 10000 - tmp_C1 * 1000 - tmp_C2 * 100) / 10;
  //tmp_C4 = (num - tmp_C0 * 100000 - tmp_C1 * 10000 - tmp_C2 * 1000 - tmp_C3 * 100) / 10;
  tmp_C4 = (num - tmp_C0 * 10000 - tmp_C1 * 1000 - tmp_C2 * 100 - tmp_C3 * 10);

  //tmp_C5 = num - tmp_C0 * 100000 - tmp_C1 * 10000 - tmp_C2 * 1000 - tmp_C3 * 100 - tmp_C4 * 10;

  if (num < 159999) {
    My7segBUFF[0] = tmp_C0;// if (tmp_C0 == 0) My7segBUFF[0] = ' ';
    My7segBUFF[1] = tmp_C1;
    My7segBUFF[2] = tmp_C2;
    My7segBUFF[3] = tmp_C3;
    My7segBUFF[4] = tmp_C4;
    //My7segBUFF[5] = tmp_C5;
    //digit_current_dp = 2;
  } else {
    My7segBUFF[0] = 'H';
    My7segBUFF[1] = 'i';
    My7segBUFF[2] = 'E';
    My7segBUFF[3] = 'r';
    My7segBUFF[4] = 'r';
    digit_current_dp = 0;
  }
}


//----------------------------------------------------
//Interupt from TIM2, handle 7-seg LED
//----------------------------------------------------
void handler_led() {
  //gpio_write_bit(GPIOC, 13, LOW);

  //all digits off
  dig1_off;
  dig2_off;
  dig3_off;
  dig4_off;
  dig5_off;
  DP_0;

  //Set7seg (My7segBUFF[digit_current_pos]);

  switch (My7segBUFF[digit_current_pos]) {
    case 0: A_1; B_1; C_1; D_1; E_1; F_1; G_0; break;
    case 1: A_0; B_1; C_1; D_0; E_0; F_0; G_0; break;
    case 2: A_1; B_1; C_0; D_1; E_1; F_0; G_1; break;
    case 3: A_1; B_1; C_1; D_1; E_0; F_0; G_1; break;
    case 4: A_0; B_1; C_1; D_0; E_0; F_1; G_1; break;
    case 5: A_1; B_0; C_1; D_1; E_0; F_1; G_1; break;
    case 6: A_1; B_0; C_1; D_1; E_1; F_1; G_1; break;
    case 7: A_1; B_1; C_1; D_0; E_0; F_0; G_0; break;
    case 8: A_1; B_1; C_1; D_1; E_1; F_1; G_1; break;
    case 9: A_1; B_1; C_1; D_1; E_0; F_1; G_1; break;
    case 0xA: case 'A': case 'a': A_1; B_1; C_1; D_0; E_1; F_1; G_1; break;
    case 0xB: case 'b': case 'B': A_0; B_0; C_1; D_1; E_1; F_1; G_1; break;
    case 0xC: case 'c': case 'C': A_1; B_0; C_0; D_1; E_1; F_1; G_0; break;
    case 0xD: case 'D': case 'd': A_0; B_1; C_1; D_1; E_1; F_0; G_1; break;
    case 0xE: case 'e': case 'E': A_1; B_0; C_0; D_1; E_1; F_1; G_1; break;
    case 0xF: case 'f': case 'F': A_1; B_0; C_0; D_0; E_1; F_1; G_1; break;
    case 'H': A_0; B_1; C_1; D_0; E_1; F_1; G_1; break;
    case 'L': A_0; B_0; C_0; D_1; E_1; F_1; G_0; break;
    case 'r': A_0; B_0; C_0; D_0; E_1; F_0; G_1; break;
    case '-': A_0; B_0; C_0; D_0; E_0; F_0; G_1; break;
    case 't': A_0; B_0; C_0; D_1; E_1; F_1; G_1; break;
    case 'S': A_1; B_0; C_1; D_1; E_0; F_1; G_1; break;
    case 'P': A_1; B_1; C_0; D_0; E_1; F_1; G_1; break;
    case ' ': A_0; B_0; C_0; D_0; E_0; F_0; G_0; break;
    case 'o': A_0; B_0; C_1; D_1; E_1; F_0; G_1; break;
    case '=': A_0; B_0; C_0; D_1; E_0; F_0; G_1; break;
    case 'n': A_0; B_0; C_1; D_0; E_1; F_0; G_1; break;
    case 'i': A_0; B_0; C_0; D_0; E_1; F_0; G_0; break;
    case 'I': A_0; B_0; C_0; D_0; E_1; F_1; G_0; break;
    case 'J': A_0; B_1; C_1; D_1; E_0; F_0; G_0; break;
    case 'u': A_0; B_0; C_1; D_1; E_1; F_0; G_0; break;
    case 'U': A_0; B_1; C_1; D_1; E_1; F_1; G_0; break;
  }

	//specialni hak da pratimo stanje input-a kao prva decimalna tacka :)
	if ((GPIOB->IDR & GPIO_IDR_IDR9) == (0xffff & GPIO_IDR_IDR9)) {
		if (digit_current_pos == 0) {
			DP_1;
		}
	}
	
  if (digit_current_pos == 0) {
    if (digit_current_dp == 1) {
      DP_1;
    }
    dig1_on;
  }
  if (digit_current_pos == 1) {
    if (digit_current_dp == 2) {
      DP_1;
      //if (cnt % 2 == 0) {DP_1;} else {
      //  DP_0;}
    }
    dig2_on;
  }
  if (digit_current_pos == 2) {
    if (digit_current_dp == 3) {
      DP_1;
    }
    dig3_on;
  }
  if (digit_current_pos == 3) {
    if (digit_current_dp == 4) {
      DP_1;
    }
    dig4_on;
  }
  if (digit_current_pos == 4) {
    if (digit_current_dp == 5) {
      DP_1;
    }
    dig5_on;
  }

  if (digit_current_pos++ >= 4) digit_current_pos = 0;

  /*
    //brigtness
    //small delay
    for (int xx = 0; xx < 400; xx++) {
      gpio_write_bit(GPIOC, 13, HIGH);
    }
    //all digits off
    //dig1_off;
    //dig2_off;
    //dig3_off;
    //dig4_off;
    dig5_off;
    //dig6_off;
  */
  //gpio_write_bit(GPIOC, 13, HIGH);
  //dig5_off;
  //dig6_off;

}


