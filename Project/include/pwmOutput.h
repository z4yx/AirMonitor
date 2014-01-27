#ifndef __PWM_OUTPUT_H
#define	__PWM_OUTPUT_H

void TIMx_Config(TIM_TypeDef* TIMx, uint16_t period, uint16_t prescaler);
void TIMx_OCx_Config(TIM_TypeDef* TIMx, int OCx, uint16_t pulse, FunctionalState output);
void TIM2_SetInterrupt(FunctionalState state);
void PWM_TIM2_Interrupt(void);

#endif
