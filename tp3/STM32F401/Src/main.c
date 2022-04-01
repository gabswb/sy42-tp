/**
 * Author : Gabriel SCHWAB
 * Date : 03/14/2022
 */

#include "main.h"
#include "usart.h"
#include "pwm.h"

#include "stdint.h"
#include "stm32f401xe.h"

float compute_duration(uint16_t begin, uint16_t end, uint16_t nb_overflows, int arr, int freq);


/**
  * @brief main function
  * @param  none
  * @retval 0 if success
 */

#ifdef EXERCISE_2
int main(void)
{

	/*=======================================================================
	 *                            EXERCICE 2                                |
	 * ======================================================================
	 *
	 * Attention pour pouvoir manipuler des floats avec printf j'ai du modifier les parametre de l'ide
	 *
	 */


	/**
	 * ------------------CONFIGURATION USART-----------------------------
	 */

	/* Activation de l'horloge GPIOA */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN;

	/*Configuration des pins PA2 et PA3 en "alternate function" AF07*/
	GPIOA->MODER &= ~(0xF << 4);
	GPIOA->MODER |= (0xA << 4);

	GPIOA->AFR[0] &= ~(0xFF << 8);
	GPIOA->AFR[0] |= (0x77 << 8);//Alternate function AF07

	USART2_Init(115200);


	/**
	 * ------------------CONFIGURATION GPIOB-----------------------------
	 */

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


	/**
	 * ------------------CONFIGURATION TIMER 4-----------------------------
	 */

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


	/**
	 * ------------------INPUT CAPTURE MODE---------------------------------
	 */

	/* désactivation input capture mode pour modification*/
	TIM4->CCER &= ~TIM_CCER_CC1E_Msk;
	TIM4->CCER &= ~TIM_CCER_CC2E_Msk;

	/* modification de CCMRR1 */
	//TIM4->CCMR1 &= ~TIM_CCMR1_IC2F_Msk;
	//TIM4->CCMR1 |= TIM_CCMR1_IC2F_3; //valeur du filtre pour les rebonds

	/* configure capture prescaler, on met à 0 car on veut capturer tous les évènements */
	//TIM4->CCMR1 &= ~TIM_CCMR1_IC1PSC_Msk;
	//TIM4->CCMR1 &= ~TIM_CCMR1_IC2PSC_Msk;

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

	/* Lancement du compteur*/
	TIM4->CR1 |= TIM_CR1_CEN_Msk;


	/**
	 * ------------------BOUCLE PRINCIPALE---------------------------------
	 */

	USART2_Print("INPUT CAPTURE MODE\r\n");

	char buffer[256];
	float duration;
	uint16_t begin=0, end=0, nb_overflows=0;

	while(1){

		while((TIM4->SR & TIM_SR_CC2IF_Msk)==0){}//Attente de détéction du font descendant

		begin = TIM4->CCR2;//récupération de la valeur de compteur stockée dans CCR2
		USART2_Print("falling edge\r\n");
		nb_overflows = 0;

		while((TIM4->SR & TIM_SR_CC1IF_Msk)==0){//détéction du font montant
			if(TIM4->SR & TIM_SR_UIF_Msk){//Détéction d'overflows
				nb_overflows++;
				TIM4->SR &= ~TIM_SR_UIF;
			 }
		}
		end = TIM4->CCR1;//récupération de la valeur de compteur stockée dans CCR2
		USART2_Print("rising edge\n");

		sprintf(buffer, "begin = %d, nb_of_overflows = %d, end = %d\r\n", begin, nb_overflows, end);
		USART2_Print(buffer);
		duration = compute_duration(begin, end, nb_overflows, arr, update_freq);
		sprintf(buffer, "duration = %f\r\n", duration);
		USART2_Print(buffer);
	}

	return 0;
}
#endif

#ifdef EXERCISE_3
int main(void)
{

	/*=======================================================================
	 *                            EXERCICE 3                                |
	 * ======================================================================
	 */

	/**
	 * ------------------CONFIGURATION USART-----------------------------
	 */

	/* activation horloge */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

	/* PA5 in alternate function mode */
	GPIOA->MODER &= ~GPIO_MODER_MODER5;
	GPIOA->MODER |= GPIO_MODER_MODER5_1;

	/* Alternate function AF01 */
	GPIOA->AFR[0] |= GPIO_AFRL_AFRL5_0;

	TIM2_init_PWM();


	/**
	 * ------------------CONFIGURATION TIMER 3-----------------------------
	 */

	/* Activation horlgoe du TIM3 */
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

	/* Fréquence d'horloge en sortie (counter_clock) du prescaler à 10kHz */
	int freq_after_psc = 10000;
	TIM3->PSC = (SystemCoreClock / freq_after_psc) -1; // 1599 pour une fréquence cible de 10kHz


	int update_freq = 2;//fréquence de sortie en Hz
	int arr = SystemCoreClock / (update_freq * TIM3->PSC + 1);
	TIM3->ARR = arr; // Granularité de 1000

	/* Configuration du mode PWM 1 : '110'*/
	TIM3->CCMR1 |= TIM_CCMR1_OC1M;
	TIM3->CCMR1 &= ~TIM_CCMR1_OC1M_0;

	/* Activation de la sortie*/
	TIM3->CCER |= TIM_CCER_CC1E;

	/* force le compteur et le prescaler à O avant le démarrage*/
	TIM4->EGR |= TIM_EGR_UG_Msk;

	/* reset les flags */
	TIM4->SR = 0;

	/* Lancement du compteur */
	TIM3->CR1 |= TIM_CR1_CEN;



	/**
	 * ------------------BOUCLE PRINCIPALE---------------------------------
	 */

	int luminosity[] = {0,11,29,62,112,184,280,407,566,762,999};
	int i=0;
	while(1){
		if(TIM3->SR & TIM_SR_UIF_Msk){
			TIM2_set_pwn(luminosity[i]);
			i++;
			if(i>10)
				i=0;
			TIM3->SR &= ~TIM_SR_UIF;
		}
	}

	return 0;

}



float compute_duration(uint16_t begin, uint16_t end, uint16_t nb_overflows, int arr, int freq)
{
	float nb_cnt = end-begin + nb_overflows*arr;//nb of tics by the counter
	float cnt_time =(float) 1/freq;//duration of 1 tic in second

	return cnt_time*nb_cnt;
}
#endif
