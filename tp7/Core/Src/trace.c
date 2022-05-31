/**
 ******************************************************************************
 * @file           : trace.c
 * @author         : NL
 * @brief          : functions for trace macro
 *
 ******************************************************************************
 */
/* header file that include stm32f401xe.h (in Drivers/CMSIS/Device/ST/Include/)
 * in the latest file are defined data structure and constant
 */
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "stm32f4xx.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"


void TIM5_Init(uint32_t freq);
uint32_t TIM5_get_cnt(void);

// profiler counter
uint32_t T1_count;
uint32_t T2_count;
uint32_t T3_count;
uint32_t Idle_count;


BaseType_t T1callback( void * pvParameter )
{
	T1_count++;	// led on
	return 0;
}
BaseType_t T2callback( void * pvParameter )
{
	T2_count++;	// led on
	return 0;
}

void task_to_ready(void * pvParameter)
{
	asm("nop");
}


/**
 * @brief   to know which task will run
 * 			task tag is used here, last_TaskTag is set to
 * 			the last task switched out in FreeRTOSConfig.h with trace macro
 * @parm   	task tag
 */

void switched_in(int task_tag)
{
//	if (last_TaskTag != task_tag){
		// context switch has occured : preemption or time-slice
		// do whatever ..
		// increase profiling counter
		switch (task_tag) {
			case 0 :
				Idle_count++;
				break;
			case 1 :
				T1_count++;
				break;
			case 2 :
				T2_count++;
				break;
			case 3 :
				T3_count++;
				break;
			default :

				break;
		}
//	}
}
/**
 * @brief   to know which task will be switched out
 * 			task tag is used here, last_TaskTag is set to
 * 			the last task switched out in FreeRTOSConfig.h with trace macro
 * @parm   	task tag
 */
void switched_out(int task_tag)
{
	switch (task_tag) {
		case 0 :
			break;
		case 1 :
			break;
		case 2 :
			break;
		case 3 :
			break;
		default :
			break;
	}

}



#define STAT_SIZE 1024
/**
 * @brief format task statistic in us
 * @param destination string
 * @retval size of string
 */
uint32_t get_tasks_statistics(char * str)
{
#if 0
	uint32_t str_len;
	HeapStats_t xHeapStats;

	/* build a string that contain Task statistics */
	/* header*/
	strncpy(str,"------------ Tasks statistics ---------------\r\n",50);
	str_len = strlen(str);
	/* statistics are appended */
	vTaskGetRunTimeStats(&str[str_len]); //48
	/* get the size */
	str_len = strlen(str);
	/* append separator */
	if (str_len < STAT_SIZE){
		strncat(str, "----------------------------------------------\r\n",
			STAT_SIZE - str_len - 1);
	}

	/* get Heap information */
	vPortGetHeapStats( &xHeapStats );
    /* get FreeRTOS heap statistic */
	str_len = strlen(str);
	if (str_len<STAT_SIZE){
		snprintf(&str[str_len],STAT_SIZE - str_len - 1,
				"Free Heap bytes : %d \r\n",
				xHeapStats.xAvailableHeapSpaceInBytes);
	}
    /* append separator */
	str_len = strlen(str);
	strncat(str, "----------------------------------------------\r\n",
			STAT_SIZE - str_len - 1);

#else
/* to do that manually (FreeRTOS example)*/
	TaskStatus_t *pxTaskStatusArray;
	volatile UBaseType_t uxArraySize, x,i;
	unsigned long ulTotalRunTime, ulStatsAsPercentage;
	/* Make sure the write buffer does not contain a string. */
	char * pcWriteBuffer = str;
	*pcWriteBuffer = 0x00;

	/* Take a snapshot of the number of tasks in case it changes while this
	function is executing. */
	uxArraySize = uxTaskGetNumberOfTasks();

	/* Allocate a TaskStatus_t structure for each task.  An array could be
	allocated statically at compile time. */
	pxTaskStatusArray = pvPortMalloc( uxArraySize * sizeof( TaskStatus_t ) );

	if( pxTaskStatusArray != NULL )
	{
	  /* Generate raw status information about each task. */
	  uxArraySize = uxTaskGetSystemState( pxTaskStatusArray,
								 uxArraySize,
								 &ulTotalRunTime );

	  /* For percentage calculations. */
	  ulTotalRunTime /= 100UL;

	  /* Avoid divide by zero errors. */
	  if( ulTotalRunTime > 0 )
	  {

		  /* Start by copying the entire string. */
		  strcpy( pcWriteBuffer, "Task name" );
		  /* Pad the end of the string with spaces to ensure columns line up when
		   * printed out. */
		  for(i = strlen( pcWriteBuffer ); i < ( size_t ) ( configMAX_TASK_NAME_LEN - 1 ); i++ )
		  {
			  pcWriteBuffer[ i ] = ' ';
		  }
		  /* Terminate. */
		  pcWriteBuffer[ i ] = ( char ) 0x00;

		  pcWriteBuffer += strlen( ( char * ) pcWriteBuffer );
		  strcpy( pcWriteBuffer, "\tCounter\t\t%\t\tRemaining stack\r\n");
		  pcWriteBuffer += strlen( ( char * ) pcWriteBuffer );

		 /* For each populated position in the pxTaskStatusArray array,
		 format the raw data as human readable ASCII data. */
		 for( x = 0; x < uxArraySize; x++ )
		 {
			/* What percentage of the total run time has the task used?
			This will always be rounded down to the nearest integer.
			ulTotalRunTimeDiv100 has already been divided by 100. */
			ulStatsAsPercentage =
				  pxTaskStatusArray[ x ].ulRunTimeCounter / ulTotalRunTime;

			/* Write the task name to the string, padding with
			 * spaces so it can be printed in tabular form more
			 * easily. */
			/* Start by copying the entire string. */
			strcpy( pcWriteBuffer, pxTaskStatusArray[ x ].pcTaskName);
			/* Pad the end of the string with spaces to ensure columns line up when
			* printed out. */
			for(i = strlen( pcWriteBuffer ); i < ( size_t ) ( configMAX_TASK_NAME_LEN - 1 ); i++ )
			{
			  pcWriteBuffer[ i ] = ' ';
			}
			/* Terminate. */
			pcWriteBuffer[ i ] = ( char ) 0x00;
			pcWriteBuffer += strlen( ( char * ) pcWriteBuffer );

			if( ulStatsAsPercentage > 0UL )
			{


				sprintf( pcWriteBuffer, "\t%lu\t\t%lu%%\t\t%lu\r\n",
								 pxTaskStatusArray[ x ].ulRunTimeCounter,
								 ulStatsAsPercentage,
								 pxTaskStatusArray[ x ].usStackHighWaterMark);
			}
			else
			{
			   /* If the percentage is zero here then the task has
			   consumed less than 1% of the total run time. */
			   sprintf( pcWriteBuffer, "\t%lu\t\t<1%%\t\t%lu\r\n",
								 pxTaskStatusArray[ x ].ulRunTimeCounter,
								 pxTaskStatusArray[ x ].usStackHighWaterMark );
			}

			pcWriteBuffer += strlen( ( char * ) pcWriteBuffer );
		 }
	  }

	  /* The array is no longer needed, free the memory it consumes. */
	  vPortFree( pxTaskStatusArray );
	}

#endif
	return strlen(str);

}


#if 0

/* Prototype of the vPortGetHeapStats() function. */
void vPortGetHeapStats( HeapStats_t *xHeapStats );

/* Definition of the Heap_stats_t structure. */
typedef struct xHeapStats
{
	size_t xAvailableHeapSpaceInBytes;      /* The total heap size currently available - this is the sum of all the free blocks, not the largest block that can be allocated. */
	size_t xSizeOfLargestFreeBlockInBytes; 	/* The maximum size, in bytes, of all the free blocks within the heap at the time vPortGetHeapStats() is called. */
	size_t xSizeOfSmallestFreeBlockInBytes; /* The minimum size, in bytes, of all the free blocks within the heap at the time vPortGetHeapStats() is called. */
	size_t xNumberOfFreeBlocks;		/* The number of free memory blocks within the heap at the time vPortGetHeapStats() is called. */
	size_t xMinimumEverFreeBytesRemaining;	/* The minimum amount of total free memory (sum of all free blocks) there has been in the heap since the system booted. */
	size_t xNumberOfSuccessfulAllocations;	/* The number of calls to pvPortMalloc() that have returned a valid memory block. */
	size_t xNumberOfSuccessfulFrees;	/* The number of calls to vPortFree() that has successfully freed a block of memory. */
} HeapStats_t;
#endif
/**
 * @brief  Setup the TIM5 to count (for FreeRTOS profiling),
 * @param  freq : frequency of counting
 * @retval None
 */
 void TIM5_Init(uint32_t freq)
 {
	TIM_TypeDef  * tim = TIM5;
	RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;		/* activate TIM5 Clock */

	/* prescaler value set to get a freq Hz clock */
	tim->PSC = (SystemCoreClock / freq) - 1 ;
	/* auto reload register set to its maximum value (32 bits) */
	tim->ARR = 0xffffffff;
	/* to enable change the in CCMR for CCxS bit the channel must be off */
	tim->CCER = 0;
	 	/* update counter and prescaler : it clear prescaler and counter, to start
	 * with a full cycle. It will set the Update flag  (UIF), note that it can
	 * be avoided by setting the UDIS bit in CR1
	 * see TIM5_clear_periodic_event() */
	tim->EGR = TIM_EGR_UG;
	/* As an update has been generated by software,
	 * clear the bit in the status register, all flags can also be cleared
	 * here
	 */
	tim->SR &= ~ TIM_SR_UIF ;
	/* enable the counter (start counting) */
	tim->CR1 |= TIM_CR1_CEN ;
 }

 uint32_t TIM5_get_cnt(void)
 {
	TIM_TypeDef  * tim = TIM5;
	return tim->CNT;

 }


