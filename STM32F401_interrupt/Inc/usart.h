#ifndef __USART_H
#define __USART_H

#include "stdint.h"

/**
 * Variables globales utilisées par USART2_transmit_IRQ()
 */
int index_transmit;//index de la derniere donnée transmise 
int len;//longueur de la donnée à transmettre
char* data;//donneé à transmettre
/*----------------------------------------------------------*/

/**
 * Initialise USART2 pour générer des requêtes d’interruption
 */
void USART2_set_IRQ();

/**
 * @brief fonction non bloquante qui permet d'initer une émission de donnée
 * 
 * @param data donnée à transmettre
 * @param len longueur de la donnée
 * @return int32_t -1 si déjà en cours d'émission, 0 sinon à la fin de l'interruption
 */
int32_t USART2_transmit_IRQ (uint8_t * data, uint32_t len);


/** tp précédent, non utilisée ici*/
void USART2_Init(uint32_t baud);
uint32_t USART2_Print(char* str);
uint32_t USART2_Transmit(uint8_t * data, uint32_t len);
uint32_t USART2_Receive(uint8_t * data, uint32_t len);

int __io_putchar(int ch) ;
int __io_getchar(void) ;
#endif /* __USART_H */

