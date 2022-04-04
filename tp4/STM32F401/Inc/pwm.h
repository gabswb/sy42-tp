/*
 * pwm.h
 *
 *  Created on: Mar 31, 2022
 *      Author: gabriels
 */

#ifndef PWM_H_
#define PWM_H_

#include "stdint.h"

void TIM2_init_PWM(void);
void TIM2_set_PWM(uint32_t duty);

float compute_duration(uint16_t begin, uint16_t end, uint16_t nb_overflows, int arr, int freq);

#endif /* PWM_H_ */
