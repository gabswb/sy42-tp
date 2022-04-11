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
TaskHandle_t xHandle1 = NULL;
TaskHandle_t xHandle2 = NULL;
TaskHandle_t xHandle3 = NULL;
TaskHandle_t xHandle4 = NULL;

SemaphoreHandle_t usart2_mutex;
SemaphoreHandle_t binary_usart2_mutex;

/* Private function prototypes -----------------------------------------------*/
/* callback function used by the interrupt handler */
void increase_brightness(void);

/* tasks */
void T1( void * pvParameters );
void T2( void * pvParameters );
void T3( void * pvParameters );
void print_task1(int p);
void print_task2(int p);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Create Task and launch Scheduler
  * @parm 	none
  * @retval none
  */
void start_application(void)
{
	BaseType_t xReturned;


	/*
	 * Peripheral init
	 */
	/* pin (GPIO) and USART2 already done by constructor functions */

    /* Create the tasks, storing the handle. priority are from 0
     * (tskIDLE_PRIORITY) to configMAX_PRIORITIES defined in FreeRTOSConfig.h */
//    xReturned = xTaskCreate(
//                    T1,       /* Function that implements the task. */
//                    "Led 1",          /* Text name for the task. */
//                    STACK_SIZE,      /* Stack size in words, not bytes. */
//                    ( void * ) 1,    /* Parameter passed into the task. */
//                    tskIDLE_PRIORITY+6,/* Priority at which the task is created. */
//                    &xHandle1 );      /* Used to pass out the created task's handle. */
//
//    if( xReturned == pdPASS ){
//        /* The task was created.  Use the task's handle to delete the task. */
//        //vTaskDelete( xHandle );
//    }else{
//    	// not enough memory to create task
//    }
//
//    xReturned = xTaskCreate(
//                     T2,       /* Function that implements the task. */
//                     "Led 2",          /* Text name for the task. */
//                     STACK_SIZE,      /* Stack size in words, not bytes. */
//                     ( void * ) 2,    /* Parameter passed into the task. */
//                     tskIDLE_PRIORITY+6,/* Priority at which the task is created. */
//                     &xHandle2 );      /* Used to pass out the created task's handle. */
//
//     if( xReturned == pdPASS ){
//         /* The task was created.  Use the task's handle to delete the task. */
//         //vTaskDelete( xHandle );
//     }else{
//     	// not enough memory to create task
//     }
//
//     if( xReturned == pdPASS ){
//         /* The task was created.  Use the task's handle to delete the task. */
//         //vTaskDelete( xHandle );
//     }else{
//     	// not enough memory to create task
//     }

     xReturned = xTaskCreate(
                      T3,       /* Function that implements the task. */
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



      usart2_mutex = xSemaphoreCreateMutex();
      binary_usart2_mutex = xSemaphoreCreateBinary();


     xReturned = xTaskCreate(
					  print_task1,       /* Function that implements the task. */
					  "hello world 1",          /* Text name for the task. */
					  STACK_SIZE,      /* Stack size in words, not bytes. */
					  ( void * ) 1,    /* Parameter passed into the task. */
					  tskIDLE_PRIORITY+5,/* Priority at which the task is created. */
					  &xHandle3 );      /* Used to pass out the created task's handle. */

	  if( xReturned == pdPASS ){
		  /* The task was created.  Use the task's handle to delete the task. */
		  //vTaskDelete( xHandle );
	  }else{
		// not enough memory to create task
	  }

	 xReturned = xTaskCreate(
					  print_task2,       /* Function that implements the task. */
					  "hello world 2",          /* Text name for the task. */
					  STACK_SIZE,      /* Stack size in words, not bytes. */
					  ( void * ) 2,    /* Parameter passed into the task. */
					  tskIDLE_PRIORITY+5,/* Priority at which the task is created. */
					  &xHandle4 );      /* Used to pass out the created task's handle. */

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

/**
 * @brief	Task T1
 * @param	parameters given @ creation
 * @retval	none (should not return)
 */
void T1( void * pvParameters )
{
	/*************** For Debuging with FreeRTOS Trace macro *******************/
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

	/* tasks must not return, use an infinite loop */
	for(;;){
		/* switch le led ON */
		GPIOA->BSRR = GPIO_BSRR_BS5 ;
		vTaskDelay(1);
	}

}


/**
 * @brief	Task T1
 * @param	parameters given @ creation
 * @retval	none (should not return)
 */
void T2( void * pvParameters )
{
	/*************** For Debuging with FreeRTOS Trace macro *******************/
	#if configUSE_APPLICATION_TASK_TAG==1
		#ifndef USE_FUNCTION_FOR_TRACE_TAG
		/* profiling function : set a tag, tag=task number (pvParameters)*/
		vTaskSetApplicationTaskTag( NULL, pvParameters );
		#else
		/* set a callback function instead for the tag */
		vTaskSetApplicationTaskTag( NULL, T2callback );
		#endif //USE_FUNCTION_FOR_TRACE_TAG
	#endif //configUSE_APPLICATION_TASK_TAG==1
	/**************************************************************************/

	/* tasks must not return, use an infinite loop */
	for(;;){
		/* switch le led OFF */
		GPIOA->BSRR = GPIO_BSRR_BR5;
		vTaskDelay(1);
	}

}

void T3( void * pvParameters )
{
	/*************** For Debuging with FreeRTOS Trace macro *******************/
	#if configUSE_APPLICATION_TASK_TAG==1
		#ifndef USE_FUNCTION_FOR_TRACE_TAG
		/* profiling function : set a tag, tag=task number (pvParameters)*/
		vTaskSetApplicationTaskTag( NULL, pvParameters );
		#else
		/* set a callback function instead for the tag */
		vTaskSetApplicationTaskTag( NULL, T2callback );
		#endif //USE_FUNCTION_FOR_TRACE_TAG
	#endif //configUSE_APPLICATION_TASK_TAG==1
	/**************************************************************************/

	TickType_t xLastWakeTime;
	const TickType_t xFrequency = configTICK_RATE_HZ/2;

	 // Initialise the xLastWakeTime variable with the current time.
	 xLastWakeTime = xTaskGetTickCount();

	/* tasks must not return, use an infinite loop */
	for(;;){
		/* switch le led OFF */
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
		GPIOA->BSRR = GPIO_BSRR_BR5;
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
		GPIOA->BSRR = GPIO_BSRR_BS5 ;

	}

}

void print_task1(int p){
	char buffer[256];
	sprintf(buffer, "Hello from task %d\r\n", p);
	while(1){

		if( binary_usart2_mutex == NULL )
		    {
		        /* There was insufficient FreeRTOS heap available for the semaphore to
		        be created successfully. */
		    }
		    else
		    {
		    	xSemaphoreTake(binary_usart2_mutex, 10);
		    	USART2_Transmit(buffer, strlen(buffer));
		    	xSemaphoreGive( binary_usart2_mutex );
		    }

		/*if( usart2_mutex != NULL )
		    {
		        //See if we can obtain the semaphore.  If the semaphore is not
		        //available wait 10 ticks to see if it becomes free.
		        if( xSemaphoreTake( usart2_mutex, ( TickType_t ) 10 ) == pdTRUE )
		        {
		            //We were able to obtain the semaphore and can now access the
		            //shared resource.

		        	USART2_Transmit(buffer, strlen(buffer));

		            //We have finished accessing the shared resource.  Release the
		            //semaphore.
		            xSemaphoreGive( usart2_mutex );
		            //vTaskDelay(200);

		        }
		        else
		        {
		            //We could not obtain the semaphore and can therefore not access
		            //the shared resource safely.
		        }
		}*/
	}
}

void print_task2(int p){
	char buffer[256];
	sprintf(buffer, "Hello from task %d\r\n", p);
	while(1){

		if( binary_usart2_mutex == NULL )
		{
			/* There was insufficient FreeRTOS heap available for the semaphore to
			be created successfully. */
		}
		else
		{
			xSemaphoreTake(binary_usart2_mutex, 10);
			USART2_Transmit(buffer, strlen(buffer));
			xSemaphoreGive( binary_usart2_mutex );

		}


		/*if( usart2_mutex != NULL )
		    {
		        //See if we can obtain the semaphore.  If the semaphore is not
		        //available wait 10 ticks to see if it becomes free.
		        if( xSemaphoreTake( usart2_mutex, ( TickType_t ) 10 ) == pdTRUE )
		        {
		            //We were able to obtain the semaphore and can now access the
		            //shared resource.

		        	USART2_Transmit(buffer, strlen(buffer));

		            //We have finished accessing the shared resource.  Release the
		            //semaphore.
		            xSemaphoreGive( usart2_mutex );
		            //vTaskDelay(200);

		        }
		        else
		        {
		            //We could not obtain the semaphore and can therefore not access
		            //the shared resource safely.
		        }
		}*/
	}
}

void writer1_task(){
}
