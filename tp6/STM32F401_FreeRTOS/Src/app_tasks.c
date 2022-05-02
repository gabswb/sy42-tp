/**
 ******************************************************************************
 * @file           : application_tasks.c
 * @author         : Nicolas Lacaille
 * @brief          : main application tasks
 ******************************************************************************
 *******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
/* Private includes ----------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "usart.h"
#include "timer.h"
#include "trace.h"

#include "string.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define STACK_SIZE	512 // in words (= 4*STACK_SIZE bytes)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TaskHandle_t xHandle5 = NULL;
TaskHandle_t xHandle2 = NULL;
TaskHandle_t xHandle3 = NULL;
TaskHandle_t xHandle4 = NULL;


/* Private function prototypes -----------------------------------------------*/
/* callback function used by the interrupt handler */
void increase_brightness(void);

/* tasks */
void pin13_activation(int p);
void led_blinking(int p);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Create Task and launch Scheduler
  * @parm 	none
  * @retval none
  */
void start_application(void)
{
	BaseType_t xReturned;

      binary_usart2_mutex = xSemaphoreCreateBinary();

	 xReturned = xTaskCreate(
			 	 	 	pin13_activation,       /* Function that implements the task. */
					  "pin waiting activation",          /* Text name for the task. */
					  STACK_SIZE,      /* Stack size in words, not bytes. */
					  ( void * ) 4,    /* Parameter passed into the task. */
					  tskIDLE_PRIORITY+5,/* Priority at which the task is created. */
					  &xHandle5 );      /* Used to pass out the created task's handle. */

	  if( xReturned == pdPASS ){
		  /* The task was created.  Use the task's handle to delete the task. */
		  //vTaskDelete( xHandle );
	  }else{
		// not enough memory to create task
	  }

	 xReturned = xTaskCreate(
					  led_blinking,       /* Function that implements the task. */
					  "Led 3",          /* Text name for the task. */
					  STACK_SIZE,      /* Stack size in words, not bytes. */
					  ( void * ) 2,    /* Parameter passed into the task. */
					  tskIDLE_PRIORITY+5,/* Priority at which the task is created. */
					  &xHandle2 );      /* Used to pass out the created task's handle. */

	  if( xReturned == pdPASS ){
		  /* The task was created.  Use the task's handle to delete the task. */
		  //vTaskDelete( xHandle );
	  }else{
		// not enough memory to create task
	  }




     // start the scheduler, task will be started and the
     // function will not return
	vTaskStartScheduler();


}


void pin13_activation(int p){
		char buffer[256];
		sprintf(buffer, "Activation numero %d\r\n", p);
		while(1){

			if( binary_usart2_mutex == NULL )
			    {
			        /* There was insufficient FreeRTOS heap available for the semaphore to
			        be created successfully. */
			    }
			    else
			    {
			        if( xSemaphoreTake( binary_usart2_mutex, ( TickType_t ) portMAX_DELAY ) == pdTRUE )
			        {
						USART2_Transmit(buffer, strlen(buffer));
			        }
			    }
		}
}

void led_blinking(int p)
{
	/* Block for 500ms. */
	 const TickType_t xDelay = 500 / portTICK_PERIOD_MS;

	/* tasks must not return, use an infinite loop */
	for(;;){


		//vTaskDelayUntil( &xLastWakeTime, xFrequency );
		vTaskDelay(xDelay);
		GPIOA->BSRR = GPIO_BSRR_BR5;/* switch le led OFF */
		//vTaskDelayUntil( &xLastWakeTime, xFrequency );
		vTaskDelay(xDelay);
		GPIOA->BSRR = GPIO_BSRR_BS5 ;/*switch led ON*/

	}

}


