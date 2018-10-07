

#ifndef UTILS_H
#define UTILS_H


void wait_1ms(uint16_t u16Factor);
void INT_1ms_init(void);
void TIM3_IRQHandler(void);
void SysTick_Handler(void);
uint32_t get_sys_tick(void);

#endif
