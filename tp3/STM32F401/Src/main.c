/**
 ******************************************************************************
 * @file           : main.c
 * @author         : NL
 * @brief          : Main program body,
 * 					 simple application to test and debug
 ******************************************************************************
 */
#include "main.h"
#include "stm32f401xe.h"
#include "usart.h"
#include "stdint.h"

/**
  * @brief Copy nr 32bits data from one location to another
  * @param  source the source address of the data to copy
  * @param  destination the destination address of the data to copy
  * @param  nr number of 32bits data to copy
  * @retval number of data left to be copied
 */
void pin_init(void);
void TIM2_set_pwn(uint32_t duty);
float compute_duration(uint16_t begin, uint16_t end, uint16_t nb_overflows, int arr, int freq);


/**
  * @brief main function
  * @param  none
  * @retval 0 if success
 */

int main(void)
{


//
//		/* Activer horloger GPIOA */
//
//		/* PA2(TX) en alternate function nÂ°7 */
//		PB->AFR[0] &= ~GPIO_AFRL_AFRL7;
//		PB->AFR[0] |= GPIO_AFRL_AFRL7_1;
//		/* alternate function PA7*/
//		PB->MODER &= ~GPIO_MODER_MODER7;
//		PB->MODER |= GPIO_MODER_MODER7_1;
//
//
//
//		int is_activate = 0;
//
//		RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
//
//		//CK_CNT * PSC = CK_PSC => PSC= CK_PSC / CK_CNT = 16 000/10
//		TIM3->PSC = SystemCoreClock /10000 -1;
//		//auto reload = CK_PSC / f_led = 10000/2 - 1
//		TIM3->ARR = 10000/10/2 - 1;
//		//set EGR register bit to 1 to force the update and start the count at 0
//		TIM3->EGR &= ~TIM_EGR_UG;
//		TIM3->EGR |= TIM_EGR_UG_Msk;
//		//set bit OGR from CR1 register of TIM3 to 1 for 'one shot' behaviour
//		//TIM3->CCR1 |= TIM_CR1_OPM_Msk;
//		//errase status register (p347 'reset value = 0x0000')
//		TIM3->SR = 0;
//		//start timer
//		TIM3->CR1 |= TIM_CR1_CEN_Msk;


//----------------------------------------------------------------------partie 1 clignotement led---------------------------------------------
//	for(;;){
//
//		/** clignotage de led**/
//		if((TIM3->SR & TIM_SR_UIF_Msk)){
//
//			if(is_activate){
//				/* éteint la led */
//				PA->ODR &= ~GPIO_ODR_OD5;
//				is_activate =0;
//			}
//			else{
//				/* allume la led */
//				PA->ODR |= GPIO_ODR_OD5;	//(1<<5);
//				is_activate = 1;
//			}
//
//			tim3->SR &= ~TIM_SR_UIF_Msk;
//		}
//	}
		//----------------------------------------------------------------------partie 2 bouton timer---------------------------------------------


//	 /*
//	     * -------------------------------------------------------------------------------
//	     * CONFIGURATION DE L'USART
//	     * -------------------------------------------------------------------------------
//	     */
//	    /* Activation de l'horloge GPIOA */
//	    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN;
//
//	    /*
//	     * Configuration des pins PA2 et PA3 en "alternate function" N7
//	     */
//	    GPIO_TypeDef *PA;
//	    PA = GPIOA;
//	    // Moder
//	    PA->MODER &= ~(0xF << 4);
//	    PA->MODER |= (0xA << 4);
//
//	    // Alternate function 7 (0b0111)
//	    PA->AFR[0] &= ~(0xFF << 8);
//	    PA->AFR[0] |= (0x77 << 8);
//
//	    USART2_Init(115200);
//
//	    /**
//	     * CONFIGURATION GPIOB
//	     */
//
//	    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
//	    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
//
//		/* activation alternate fonction pour GPIOB pour la pin PB7*/
//		GPIOB->MODER &= ~GPIO_MODER_MODER7_Msk;// mise à '10' des bits du registre moder7 pour alternate fonction
//		GPIOB->MODER |= GPIO_MODER_MODER7_1;
//		GPIOB->AFR[0] &= GPIO_AFRL_AFSEL7_Msk;// dans la data sheet, section alternate fonction mapping p44, on voit que TIM4_CH2 est mappé avec la fonction alternative AF02 sur la pin PA1
//		GPIOB->AFR[0] |= GPIO_AFRL_AFSEL7_1;
//
//		/* PC13 in input mode, no pull */
//		  GPIOC->MODER &= ~GPIO_MODER_MODE13_Msk;
//		  GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD13_Msk;
//
//
//		/**
//		 * TIMER TIM4
//		 */
//
//	    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
//
//	    int psc_target_frequency = 10000;
//		TIM4->PSC = (SystemCoreClock / psc_target_frequency) -1; // 1599 pour une fréquence cible de 10kHz
//
//		/* Calcul de la valeur désirée pour la durée de comptage */
//		int update_freq = 10000; // Fréquence cible (en Hz)
//		int arr = SystemCoreClock / (update_freq * TIM4->PSC + 1);
//		TIM4->ARR = arr;
//
//		TIM4->EGR |= TIM_EGR_UG_Msk;
//
//		TIM4->SR = 0;
//
//
//		/* désactivation input capture mode */
//		TIM4->CCER &= ~TIM_CCER_CC1E_Msk;
//		TIM4->CCER &= ~TIM_CCER_CC2E_Msk;
//
//		/* modification de CCMRR1 */
//		//TIM4->CCMR1 &= ~TIM_CCMR1_IC2F_Msk;
//		//TIM4->CCMR1 |= TIM_CCMR1_IC2F_3; //valeur du filtre pour les rebonds
//
//		/* configure capture prescaler, on met à 0 car on veut capturer tous les évènements */
//		//TIM4->CCMR1 &= ~TIM_CCMR1_IC1PSC_Msk;
//		//TIM4->CCMR1 &= ~TIM_CCMR1_IC2PSC_Msk;
//
//		/**/
//		TIM4->CCMR1 &= ~TIM_CCMR1_CC1S_Msk;
//		TIM4->CCMR1 |= TIM_CCMR1_CC1S_1; //met le registre à '10' pour mapper IC1 à TI2
//
//		TIM4->CCMR1 &= ~TIM_CCMR1_CC2S_Msk;
//		TIM4->CCMR1 |= TIM_CCMR1_CC2S_0;
//
//		/**/
//		TIM4->CCER &= ~TIM_CCER_CC1P_Msk;  //met le registre à 'OO' pour front montant
//		TIM4->CCER &= ~TIM_CCER_CC1NP_Msk;
//
//		TIM4->CCER &= TIM_CCER_CC2NP_Msk;//met les bits à '01' pour configurer TI2FP1 en front descendant
//		TIM4->CCER |= TIM_CCER_CC2P_Msk;
//
//
//
//
//
//
//		/* résactivation input capture mode */
//		TIM4->CCER |= TIM_CCER_CC1E_Msk;
//		TIM4->CCER |= TIM_CCER_CC2E_Msk;
//
//		TIM4->CR1 |= TIM_CR1_CEN_Msk;


		/**
		 * Programme principal
		 */
//
//	    USART2_Print("Input/Capture Mode configuré\r\n");
//
//
//		char buffer[256];
//		float duration;
//		uint16_t begin=0, end=0, nb_overflows=0;
//		while(1){
//			while((TIM4->SR & TIM_SR_CC2IF_Msk)==0){}
//			begin = TIM4->CCR2;
//			USART2_Print("falling edge\r\n");
//			nb_overflows = 0;
//
//			while((TIM4->SR & TIM_SR_CC1IF_Msk)==0){
//				if(TIM4->SR & TIM_SR_UIF_Msk){
//					nb_overflows++;
//	                TIM4->SR &= ~TIM_SR_UIF;
//				 }
//			}
//
//			end = TIM4->CCR1;
//			USART2_Print("rising edge\n");
//			sprintf(buffer, "beg = %d, count = %d, end = %d\r\n", begin, nb_overflows, end);
//			USART2_Print(buffer);
//			duration = compute_duration(begin, end, nb_overflows, arr, update_freq);
//			sprintf(buffer, "result = %f\r\n", duration);
//			USART2_Print(buffer);
//			}




		//--------------------------------------------------------------------Partie 3

	/* 2 port instance are declared and mapped on GPIOA and GPIOC address
	 * GPIOA and GPIOC are constant address defined in stm32f401xe.h
	 */
	GPIO_TypeDef * PA = GPIOA;

	//modify bit 0 and 3 of RCC AHB1ENR register to enable the clock
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN;

	/* GPIOA configuration :  output, slow speed, push-pull, no pull
	 * After reset, only the output mode should be set. the other configuration
	 * are correct.
	 */
	/* PA5 in output mode */
	PA->MODER &= ~GPIO_MODER_MODER5_Msk;
	PA->MODER |= GPIO_MODER_MODER5_1;
//	/* output push pull */
//	PA->OTYPER &= ~GPIO_OTYPER_OT5_Msk;			// clear bit field
//	/* slow speed, better for low electrical interference (less noisy) */
//	PA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED5_Msk;	// clear bit field
//	/* no pull-up or pull-down resistor */
//	PA->PUPDR &= ~GPIO_PUPDR_PUPD5_Msk;			// clear bit field
	/* Alternate function AF01 */
	PA->AFR[0] |= GPIO_AFRL_AFRL5_1;

	TIM2_init_pwm();

	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

	/* Fréquence d'horloge en sortie (counter_clock) du prescaler à 10kHz */
	    int psc_target_frequency = 10000;
	    TIM3->PSC = (SystemCoreClock / psc_target_frequency) -1; // 1599 pour une fréquence cible de 10kHz

	    /*
	     * Calcul de la valeur désirée pour la période de clignotement
	     * Une valeur plus élevée signifie que la granularité sera plus grande
	     * (plus grand nombre de valeurs possibles)
	     */
	    int update_freq = 2; // Fréquence cible (en Hz)
	    int arr = SystemCoreClock / (update_freq * TIM3->PSC + 1);
	    TIM3->ARR = arr; // Granularité de 1000 valeurs possibles

	    /*
		 * Configuration du PWM (mode 1 : 110)
		 * DOC : In upcounting, channel 1 is active as long as TIMx_CNT<TIMx_CCR1 else inactive
		 */
		TIM3->CCMR1 |= TIM_CCMR1_OC1M;
		TIM3->CCMR1 &= ~TIM_CCMR1_OC1M_0;

		TIM3->CCMR1 &= ~TIM_CCMR1_OC1PE_Msk;/////test pour le preload aaaaaaaaaaaaaaaaaaaaaaaaa

		/*
		     * Activation de la sortie
		     */
		    TIM3->CCER |= TIM_CCER_CC1E;

	//set EGR register bit to 1 to force the update and start the count at 0
	TIM3->EGR &= ~TIM_EGR_UG;
	TIM3->EGR |= TIM_EGR_UG_Msk;
	//set bit OGR from CR1 register of TIM3 to 1 for 'one shot' behaviour
	//TIM3->CCR1 |= TIM_CR1_OPM_Msk;
	//errase status register (p347 'reset value = 0x0000')
	TIM3->SR = 0;
	//start timer
	TIM3->CR1 |= TIM_CR1_CEN_Msk;


	int luminosity[] = {0,11,29,62,112,184,280,407,566,762,999};
	int i=0;
	while(1){
		if(TIM3->SR & TIM_SR_UIF_Msk){
			TIM2_set_pwn(luminosity[i]);
			i++;
		}
		if(i>10) i=0;
	}







		return 0;

}




void input_capture_mode()
{
	/* activation alternate fonction pour GPIOB pour la pin PB7*/
	GPIOB->MODER &= ~GPIO_MODER_MODER7_Msk;// mise à '10' des bits du registre moder7 pour alternate fonction
	GPIOB->MODER |= GPIO_MODER_MODER7_1;
	GPIOB->AFR[0] &= (0xF<<1*4);// dans la data sheet, section alternate fonction mapping p44, on voit que TIM4_CH2 est mappé avec la fonction alternative AF02 sur la pin PA1
	GPIOB->AFR[0] |= (2<<1*4);

	/* désactivation input caputre mode */
	TIM4->CCER &= ~TIM_CCER_CC1E_Msk;
	TIM4->CCER &= ~TIM_CCER_CC2E_Msk;

	/* modification de CCMRR1 */
	TIM4->CCMR1;
}






void TIM2_init_pwm(void){

	/* horloge du TIM2 */
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;


	//On veut une granularité de 1000 c'est à dire ARR = 1000, et une fréquence de sortie de 100Hz, donc f en sortie de prescaler = 1000*100 = 10e5
	//donc on règle le prescaler à 160


	//CK_CNT * PSC = CK_PSC => PSC= CK_PSC / CK_CNT = 16 000/10
	TIM2->PSC = SystemCoreClock /100000 -1;
	//auto reload = CK_PSC / f_led = 10000/2 - 1
	TIM2->ARR = 1000 - 1;

	//mise en output compare mode
	TIM2->CCMR1 &= ~TIM_CCMR1_OC1M_Msk;//PWM mode 1 : set OC1M bits from register CCMR1 to '110' (p351 ref manual)
	TIM2->CCMR1 |= TIM_CCMR1_OC1M_1;
	TIM2->CCMR1 |= TIM_CCMR1_OC1M_2;

	TIM2->CCER |= TIM_CCER_CC1E_Msk; //set CC1E bits from CCER register to enable output
	/// il y a des trucs à rajouter pour ccer, voir doc


	//set EGR register bit to 1 to force the update and start the count at 0
	TIM2->EGR &= ~TIM_EGR_UG;
	TIM2->EGR |= TIM_EGR_UG_Msk;
	TIM2->SR = 0;


	TIM2->CCMR1 &= ~TIM_CCMR1_OC1PE_Msk;/////test pour le preload aaaaaaaaaaaaaaaaaaaaaaaaa


	//TIM2->CCR1 = 10;//jsplus pk

	//start timer
	TIM2->CR1 |= TIM_CR1_CEN_Msk;
}


void TIM2_set_pwn(uint32_t duty)
{
	TIM2->CCR1 = (duty*TIM2->ARR)/1000;
}

float compute_duration(uint16_t begin, uint16_t end, uint16_t nb_overflows, int arr, int freq)
{
	float nb_cnt = end-begin + nb_overflows*arr;//nb of tics by the counter
	float cnt_time =(float) 1/freq;//duration of 1 tic in second

	return cnt_time*nb_cnt;
}


