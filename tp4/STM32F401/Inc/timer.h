/*
 * timer.h
 *
 *  Created on: 4 avr. 2022
 *      Author: gschwab
 */

#ifndef TIMER_H_
#define TIMER_H_
#include "stm32f401xe.h"
#include "stm32f4xx.h"
#include <stdint.h>
#include "pwm.h"
#include "usart.h"



char buffer[256];
float duration;
uint16_t begin;
uint16_t end;
uint16_t nb_overflows;

void TIM3_set_periodic_IRQ(uint32_t ms);
void TIM3_IRQHandler(void);

void TIM4_set_periodic_IRQ(uint32_t ms);
void TIM4_IRQHandler(void);

#endif /* TIMER_H_ */
