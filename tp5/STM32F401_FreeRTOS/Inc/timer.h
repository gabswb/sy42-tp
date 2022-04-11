#ifndef INC_TIMER_H_
#define INC_TIMER_H_

void TIM3_set_periodic_event(uint32_t ms);
void TIM3_wait_periodic_event(void);
void TIM3_clear_periodic_event(void);
uint32_t TIM3_test_periodic_event(void);

void TIM3_set_periodic_interrupt(uint32_t ms, void (*callback)(void));
void TIM3_remove_periodic_interrupt(void);

void TIM2_init_pwm(void);
void TIM2_set_pwm(uint32_t duty);
uint32_t TIM2_get_pwm(void);

void TIM4_init(uint32_t frequ);
uint32_t TIM4_pulse_capture(void);
int32_t TIM4_test_pulse_capture(void);
uint32_t TIM4_get_pulse(void);


void TIM5_Init(uint32_t freq);
uint32_t TIM5_get_cnt(void);

#endif /* INC_TIMER_H_ */
