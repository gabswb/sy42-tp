/**
 ******************************************************************************
 * @file           : main.c
 * @author         : NL
 * @brief          : Main program body,
 * 					 simple application to test and debug
 ******************************************************************************
 */
#include "main.h"
#include "usart.h"
#include "system_stm32f4xx.h"



/**
  * @brief main function
  * @param  none
  * @retval 0 if success
 */

int main(void)
{

	GPIOA_Init();
	USART2_Init(115200);

	uint8_t data[] = "Hello World";
	char* buffer;


	/* systeme 'bare metal', pas de systeme d'exploitation qui maintient une
	 * boucle infinie lorsqu'aucune application ne tourne, l'application doit
	 * comprendre une boucle infinie
	 */
	while (1)
	{

		//__io_putchar('A');
		//USART2_Transmit(&data, 12);
		printf("value : %d",__io_getchar());
	}

	return 0;
}



