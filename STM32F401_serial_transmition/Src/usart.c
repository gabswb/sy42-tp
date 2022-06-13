/*
 * usart.c
 *
 *  Created on: Mar 17, 2022
 *      Author: gabriels
 */

#include "system_stm32f4xx.h"
#include "usart.h"
#include "stm32f401xe.h"

/**
  * @brief  Init USART2 @ baud,8,1,n
  * @param  baud rate
  * @retval None
  * */
void USART2_Init(uint32_t baud)
{
	uint32_t tmp = 0, divmantissa, divfraction, apbclk;

	/* initialisation de l'USART2 : baud,8,1,n */

	/* reset/de-reset USART2 */
	RCC->APB1RSTR |= RCC_APB1RSTR_USART2RST;
	RCC->APB1RSTR &= ~RCC_APB1RSTR_USART2RST;
	/* enable USART2 clk */
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

	/*-------------- UART parameters configuration --------------- */
	USART2->CR1 &= ~USART_CR1_UE;
	/* USART CR1 Configuration : tx and rx enable; oversampling = 16 */
	USART2->CR1 = USART_CR1_TE | USART_CR1_RE;
	/* USART CR2 Configuration : 1 stop bit*/
	USART2->CR2 = 0 ;
	/* USART CR3 Configuration : no flow control*/
	USART2->CR3 = 0;
	/* USART BRR Configuration : depend on the bus frequency*/
	/* get APB1 prescaler to determine the USART clock frequency apbclk*/
	tmp = (RCC->CFGR & RCC_CFGR_PPRE1)>>10;
	if (tmp & 4){
		tmp =  (tmp & 3) + 1;
		apbclk = SystemCoreClock >> tmp;
	}
	else {
		apbclk = SystemCoreClock;
	}

	/* compute baud rate from the apbclock frequency
	 *  baud = apbclk /(8*(2-OVER8)*USARTDIV) */

	tmp = (USART2->CR1 & USART_CR1_OVER8)>>15;
	if (tmp == 0) {
		/* OVER8 = 0, div by 16 */
		divmantissa = (apbclk/baud) >> 4;
		divfraction = (apbclk/baud) & 0xF;
	}
	else {
		/* OVER8 = 0, div by 8 */
		divmantissa = (apbclk/baud) >> 3;
		divfraction = (apbclk/baud) & 3;
	}
	/*USART2->BRR = mantissa and fraction part*/
	USART2->BRR = (divmantissa << 4) | divfraction ;


	/* enable USART */
	USART2->CR1 |= USART_CR1_UE;

	/*-------------- interrupt --------------- */
	//NVIC_SetPriority(USART2_IRQn,0x15); /*  priority */
	//NVIC_EnableIRQ(USART2_IRQn);
}

/**
 * @brief Configure le GPIOA des pins PA2 (TX) et PA3 (RX) en mode alternate avec la fonction alternative 7 : AF07.
 *
 *
 */
void GPIOA_Init()
{

	/**
	 * activation de l'horloge
	 */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN;//horloge

	/**
	 * Configuration du GPIOA en mode alternate
	 * selon la documentation, il faut dont mettre les deux bits des registres des pin 2 et 3 à '10'.
	 * On utilise pour cela les masques définit dans le fichier "stm32f401xe.h"
	 */
	GPIOA->MODER &= ~GPIO_MODER_MODER3_Msk;//On met les deux bits de chaque registre à '0'
	GPIOA->MODER &= ~GPIO_MODER_MODER2_Msk;
	GPIOA->MODER |= GPIO_MODER_MODER2_1;//On met le bit de rang 1 à '1'
	GPIOA->MODER |= GPIO_MODER_MODER3_1;


	/**
	 * La configuration de la fonction alternative se fait dans le registre AFR du GPIOA, celle-ci est codée sur 4 bits.
	 * La fonction AF7 se code '0111'
	 * On utilise les macros founrient par le consctructeur
	 */
	GPIOA->AFR[0] &= GPIO_AFRL_AFSEL2_Msk;//Mise à '0' des bits
	GPIOA->AFR[0] &= GPIO_AFRL_AFSEL3_Msk;
	GPIOA->AFR[0] |= (7<<GPIO_AFRL_AFSEL2_Pos);//Choisit la fonction alternative 7
	GPIOA->AFR[0] |= (7<<GPIO_AFRL_AFSEL3_Pos);
}

/**
 * @brief envoie un caractere via USART2
 *
 * @param ch caractere a envoyer
 * @return caractere sous format int
 */
int __io_putchar(int ch)
{
	/**
	 *On attend que le drapeau du registre d'etat SR est a 1
	 */
	while(!(USART2->SR & USART_SR_TXE)){
	}
	USART2->DR = ch;//copie de la chaine dans le registre DR
	return ch;
}

/**
 * @brief envoie une chaine de caractre via USART2
 *
 * @param data adresse de la chaine de caractere
 * @param len longueur de la chaine de caractere
 */
void USART2_Transmit(uint8_t * data, uint32_t len)
{
	/**
	 * temps que la chaine n'est pas vide
	 */
	int i = 0;
	while(i<len){
		__io_putchar(data[i++]);//On utilise __io_putchar pour envoyer les caracteres de la chaine un a un
	}
	while (!(USART2->SR & USART_SR_TC));//attend que tout ai ete envoye

}

/**
 * @brief receptin d'un caractere via USART2
 *
 * @return le caractere contenu dans USART2->DR
 */
int __io_getchar(void)
{

	while(!(USART2->SR & USART_SR_RXNE)){// attend que le registre RX soit vide
	}

	return USART2->DR;//renvoie la donnee dans le registre DR
}

/**
 * @brief reception d'une chaine de caracteres
 *
 * @param data buffer dans lequel on va stocker la donnee
 * @param len longueur max de la chaine de caractere
 * @return longueur de la chaine
 */
int32_t USART2_Receive(uint8_t * data, uint32_t len)
{
	USART2->SR &= ~USART_SR_IDLE; //reinitialise IDLE à 0
	int i=0;//compteur
	while(!(USART2->SR & USART_SR_IDLE) && i<len){//temps que IDLE est different de 1 et donc que la trame n'est pas finie
												 // et tant que la longueur de la chaine ne depasse pas la longueur max
		data[i++] = __io_getchar();//recupere la chaine
	}
	return i;//retourne la longueur de la chaine recuperee
}
