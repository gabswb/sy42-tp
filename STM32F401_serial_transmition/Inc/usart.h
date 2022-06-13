/*
 * usart.h
 *
 *  Created on: Mar 17, 2022
 *      Author: gabriels
 */

#ifndef USART_H_
#define USART_H_
#include "stdint.h"


/*
 * usart.c
 *
 *  Created on: Mar 17, 2022
 *      Author: gabriels
 */

/**
  * @brief  Init USART2 @ baud,8,1,n
  * @param  baud rate
  * @retval None
  * */
void USART2_Init(uint32_t baud);

/**
 * @brief Configure le GPIOA des pins PA2 (TX) et PA3 (RX) en mode alternate avec la fonction alternative 7 : AF07.
 */
void GPIOA_Init();


/**
 * @brief envoie un caractere via USART2
 *
 * @param ch caractere a envoyer
 * @return caractere sous format int
 */
int __io_putchar(int ch);


/**
 * @brief envoie une chaine de caractre via USART2
 *
 * @param data adresse de la chaine de caractere
 * @param len longueur de la chaine de caractere
 */
void USART2_Transmit(uint8_t * data, uint32_t len);

/**
 * @brief receptin d'un caractere via USART2
 *
 * @return
 */
int __io_getchar(void);

/**
 * @brief reception d'une chaine de caracteres
 *
 * @param data buffer dans lequel on va stocker la donnee
 * @param len longueur max de la chaine de caractere
 * @return longueur de la chaine
 */
int32_t USART2_Receive(uint8_t * data, uint32_t len);


#endif /* USART_H_ */
