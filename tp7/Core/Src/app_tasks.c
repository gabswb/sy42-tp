/**
 ******************************************************************************
 * @file           : application_tasks.c
 * @author         : Nicolas Lacaille
 * @brief          : main application
 ******************************************************************************
 *******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Private includes ----------------------------------------------------------*/
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "semphr.h"
#include "usart.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define STACK_SIZE	512 // in words (= 4*STACK_SIZE bytes)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TaskHandle_t xHandle1 = NULL;
TaskHandle_t xHandle2 = NULL;
TaskHandle_t xHandle3 = NULL;

/* mutex for USART2 LOCK  */
SemaphoreHandle_t uart2_lock;

/* Private function prototypes -----------------------------------------------*/
// declare Tasks
void T1( void * pvParameters );
void T2( void * pvParameters );
void T3( void * pvParameters );
/* callback function for USART2 */
void HAL_UART_TxCpltCallback (UART_HandleTypeDef * huart);
void HAL_UART_RxCpltCallback (UART_HandleTypeDef * huart);
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart);
void HAL_UART_AbortReceiveCpltCallback (UART_HandleTypeDef * huart);
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef * huart, uint16_t len);
/* other functions */
void simple_active_wait_ms(uint32_t);
int __io_getchar(void);
int __io_putchar(int ch);
/* Private user code ---------------------------------------------------------*/


/**
  * @brief  Create Task and launch Scheduler
  * @retval int
  */
void start_application(void)
{
  xTaskCreate(
               T1,       /* Function that implements the task. */
			  "Blink Task",          /* Text name for the task. */
			  STACK_SIZE,      /* Stack size in words, not bytes. */
			  ( void * ) 1,    /* Parameter passed into the task. */
			  tskIDLE_PRIORITY+8,/* Priority at which the task is created. */
			  &xHandle1 );      /* Used to pass out the created task's handle. */

  xTaskCreate(
              T2,       /* Function that implements the task. */
			  "Print Task",          /* Text name for the task. */
			  STACK_SIZE*4,      /* Stack size in words, not bytes. */
			  ( void * ) 2,    /* Parameter passed into the task. */
			  tskIDLE_PRIORITY+6,/* Priority at which the task is created. */
			  &xHandle2 );      /* Used to pass out the created task's handle. */

  xTaskCreate(
              T3,       /* Function that implements the task. */
			  "Print Task",          /* Text name for the task. */
			  STACK_SIZE*4,      /* Stack size in words, not bytes. */
			  ( void * ) 3,    /* Parameter passed into the task. */
			  tskIDLE_PRIORITY+6,/* Priority at which the task is created. */
			  &xHandle3 );      /* Used to pass out the created task's handle. */


#if USE_HAL_UART_REGISTER_CALLBACKS == 1
  pUART_CallbackTypeDef txcallback = HAL_UART_TxCpltCallback ;
  HAL_UART_RegisterCallback(&huart2, HAL_UART_TX_COMPLETE_CB_ID, txcallback);
  HAL_UART_RegisterCallback(&huart2,HAL_UART_RX_COMPLETE_CB_ID,
		  	  	  	  	  	  	  	  	  	  	  HAL_UART_RxCpltCallback) ;
  HAL_UART_RegisterCallback(&huart2,HAL_UART_ERROR_CB_ID,
		  	  	  	  	  	  	  	  	  	  	  HAL_UART_ErrorCallback);
  HAL_UART_RegisterCallback(&huart2,HAL_UART_ABORT_RECEIVE_COMPLETE_CB_ID,
		  	  	  	  	  	  	  	  	  HAL_UART_AbortReceiveCpltCallback);
  /* specific function for receive_to_idle callback function */
  HAL_UART_RegisterRxEventCallback(&huart2, HAL_UARTEx_RxEventCallback);

#endif

  // for USART2 (stdio):
  uart2_lock = xSemaphoreCreateMutex();

  vTaskStartScheduler();
  /* Infinite loop */
  while (1)
  {
  }

}


/**
 * @brief	Task T1, blink led
 * @param	parameters given @ creation
 * @retval	none (should not return)
 */
void T1( void * pvParameters )
{
	/*************** For Debugging with FreeRTOS Trace macro *******************/
	#if configUSE_APPLICATION_TASK_TAG==1
		#ifndef USE_FUNCTION_FOR_TRACE_TAG
		/* profiling function : set a tag, tag=task number (pvParameters)*/
		vTaskSetApplicationTaskTag( NULL, pvParameters );
		#else
		/* set a callback function in tag */
		vTaskSetApplicationTaskTag( NULL, T1callback );
		#endif //USE_FUNCTION_FOR_TRACE_TAG
	#endif //configUSE_APPLICATION_TASK_TAG==1
	/**************************************************************************/

	TickType_t LastWakeTime;
	const TickType_t Delay = 200 / portTICK_PERIOD_MS; //(200*configTICK_RATE_HZ)/1000


	LastWakeTime = xTaskGetTickCount();

	for( ;; ){
		// periodic loop, with Delay calculated from the last wake up
		HAL_GPIO_WritePin (GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
		vTaskDelayUntil( &LastWakeTime, Delay );
		HAL_GPIO_WritePin (GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
		vTaskDelayUntil( &LastWakeTime, Delay );
		}
}


/*
 * @brief  print task : send message to the host via USART2
 * @param  parameters given at the task creation
 * @retval none (should not return)
 */
void T2( void * pvParameters )
{
	/*************** For Debugging with FreeRTOS Trace macro *******************/
	#if configUSE_APPLICATION_TASK_TAG==1
		#ifndef USE_FUNCTION_FOR_TRACE_TAG
		/* profiling function : set a tag, tag=task number (pvParameters)*/
		vTaskSetApplicationTaskTag( NULL, pvParameters );
		#else
		/* set a callback function in tag */
		vTaskSetApplicationTaskTag( NULL, T2callback );
		#endif //USE_FUNCTION_FOR_TRACE_TAG
	#endif //configUSE_APPLICATION_TASK_TAG==1
	/**************************************************************************/

		HAL_StatusTypeDef HAL_status = HAL_BUSY ; // HAL_OK, HAL_ERROR, HAL_TIMEOUT

	while(1){

		do{
			// synchronize access to test USART2 HAL lock
			xSemaphoreTake(uart2_lock,portMAX_DELAY);
			HAL_status = HAL_UART_Transmit_IT (&huart2,
								(uint8_t *) "Task T2 : print task \r\n",23);
			xSemaphoreGive(uart2_lock);
			if (HAL_status != HAL_OK ){
				/* a transmission is running, wait before trying again */
				vTaskDelay( 1 );
			}
		}while (HAL_status == HAL_BUSY);


		vTaskDelay(configTICK_RATE_HZ);// every second
	}

}

/*
 * @brief  print task : send message to the host via USART2
 * @param  parameters given at the task creation
 * @retval none (should not return)
 */
void T3( void * pvParameters )
{
	/*************** For Debugging with FreeRTOS Trace macro *******************/
	#if configUSE_APPLICATION_TASK_TAG==1
		#ifndef USE_FUNCTION_FOR_TRACE_TAG
		/* profiling function : set a tag, tag=task number (pvParameters)*/
		vTaskSetApplicationTaskTag( NULL, pvParameters );
		#else
		/* set a callback function in tag */
		vTaskSetApplicationTaskTag( NULL, T3callback );
		#endif //USE_FUNCTION_FOR_TRACE_TAG
	#endif //configUSE_APPLICATION_TASK_TAG==1
	/**************************************************************************/
	while(1){
		/* simulate a process that take 200ms */
		simple_active_wait_ms(200);
		/* task sleep for 500ms)*/
		vTaskDelay(configTICK_RATE_HZ/2); // wait 1/2 second
	}

}

void HAL_UART_TxHalfCpltCallback (UART_HandleTypeDef * huart)
{
	if (huart->Instance == USART2){
		asm("nop");
	}
}

void HAL_UART_TxCpltCallback (UART_HandleTypeDef * huart)
{

	if (huart->Instance == USART2){
		asm("nop");
	}
}

/*******************************************************************************
 * Functions for FreeRTOS
 */


/**
  * Idle Hook function, called in loop in the Idle Task
  */
 void vApplicationIdleHook( void )
 {
static TickType_t Idle_tick_cnt=0;
#if configUSE_TICKLESS_IDLE==0
	 /* no tasks to run go in sleep until next interrupt */
 	__WFI();

#else
 	/* in tickless mode, __WFI is called by the idle Task at return of this
 	 * function,
 	 */
 	/* for debug purpose get the number of ticks */
	 if (Idle_tick_cnt != xTaskGetTickCount()){
		 Idle_tick_cnt = xTaskGetTickCount();
	 }
#endif

 }



 /**
   * Tick Hook function, called from ISR SysTick
   */
 void vApplicationTickHook( void )
 {

 }

 /**
   * @brief  send a char using polling
   * @param  char to send
   * @retval char sent
   * */
 int __io_putchar(int ch)
 {
 	if (HAL_UART_Transmit (&huart2, (uint8_t*)&ch, 1, HAL_MAX_DELAY)==HAL_OK)
 		return ch;
 	else
 		return -1;

 }

 /**
   * @brief  get a char (infinite wait)
   * @param  none
   * @retval char got
   * */
 int __io_getchar(void)
 {
 	uint8_t ch;
 	if (HAL_UART_Receive (&huart2, &ch, 1, HAL_MAX_DELAY)==HAL_OK)
 		return (int)ch;
 	else
 		return -1;
 }

 /**
   * @brief  small waiting functions based on a active counter
   * 		(time is really approx.)
   * @param  None
   * @retval None
   */
 void simple_active_wait_ms(uint32_t ms)
 {
 	ms =(SystemCoreClock/1000/4)*ms;
 	while(ms--)
 		asm("");	// avoid the function to be optimized out as it doesn't do anything !
 }

