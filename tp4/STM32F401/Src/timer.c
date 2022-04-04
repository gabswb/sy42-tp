/*
 * timer.c
 *
 *  Created on: 4 avr. 2022
 *      Author: gschwab
 */


#include "timer.h"


void TIM3_set_periodic_IRQ(uint32_t ms){

	/* définit piorité de prise en compte de l'interruption au niveau du NVIC*/
	NVIC_SetPriority(TIM3_IRQn,10);
	/* autorise les interruptions au niveau du NVIC*/
	NVIC_EnableIRQ(TIM3_IRQn);
	/*autorise IRQ sur le flag UIF*/
	TIM3->DIER |= TIM_DIER_UIE;


	TIM4->CR1 |= TIM_CR1_CEN ;


}
void TIM3_IRQHandler(void){
//	/* teste si le drapeau relatif à l'interruption est à '1'et test si l'interruption relative à ce drapeau est autorisée*/
 	if((TIM3->SR & TIM_SR_UIF) && (TIM3->DIER & TIM_DIER_UIE)){
		TIM2_set_PWM((uint32_t) 555);
 	}
 	/*remise à 0 du drapeau d'état*/
 	TIM3->SR &= ~TIM_SR_UIF;
}

void TIM4_set_periodic_IRQ(uint32_t ms){

	/* définit piorité de prise en compte de l'interruption au niveau du NVIC*/
	NVIC_SetPriority(TIM4_IRQn,10);
	/* autorise les interruptions au niveau du NVIC*/
	NVIC_EnableIRQ(TIM4_IRQn);

	/*autorise IRQ sur le flag CC2IF*/
	TIM4->DIER |= TIM_DIER_CC2IE;
	/*autorise IRQ sur le flag CC2IF*/
	TIM4->DIER |= TIM_DIER_CC1IE;
	/*autorise IRQ sur le flag UIF*/
	TIM4->DIER |= TIM_DIER_UIE;


	TIM4->CR1 |= TIM_CR1_CEN ;


}
void TIM4_IRQHandler(void){
	/* pour detecter font descendant
	 * teste si le drapeau relatif à l'interruption est à '1' et  teste si l'interruption relative à ce drapeau est autorisée*/
 	if((TIM4->SR & TIM_SR_CC2IF) && (TIM4->DIER & TIM_DIER_CC2IE)){
		TIM2_set_PWM((uint32_t) 555);//pas demander aaaaaaaaaaaaaaaaaaaaa

		USART2_Print("falling edge\r\n");
		nb_overflows = 0;

		/*remise à 0 du drapeau d'état*/
		TIM3->SR &= ~TIM_SR_CC2IF;

 	}

 	if((TIM4->SR & TIM_SR_UIF) && (TIM4->DIER & TIM_DIER_UIE)){
 		nb_overflows++;

		/*remise à 0 du drapeau d'état*/
 		TIM4->SR &= ~TIM_SR_UIF;
 	}


 	/* pour detecter font montant
 	* teste si le drapeau relatif à l'interruption est à '1' et  teste si l'interruption relative à ce drapeau est autorisée*/
 	if((TIM4->SR & TIM_SR_CC1IF_Msk) && TIM4->DIER & TIM_DIER_CC1IE){
		TIM2_set_PWM((uint32_t) 0);//pas demander aaaaaaaaaaaaaaaaaaaaa

		end = TIM4->CCR1;//récupération de la valeur de compteur stockée dans CCR2
		USART2_Print("rising edge\n");

		sprintf(buffer, "begin = %d, nb_of_overflows = %d, end = %d\r\n", begin, nb_overflows, end);
		USART2_Print(buffer);
		//la fréquence d'update = SystemCoreClock/(TIM4->ARR * (TIM4->PSC+1))
		duration = compute_duration(begin, end, nb_overflows, TIM4->ARR,(SystemCoreClock/(TIM4->ARR * (TIM4->PSC+1))) );
		sprintf(buffer, "duration = %f\r\n", duration);
		USART2_Print(buffer);

		/*remise à 0 du drapeau d'état*/
		TIM4->SR &= ~TIM_SR_CC1IF_Msk;
 	}

}
