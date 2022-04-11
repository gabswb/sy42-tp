/**
 * Author : Gabriel SCHWAB
 * Date : 03/14/2022
 */

#include "main.h"
#include "usart.h"
#include "pwm.h"
#include "timer.h"

#include "stdint.h"
#include "stm32f401xe.h"

void GPIOB_init();
void TIM4_init();
void TIM4_input_capture_config();

/**
  * @brief main function
  * @param  none
  * @retval 0 if success
 */

int main(void)
{

	USART2_Init(115200);
	GPIOB_init();
	TIM4_init();
	TIM4_input_capture_config();


	//TIM3_configuration();
	TIM3_set_periodic_IRQ((uint32_t) 500);
	TIM4_set_periodic_IRQ((uint32_t) 500);
	USART2_set_IRQ();

	__WFI();
	SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;

	while(1){}
	return 0;
}

void GPIOB_init(void){
	/*Activation de l'horloge du GPIOB*/
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

	/* activation alternate fonction pour GPIOB pour la pin PB7*/
	GPIOB->MODER &= ~GPIO_MODER_MODER7_Msk;// mise à '10' des bits du registre MODER7 pour alternate function
	GPIOB->MODER |= GPIO_MODER_MODER7_1;
	GPIOB->AFR[0] &= GPIO_AFRL_AFSEL7_Msk;// dans la data sheet, section alternate fonction mapping p45, on voit que TIM4_CH2 est mappé avec la fonction alternative AF02 sur la pin PB7
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL7_1;

	/* PC13 in input mode, no pull */
	 GPIOC->MODER &= ~GPIO_MODER_MODE13_Msk;
	 GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD13_Msk;
}

void TIM4_init(){
	/* activation de l'horloge */
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

	int freq_after_psc = 10000;// fréquence de 10kHz
	TIM4->PSC = (SystemCoreClock / freq_after_psc) -1; //psc = 1599

	/* Calcul de la valeur désirée pour la durée de comptage */
	int update_freq = 10000; // Fréquence de 10kHz
	int arr = SystemCoreClock / (update_freq * TIM4->PSC + 1);
	TIM4->ARR = arr;

	/* force le compteur et le prescaler à O avant le démarrage*/
	TIM4->EGR |= TIM_EGR_UG_Msk;

	/* reset les flags */
	TIM4->SR = 0;
}

void TIM4_input_capture_config(){
	/* désactivation input capture mode pour modification*/
		TIM4->CCER &= ~TIM_CCER_CC1E_Msk;
		TIM4->CCER &= ~TIM_CCER_CC2E_Msk;

		/* modification de CCMRR1 */
		//TIM4->CCMR1 &= ~TIM_CCMR1_IC2F_Msk;
		//TIM4->CCMR1 |= TIM_CCMR1_IC2F_3; //valeur du filtre pour les rebonds

		/*Mappe IC1 et IC2 sur TI2*/
		TIM4->CCMR1 &= ~TIM_CCMR1_CC1S_Msk;
		TIM4->CCMR1 |= TIM_CCMR1_CC1S_1; //met le registre à '10' pour mapper IC1 à TI2
		TIM4->CCMR1 &= ~TIM_CCMR1_CC2S_Msk;
		TIM4->CCMR1 |= TIM_CCMR1_CC2S_0;//met le registre à '01' pour mapper IC2 à TI2

		/*Configuration de TI2FP1 sur front montant*/
		TIM4->CCER &= ~TIM_CCER_CC1P_Msk;//met le registre à '00' pour front montant
		TIM4->CCER &= ~TIM_CCER_CC1NP_Msk;

		/*Configuration de TI2FP1 sur front descendant*/
		TIM4->CCER &= TIM_CCER_CC2NP_Msk;//met les bits à '01' pour configurer TI2FP1 en front descendant
		TIM4->CCER |= TIM_CCER_CC2P_Msk;


		/* résactivation input capture mode */
		TIM4->CCER |= TIM_CCER_CC1E_Msk;
		TIM4->CCER |= TIM_CCER_CC2E_Msk;

		/*lance le compteur*/
		TIM4->CR1 |= TIM_CR1_CEN ;
}
