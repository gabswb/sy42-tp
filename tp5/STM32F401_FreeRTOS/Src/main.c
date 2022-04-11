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
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
extern void start_application(void);
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	/*priority grouping 4 bits for pre-emption priority 0 bits for subpriority
	 * (No Subpriority) for FreeRTOS*/
	NVIC_SetPriorityGrouping( 3 );

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

	 /* no tasks to run go in light sleep until next interrupt */
 	//__WFI();

 }

 /**
   * Tick Hook function, called from ISR SysTick
   */
 void vApplicationTickHook( void )
 {

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


