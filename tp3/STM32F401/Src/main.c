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
char *mystring = "1234569802";
int src[20] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
int dest[20];
volatile int dummy = 20;
volatile int gnr=5 ;

/**
  * @brief Copy nr 32bits data from one location to another
  * @param  source the source address of the data to copy
  * @param  destination the destination address of the data to copy
  * @param  nr number of 32bits data to copy
  * @retval number of data left to be copied
 */
static void pin_init(void);

void gpio_init(void);
void TIM3_init(void);


/**
  * @brief main function
  * @param  none
  * @retval 0 if success
 */

int main(void)
{

//----------------------------------------------------------------------partie 1 clignotement led---------------------------------------------
//	int is_activate = 0;
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



		/* activation alternate fonction pour GPIOB pour la pin PB7*/
		GPIOB->MODER &= ~GPIO_MODER_MODER7_Msk;// mise à '10' des bits du registre moder7 pour alternate fonction
		GPIOB->MODER |= GPIO_MODER_MODER7_1;
		GPIOB->AFR[0] &= GPIO_AFRL_AFSEL7_Msk;// dans la data sheet, section alternate fonction mapping p44, on voit que TIM4_CH2 est mappé avec la fonction alternative AF02 sur la pin PA1
		GPIOB->AFR[0] |= GPIO_AFRL_AFSEL7_1;

		/* désactivation input capture mode */
		TIM4->CCER &= ~TIM_CCER_CC1E_Msk;
		TIM4->CCER &= ~TIM_CCER_CC2E_Msk;

		/* modification de CCMRR1 */
		TIM4->CCMR1 &= ~TIM_CCMR1_IC2F_Msk;
		TIM4->CCMR1 |= TIM_CCMR1_IC2F_3; //valeur du filtre pour les rebonds

		/* configure capture prescaler, on met à 0 car on veut capturer tous les évènements */
		TIM4->CCMR1 &= ~TIM_CCMR1_IC1PSC_Msk;
		TIM4->CCMR1 &= ~TIM_CCMR1_IC2PSC_Msk;

		/**/
		TIM4->CCMR1 &= ~TIM_CCMR1_CC1S_Msk;
		TIM4->CCMR1 |= TIM_CCMR1_CC1S_1; //met le registre à '10' pour mapper IC1 à TI2

		/**/
		//TIM4->CCER;


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




		return 0;
}

void TIM32_init_pwm(void){

	/* horloge du TIM2 */
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	PB->AFR[0] &= ~GPIO_AFRL_AFSEL5_Msk;
	PB->AFR[0] |= GPIO_AFRL_AFRL5_1;
	/* alternate function PA7*/
	PB->MODER &= ~GPIO_MODER_MODER5;
	PB->MODER |= GPIO_MODER_MODER5_1;

	//On veut une granularité de 1000 c'est à dire ARR = 1000, et une fréquence de sortie de 100Hz, donc f en sortie de prescaler = 1000*100 = 10e5
	//donc on règle le prescaler à 160


	//CK_CNT * PSC = CK_PSC => PSC= CK_PSC / CK_CNT = 16 000/10
	TIM2->PSC = SystemCoreClock /100000 -1;
	//auto reload = CK_PSC / f_led = 10000/2 - 1
	TIM2->ARR = 1000 - 1;
	//set EGR register bit to 1 to force the update and start the count at 0
	TIM2->EGR &= ~TIM_EGR_UG;
	TIM2->EGR |= TIM_EGR_UG_Msk;
	TIM2->SR = 0;

	//mise en output compare mode
	TIM2->CCMR1 &= ~TIM_CCMR1_OC1M_Msk;//PWM mode 1 : set OC1M bits from register CCMR1 to '110' (p351 ref manual)
	TIM2->CCMR1 |= TIM_CCMR1_OC1M_1;
	TIM2->CCMR1 |= TIM_CCMR1_OC1M_2;

	TIM2->CCER |= TIM_CCER_CC1E_Msk; //set CC1E bits from CCER register to enable output
	/// il y a des trucs à rajouter pour ccer, voir doc

	TIM2->CCR1 = 10;//jsplus pk

	//start timer
	TIM2->CR1 |= TIM_CR1_CEN_Msk;
}


void TIM2_set_pwn(unint32_t duty)
{

}


void gpio_init(void)
{
	/* 2 port instance are declared and mapped on GPIOA and GPIOC address
	 * GPIOA and GPIOC are constant address defined in stm32f401xe.h
	 */
	GPIO_TypeDef * PA = GPIOA, *PC = GPIOC, *PB = GPIOB;

	/* modify bit 0 and 3 of RCC AHB1ENR register to enable the clock
	 * of GPIOA and GPOC
	 * RCC is macro definced in stm32f401xe.h and RCC_AHB1ENR_GPIOAEN and
	 * RCC_AHB1ENR_GPIOCEN RCC_AHB1ENR_GPIOCEN are constant also defined in
	 * stm32f401xe.h
	 */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIOBEN;

	/* GPIOA configuration :  output, slow speed, push-pull, no pull
	 * After reset, only the output mode should be set. the other configuration
	 * are correct.
	 */
	/* PA5 in output mode */
	PA->MODER &= ~GPIO_MODER_MODER5_Msk; /* GPIO_MODER_MODER5_Msk : constante
											dÃ©finie dans stm32f401xe.h */
	PA->MODER |= GPIO_MODER_MODER5_0;	/* GPIO_MODER_MODER5_0, constante
											dÃ©finie dans stm32f401xe.h */
	/* Si on considÃ¨re que PA est dans un Ã©tat indÃ©terminÃ© */
	/* output push pull */
	PA->OTYPER &= ~GPIO_OTYPER_OT5_Msk;			// clear bit field
	/* slow speed, better for low electrical interference (less noisy) */
	PA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED5_Msk;	// clear bit field
	/* no pull-up or pull-down resistor */
	PA->PUPDR &= ~GPIO_PUPDR_PUPD5_Msk;			// clear bit field

	/* configure PC13 : input, no pull */
	PC->MODER &= ~GPIO_MODER_MODE13_Msk;		// clear bit field
	PC->PUPDR &= ~GPIO_PUPDR_PUPD13_Msk;		// clear bit field
	/* other config registers are irrelevant for pins in input mode */

	/* Activer horloger GPIOA */

	/* PA2(TX) en alternate function nÂ°7 */
	PB->AFR[0] &= ~GPIO_AFRL_AFRL7;
	PB->AFR[0] |= GPIO_AFRL_AFRL7_1;
	/* alternate function PA7*/
	PB->MODER &= ~GPIO_MODER_MODER7;
	PB->MODER |= GPIO_MODER_MODER7_1;
}

void TIM3_init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

	//CK_CNT * PSC = CK_PSC => PSC= CK_PSC / CK_CNT = 16 000/10
	TIM3->PSC = SystemCoreClock /10000 -1;
	//auto reload = CK_PSC / f_led = 10000/2 - 1
	TIM3->ARR = 10000/10/2 - 1;
	//set EGR register bit to 1 to force the update and start the count at 0
	TIM3->EGR &= ~TIM_EGR_UG;
	TIM3->EGR |= TIM_EGR_UG_Msk;
	//set bit OGR from CR1 register of TIM3 to 1 for 'one shot' behaviour
	//TIM3->CCR1 |= TIM_CR1_OPM_Msk;
	//errase status register (p347 'reset value = 0x0000')
	TIM3->SR = 0;
	//start timer
	TIM3->CR1 |= TIM_CR1_CEN_Msk;
}
