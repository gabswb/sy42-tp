#include "stm32f4xx.h"

__attribute__((constructor(105))) static void pin_init(void);

/**
 * @brief	constructor to configure pin before main()
 * @param	none
 * @retval	none
 */
static void pin_init(void)
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
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN;

	/***************************************************************************
	 * Pin configuration for the LED,
	 * PA5  output, slow speed, push-pull, no pull
	 * After reset, only the output mode should be set. the electric
	 * configuration are correct.
	 */
	/* electric configuration (reset the state if in an other state than the
	 * one after reset
	 */
	/* output push pull */
	PA->OTYPER &= ~GPIO_OTYPER_OT5_Msk;			// clear bit field
	/* slow speed, better for low electrical interference (less noisy) */
	PA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED5_Msk;	// clear bit field
	/* no pull-up or pull-down resistor */
	PA->PUPDR &= ~GPIO_PUPDR_PUPD5_Msk;			// clear bit field

#undef USE_PWM_LED
#ifndef USE_PWM_LED
	/* PA5 in output mode */
	PA->MODER &= ~GPIO_MODER_MODER5_Msk; /* GPIO_MODER_MODER5_Msk : constante
											définie dans stm32f401xe.h */
	PA->MODER |= GPIO_MODER_MODER5_0;	/* GPIO_MODER_MODER5_0, constante
											définie dans stm32f401xe.h */
#else
	/************************ PINS PA5 for TIM2 CH1 (PWM)**********************/
	/* PA5 driven by alternate function : */
	/* select the alternate function N°1 for PIN5  */
	PA->AFR[0] &= ~ (0xF << 20)	;
	PA->AFR[0] |= (0x1 << 20)	;	/* AF01 */
	/* choose alternate function in moder */
	PA->MODER &= ~GPIO_MODER_MODER5;
	PA->MODER |= GPIO_MODER_MODER5_1; /* alternate function */
#endif
	/***************************** PINS PA5 ***********************************/

	/***************************************************************************
	 * Pin configuration for the user button connected to PC13
	 * input,no pull
	 */
	PC->MODER &= ~GPIO_MODER_MODE13_Msk;		// clear bit field
	PC->PUPDR &= ~GPIO_PUPDR_PUPD13_Msk;		// clear bit field
	/* other config registers are irrelevant for pins in input mode */

	/***************************************************************************
	 * Pin configuration for USART2,
	 * PA2 et PA3 select alternate function N°7 in alternate function register
	 */
	/* TX on PA2 alternate function 7*/
	GPIOA->AFR[0] &= ~(0xF << (2*4) );	/* clear the 4 bits */
	GPIOA->AFR[0] |= (7 << (2*4) ); 	/* set alternate function Nbr 7*/
	/* RX on PA3 alternate function 7 */
	GPIOA->AFR[0] &= ~(0xF << (3*4) );	/* clear the 4 bits */
	GPIOA->AFR[0] |= (7 << (3*4) );		/* set alternate function Nbr 7*/
	/* Configure alternate function for USART2 TX/RX (PIN2/3) in mode register*/
	GPIOA->MODER &= ~(3 << (2 * 2) );	/*TX*/
	GPIOA->MODER &= ~(3 << (3 * 2) );	/*RX*/
	GPIOA->MODER |= (2 << (2 * 2) );	/*TX*/
	GPIOA->MODER |= (2 << (3 * 2) );	/*RX*/
	/************************** PINS PB7 for TIM4 CH2 *************************/
	/* Enable GPIOB and clock */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	/* PB7 as alternate function N°2*/
	PB->AFR[0] &= ~ GPIO_AFRL_AFSEL7_Msk	;
	PB->AFR[0] |= (0x2 << GPIO_AFRL_AFSEL7_Pos)	;	/* AF02 */
	PB->MODER &= ~GPIO_MODER_MODER7;
	PB->MODER |= GPIO_MODER_MODER7_1; /* alternate function */
	/************************** PINS PB7 for TIM4 CH2 *************************/
}
