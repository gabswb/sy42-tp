/**
 ******************************************************************************
 * @file           : main.c
 * @author         : NL
 * @brief          : Main program body, FreeRTOS
 *
 ******************************************************************************
 */
/* header file that include stm32f401xe.h (in Drivers/CMSIS/Device/ST/Include/)
 * in the latest file are defined data structure and constant
 */
/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "main.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
extern void start_application(void);
/* Private functions ---------------------------------------------------------*/

int count_tick_system = 0;

SemaphoreHandle_t binary_usart2_mutex;

SemaphoreHandle_t get_binary_usart2_mutex()
{
	return binary_usart2_mutex;
}

void EXTI15_10_IRQHandler()
{
	BaseType_t xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;
	/*make sure the interrupt is enable for pc13*/
	if((EXTI->PR & EXTI_PR_PR13)&& (EXTI->IMR & EXTI_IMR_MR13)){

		xSemaphoreGiveFromISR( binary_usart2_mutex,  &xHigherPriorityTaskWoken);
		/*clear the bit by setting it to '1'*/
		EXTI->PR |= EXTI_PR_PR13;
	}
}


void EXTI15_10_set_IRQ()
{
	/** activate clock of peripheric GPIOC and SYSCFG*/
	 RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIOAEN;
	 RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	 /*Configuration of pc13 input mode*/
	 GPIOC->MODER &= ~GPIO_MODER_MODE13;
	 GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD13;

	 /*Séléction de PC13 en entrée de EXTI15_10*/
	 SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI13_PC;
	 SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;

	 /*AUtorise la détection de front descendant*/
	 EXTI->FTSR |= EXTI_FTSR_TR13;
	 /*: Interrupt request from line 13 is not masked*/
	 EXTI->IMR |= EXTI_IMR_MR13;

	 /*Set priority and enable IRQ on EXTI15_10*/
	 NVIC_SetPriority(EXTI15_10_IRQn,configLIBRARY_LOWEST_INTERRUPT_PRIORITY);
	 NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void config_stop_mode()
{
	/*set bit "deep sleep" to 1*/
	SCB->SCR |= (1 << SCB_SCR_SLEEPDEEP_Pos);
	/* Clear PDDS bit in Power Control register to Enter Stop mode when the CPU enters deepsleep*/
	PWR->CR &= ~PWR_CR_PDDS;
	/* set Low-power Voltage regulator on during Stop mode.(set the bit to 1)*/
	PWR->CR |= PWR_CR_LPDS;

	__WFI();
}



/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{

	EXTI15_10_set_IRQ();

	/*priority grouping 4 bits for pre-emption priority 0 bits for subpriority
	 * (No Subpriority) for FreeRTOS*/
	NVIC_SetPriorityGrouping( 3 );
	//config_stop_mode();
	/* create tasks and lanch scheduler */
	start_application();

 	/* Loop forever */
	for(;;);

 }





/*******************************************************************************
 * Functions for FreeRTOS
 */


/**
  * Idle Hook function, called in loop in the Idle Task
  */
 void vApplicationIdleHook( void )
 {
	 //__WFI();
	 /**
	  * Explication du test :
	  * On incrémente le compteur de tick system à chaque tick du system dans la fonction vApplicationTickHook
	  * Dans la fonction vApplicationIdleHook() on affiche chaque incrémentation du compteur
	  * Comme nous avons configurer le fichier FreeRTOSConfig.h en mode TickLess, le système s'endort automatiquement
	  * en fonction de l'attente passive programmée dans app_tasks avec vTaskDelay()
	  * Lorsque ces tasks "s'endorment", la fonction vApplicationIdleHook est appelée, et affiche l'incrémentation du compteur de tick system
	  * On remarque que la fréquence de cet affichage est proportionnelle à la période d'attende de vTaskDelay()
	  * De plus lorsque l'on remet le bit configUSE_TICKLESS_IDLE à 0, la fréquence d'incrémentation de tick système explose,
	  * De même lorqu'on appuie sur le bouton, cela génère une interruption et donc réveil le système
	  * Le système s'endort donc bien ce qui permet des économies d'énergie :)
	  */
	 /** to test if the system sleep*/
	 char* buffer = malloc(sizeof(uint8_t)*30);
	 sprintf(buffer, "idle hook, count : %d\r\n", count_tick_system);
	 USART2_Transmit_IRQ(buffer, strlen(buffer));

	 /*On attend que toutes les données aient été transmises*/
	 while(!USART2->SR & USART_SR_TC);
	 free(buffer);
 }

 /**
   * Tick Hook function, called from ISR SysTick
   */
 void vApplicationTickHook( void )
 {
	 /**to test if the system sleep*/
	 count_tick_system ++;
 }
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}

/**
  * @brief  small waiting functions based on a active counter
  * 		(time is really approx.)
  * @param  None
  * @retval None
  */
void simple_active_wait_ms(uint32_t ms){
	ms =(SystemCoreClock/1000/4)*ms;
	while(ms--)
		asm("");	// avoid the function to be optimized out as it doesn't do anything !
}


