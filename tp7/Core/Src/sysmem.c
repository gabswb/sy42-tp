/**
 ******************************************************************************
 * @file      sysmem.c
 * @author    Nicolas Lacaille
 * @brief     System Memory calls for FreeRTOS
 ******************************************************************************
  */

/* Includes */
#include <errno.h>
#include <stdint.h>
#include <errno.h>
#include "FreeRTOS.h"
#include "task.h"
#include "stm32f4xx.h"


/* Functions */

#if 1
/* wrap malloc on pvPortMalloc */

void * _malloc_r(struct _reent *reent, size_t nbytes) {
     return pvPortMalloc(nbytes);
};


void _free_r(struct _reent *reent,void* ptr)
{
        // We simply wrap the FreeRTOS call into a standard form
        vPortFree(ptr);
}

void *_realloc_r(struct _reent *reent, void *aptr, size_t nbytes)
{
	/* no equivalent function in free RTOS, some newlib functions use it, maybe better to trap
	 * it to debug  */
	return NULL;
}
#else
/* otherwise lock scheduller for _malloc, _free, _sbrk, ... */
void __malloc_lock()     {       vTaskSuspendAll(); };
void __malloc_unlock()   { (void)xTaskResumeAll();  };

/**
 _sbrk
 Increase program data space. Malloc and related functions depend on this
(if incr is negative _sbrk should free memory blocks)
 for FreeRTOS, we use the total space allocated @ link between
 .end (bss) and .end + _Min_Heap_Size
 or .end (bss) and _estack -  _Min_Stack_Size
 for the reent structure use
 #define configUSE_NEWLIB_REENTRANT              1
 in FreeRTOSConfig.h
**/

/**
 * Pointer to the current high watermark of the heap usage
 */
static uint8_t *__sbrk_heap_end = NULL;

/**
 * @brief _sbrk() allocates memory to the newlib heap and is used by malloc
 *        and others from the C library
 *
 * @verbatim
 * ############################################################################
 * #  .data  #  .bss  #       newlib heap       #          MSP stack          #
 * #         #        #                         # Reserved by _Min_Stack_Size #
 * ############################################################################
 * ^-- RAM start      ^-- _end                             _estack, RAM end --^
 * @endverbatim
 *
 * This implementation starts allocating at the '_end' linker symbol
 * The '_Min_Stack_Size' linker symbol reserves a memory for the MSP stack
 * The implementation considers '_estack' linker symbol to be RAM end
 * NOTE: If the MSP stack, at any point during execution, grows larger than the
 * reserved size, please increase the '_Min_Stack_Size'.
 *
 * @param incr Memory size
 * @return Pointer to allocated memory
 */
void *_sbrk(ptrdiff_t incr)
{
  extern uint8_t _end; /* Symbol defined in the linker script */
  extern uint8_t _estack; /* Symbol defined in the linker script */
  extern uint32_t _Min_Stack_Size; /* Symbol defined in the linker script */
  const uint32_t stack_limit = (uint32_t)&_estack - (uint32_t)&_Min_Stack_Size;
  const uint8_t *max_heap = (uint8_t *)stack_limit;
  uint8_t *prev_heap_end;

  /* Initialize heap end at first call */
  if (NULL == __sbrk_heap_end)
  {
    __sbrk_heap_end = &_end;
  }

  /* Protect heap from growing into the reserved MSP stack */
  if (__sbrk_heap_end + incr > max_heap)
  {
    errno = ENOMEM;
    return (void *)-1;
  }

  prev_heap_end = __sbrk_heap_end;
  __sbrk_heap_end += incr;

  return (void *)prev_heap_end;
}
#endif
