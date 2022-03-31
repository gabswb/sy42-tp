#include "stm32f4xx.h"
#include "string.h"

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


/********************* polling functions ***************************/

/**
  * @brief  send a char
  * @param  char to send
  * @retval char sent
  * */
int __io_putchar(int ch)
{
	while (!(USART2->SR & USART_SR_TXE)){
	}
	USART2->DR = ch;
	return ch;
}

/**
  * @brief  get a char (infinite wait)
  * @param  none
  * @retval char got
  * */
int __io_getchar(void)
{
		while (!(USART2->SR & USART_SR_RXNE)){
			/* wait for datum */
		}
		return (int) USART2->DR;
}

/**
  * @brief  send len 8 bytes data
  * @param  data address
  * 		len (in bytes = nbr of data)
  * @retval nbr of data sent
  * */
uint32_t USART2_Transmit(uint8_t * data, uint32_t len)
{
	uint32_t Nrdata = 0;

	while (Nrdata < len){
		while (!(USART2->SR & USART_SR_TXE)){
			/* wait for TXE to be set */
		}
		/* store the datum in TDR */
		USART2->DR = *data++;
		/* increment the data counter */
		Nrdata++;
	}
	/* wait last data to be completely sent (Optionnal) */
	//while (!(USART2->SR & USART_SR_TC));

	return Nrdata;
}

uint32_t USART2_Print(char* str){
    return USART2_Transmit((uint8_t*) str, strlen(str));
}

/**
  * @brief  get len 8 bytes data
  * @param  application buffer for incoming data
  * 		len (in bytes = nbr of data)
  * @retval nbr of data sent
  * */
uint32_t USART2_Receive(uint8_t * data, uint32_t len)
{
	uint32_t Nrdata = 0;
	uint32_t dummy_read;

	// clear IDLE flag and error flags in case they are not cleared
	// the bits are cleared by a read of the status register
	// followed by a data register read
	if (USART2->SR & 0x1F)
		dummy_read = USART2->DR;

	/* get first datum*/
	while (!(USART2->SR & USART_SR_RXNE));
	data[Nrdata]=USART2->DR ;
	Nrdata++;

	/* time out after */
	while ( Nrdata < len ){
		/* wait for RXNE or IDLE to be set  */
		while (!(USART2->SR & USART_SR_RXNE)){
			if ( USART2->SR & USART_SR_IDLE ){
				/* end of data frame
				 * clear IDLE and return the number of data received
				 */
				dummy_read = USART2->DR;
				return Nrdata;
			}
		}
		/* get the datum from RDR and copy it in the application buffer*/
		data[Nrdata]=USART2->DR ;
		/* increment the data counter */
		Nrdata++;
	}

	return len;
}


/**
  * @brief  De-Init USART2
  * @param  None
  * @retval None
  * */
void USART2_DeInit(void)
{

	/* reset/de-reset USART2 */
	RCC->APB1RSTR |= RCC_APB1RSTR_USART2RST;
	RCC->APB1RSTR &= ~RCC_APB1RSTR_USART2RST;
	/* de-activate the USART2 clk */
	RCC->APB1ENR &= ~RCC_APB1ENR_USART2EN;
}

