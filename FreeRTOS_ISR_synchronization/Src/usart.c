#include "stm32f4xx.h"
#include <errno.h>

/**
  * @brief  constructor to Init USART2 @ 115200 baud before main()
  * @param  None
  * @retval None
  * */
__attribute__((constructor(108))) static void USART_Init_115200(void)
{
	USART2_Init(115200);
}

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

/**
  * @brief  get len 8 bytes data
  * @param  application buffer for incoming data
  * 		len (in bytes = nbr of data)
  * @retval nbr of data sent
  * */
uint32_t USART2_Receive(uint8_t * data, uint32_t len)
{
	uint32_t Nrdata = 0;
	uint32_t timeout;
	volatile uint32_t dummy_read;

	/*timout should depend of the baud rate and the core clock frequency */
	timeout =SystemCoreClock;

	// clear IDLE flag and error flags in case they are not cleared
	// the bits are cleared by a read of the status register
	// followed by a data register read
	if (USART2->SR & 0x1F)
		dummy_read = USART2->DR;

	/* infinite wait for first datum */
	while (!(USART2->SR & USART_SR_RXNE)){
		if (timeout == 0){
			return 0;
		}
		else {
			timeout--;
		}
	}

	/* get first datum*/
	*data++=USART2->DR ;
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
		*data++=USART2->DR ;
		/* increment the data counter */
		Nrdata++;
	}

	return len;
}



/****************************** Interrupt method *****************************/
#define TX_BUSY 2
#define RX_BUSY 1
#define RX_COMPLETED 4

/* Device info struct */
/**
  * global structure for USART Driver
  * Store pointer to user buffer (input and output)
  * number of data requested, and number of data already proceeded
  * */

struct USART2_Device{
	uint32_t state;
	uint8_t *pTxBuffer;
	uint8_t *pRxBuffer;
	uint32_t TxSize;
	uint32_t RxSize;
	uint32_t TxCount;
	uint32_t RxCount;
} usart2_dev={
		.state = 0,	// init to 0
	};

/**
  * @brief  send data on the serial line using interrupt
  * @param  adress of the data to send
  * @param	number of data
  * @retval 0 if success, -1 if the transmission is already busy
  * */
int32_t USART2_Transmit_IRQ( uint8_t * data, uint32_t len)
{

	/* critical section */
	NVIC_DisableIRQ(USART2_IRQn);

	/* test busy flag for transmission */
	if (usart2_dev.state & TX_BUSY){
		/* a transmission is running, quit critical section return error */
		NVIC_EnableIRQ(USART2_IRQn);
		errno = EAGAIN;
		return -1;
	}
	/* test if there are any data to send */
	if (len==0){
		NVIC_EnableIRQ(USART2_IRQn);
		return 0;
	}
	/* set busy flag for transmission */
	__DSB(); // synchronisation barrier
	usart2_dev.state |=  TX_BUSY;
	/* initialize the transmit info for ISR */
	usart2_dev.pTxBuffer = data;
	usart2_dev.TxSize = len;
	usart2_dev.TxCount=0;

	/* end of critical section */
	NVIC_EnableIRQ(USART2_IRQn);

	/* enable interrupt on transmit data register empty */
	USART2->CR1 |= USART_CR1_TXEIE;

	// return len
	return 0;
}


/**
  * @brief  ISR of USART2
  * @param  none
  * @retval none
  * */
void USART2_IRQHandler(void)
{
	uint32_t status, cr1, dummy_read;

	/* get status register */
	status = USART2->SR;
	/* get Interrupt sources */
	cr1 = USART2->CR1 & 0x1F0;

	/* TXIE interrupt (a char has been transfered in the serializer) */
	if ((status & USART_SR_TXE) && (cr1 & USART_CR1_TXEIE) ){
		if (usart2_dev.TxCount < usart2_dev.TxSize){
			USART2->DR = *usart2_dev.pTxBuffer++;
			usart2_dev.TxCount++;
		} else {
			/* No char to transmit anymore stop TXE interrupt */
			USART2->CR1 &= ~USART_CR1_TXEIE;
			/* optionally enable TC interrupt if the usart is going to be
			 * shut down */
//			USART2->CR1 |= USART_CR1_TCIE;

			/* now the transmission is not busy anymore */
			usart2_dev.state &= ~TX_BUSY ;
		}
	}

	/* TCIE interrupt (a char has been completely transmitted on the line) */
	if ((status & USART_SR_TC) && (cr1 & USART_CR1_TCIE) ){
		/* last char has been completely sent
		 * the line is now IDLE
		 * USART can be shut down to save power
		 * to maintain the line IDLE for a transmit character duration
		 * and enable end of frame detection a timer should be used
		 */
		/* stop the TC interrupt */
		USART2->CR1 &= ~USART_CR1_TCIE;
		/* now the transmission is not busy anymore */
		usart2_dev.state &= ~TX_BUSY ;
	}


	/* RXNE interrupt (a char has been received) */

	/* ... */

}
