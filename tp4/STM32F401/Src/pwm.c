/*
 * pmw.c
 *
 *  Created on: Mar 31, 2022
 *      Author: gabriels
 */


#include "pwm.h"
#include "stm32f401xe.h"

void TIM2_init_PWM(void) {

    /* Activation de TIM2 */
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    /* Fréquence d'horloge en sortie (counter_clock) du prescaler à 10kHz */
    int freq_after_psc = 10000;
    TIM2->PSC = (SystemCoreClock / freq_after_psc) -1; // psc = 1599

    /*
	 *Configuration de la fréquence de clignotement en prenant en compte la granularité
     */
    int update_freq = 100;
    int arr = SystemCoreClock / (update_freq * TIM2->PSC + 1);
    TIM2->ARR = arr; // Granularité de 1000


    /* Configuration du mode PWM 1 : '110'*/
    TIM2->CCMR1 |= TIM_CCMR1_OC1M;
    TIM2->CCMR1 &= ~TIM_CCMR1_OC1M_0;

    /*
     * Activation de l'output
     */
    TIM2->CCER |= TIM_CCER_CC1E;

	/* force le compteur et le prescaler à O avant le démarrage*/
	TIM4->EGR |= TIM_EGR_UG_Msk;

	/* reset les flags */
	TIM4->SR = 0;


    /* Lancement du compteur */
    TIM2->CR1 |= TIM_CR1_CEN;
}

void TIM2_set_PWM(uint32_t duty) {
    TIM2->CCR1 = (duty * TIM2->ARR) / 1000;
}




