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

#endif /* PWM_H_ */
