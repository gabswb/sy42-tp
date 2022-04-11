#ifndef __USART_H
#define __USART_H


/**
 * Variables globales utilisées par USART2_transmit_IRQ()
 */
int index_transmit;
int len;
char* data;
/*----------------------------------------------------------*/

void USART2_set_IRQ();
int32_t USART2_transmit_IRQ (uint8_t * data, uint32_t len);


void USART2_Init(uint32_t baud);
uint32_t USART2_Print(char* str);
uint32_t USART2_Transmit(uint8_t * data, uint32_t len);
uint32_t USART2_Receive(uint8_t * data, uint32_t len);

int __io_putchar(int ch) ;
int __io_getchar(void) ;
#endif /* __USART_H */

