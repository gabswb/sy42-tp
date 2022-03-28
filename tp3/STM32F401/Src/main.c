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


/**
  * @brief main function
  * @param  none
  * @retval 0 if success
 */

int main(void)
{
	led_init();

	TIM_TypeDef* tim3 = TIM3;
	GPIO_TypeDef * PA = GPIOA, *PC = GPIOC;
	int is_activate = 1;



	//CK_NT * PSC = CK_CNT => PSC= CK_CNT / CK_INIT = 10/16000
	tim3->PSC = (16-1/1)*1000;
	//auto reload = T_timer/(PSC*CK_INT) - 1 = 1/2*10000 -1
	tim3->ARR = 1/(2*10000) -1;
	//set EGR register bit to 1 to force the update and start the count at 0
	tim3->EGR |= TIM_EGR_UG_Msk;
	//set bit OGR from CR1 register of TIM3 to 1 for 'one shot' behaviour
	//TIM3->CCR1 |= TIM_CR1_OPM_Msk;
	//errase status register (p347 'reset value = 0x0000')
	tim3->SR &= 0x0000;
	//start timer
	TIM3->CR1 |= TIM_CR1_CEN_Msk;



	for(;;){

		/* activation alternate fonction pour GPIOB pour la pin PB7*/
		GPIOB->MODER &= ~GPIO_MODER_MODER7_Msk;// mise à '10' des bits du registre moder7 pour alternate fonction
		GPIOB->MODER |= GPIO_MODER_MODER7_1;
		GPIOB->AFR[0] &= (0xF<<1*4);// dans la data sheet, section alternate fonction mapping p44, on voit que TIM4_CH2 est mappé avec la fonction alternative AF02 sur la pin PA1
		GPIOB->AFR[0] |= (2<<1*4);

		/* désactivation input caputre mode */
		TIM4->CCER &= ~TIM_CCER_CC1E_Msk;
		TIM4->CCER &= ~TIM_CCER_CC2E_Msk;

		/* modification de CCMRR1 */
		TIM4->CCMR1









//		/** clignotage de led**/
//		while(!(tim3->SR & ~TIM_SR_UIF_Msk)){
//		}
//		if(is_activate){
//			/* éteint la led */
//			PA->ODR = PA->ODR & ~(1<<5);// met le bit 5 à 0
//			//PA->ODR &= ~GPIO_ODR_OD5;
//		}
//		else{
//			/* allume la led */
//			PA->ODR = PA->ODR | 1<<5; // met le bit 5 à 1
//			//PA->ODR |= GPIO_ODR_OD5;	//(1<<5);
//		}
//
//		tim3->SR &= ~TIM_SR_UIF_Msk;
	}
	return 0;
}
//alternate fonction 2 au tim4 ch2 (ti2)
//pb7 c'est GPIOB



void led_init()
{
	/* Activation horloge */
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN;

		/* Configuration du GPIOA */
		GPIO_TypeDef *PA = GPIOA;
		GPIO_TypeDef *PC = GPIOC;
		PA->MODER = PA->MODER & ~(1<<11); // met le bit 11 à 0
		PA->MODER = PA->MODER | (1<<10); // met le bit 10 à 1

		/* allume la led */
		PA->ODR = PA->ODR | 1<<5; // met le bit 5 à 1

		/* éteint la led */
		PA->ODR = PA->ODR & ~(1<<5);// met le bit 5 à 0

		// Pour la config des registres voir page 151 du reference-manual

		/* Met PA0 en input et PA1 en general purpose output mode = ...0010 */
		PA->MODER &= ~(1<<0);
		PA->MODER &= ~(1<<1);
		PA->MODER &= ~(1<<3);
		PA->MODER |= 0b0100;

		/* Mettre PA1 en open-drain = ...0. */
		PA->OTYPER &= ~(1<<1);

		/* Mettre PA0 en pull-up = ...01 */
		PA->OSPEEDR &= ~(1<<1);
		PA->OSPEEDR |= 1<<0;
}

static void pin_init(void)
{
	GPIO_TypeDef * PA = GPIOA, *PC = GPIOC;

		/* modify bit 0 and 3 of RCC AHB1ENR register to enable the clock
		 * of GPIOA and GPOC
		 * RCC is macro definced in stm32f401xe.h and RCC_AHB1ENR_GPIOAEN and
		 * RCC_AHB1ENR_GPIOCEN RCC_AHB1ENR_GPIOCEN are constant also defined in
		 * stm32f401xe.h
		 */
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN;

		/* GPIOA configuration :  output, slow speed, push-pull, no pull
		 * After reset, only the output mode should be set. the other configuration
		 * are correct.
		 */
		/* PA5 in output mode */
		PA->MODER &= ~GPIO_MODER_MODER5_Msk; /* GPIO_MODER_MODER5_Msk : constante
												définie dans stm32f401xe.h */
		PA->MODER |= GPIO_MODER_MODER5_0;	/* GPIO_MODER_MODER5_0, constante
												définie dans stm32f401xe.h */
		/* Si on considère que PA est dans un état indéterminé */
		/* output push pull */
		PA->OTYPER &= ~GPIO_OTYPER_OT5_Msk;			// clear bit field
		/* slow speed, better for low electrical interference (less noisy) */
		PA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED5_Msk;	// clear bit field
		/* no pull-up or pull-down resistor */
		PA->PUPDR &= ~GPIO_PUPDR_PUPD5_Msk;			// clear bit field

		/* configure PC13 : input, no pull */
		PC->MODER &= ~GPIO_MODER_MODE13_Msk;		// clear bit field
		PC->PUPDR &= ~GPIO_PUPDR_PUPD13_Msk;		// clear bit field

}






