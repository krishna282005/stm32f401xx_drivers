/*
 * stm32f401xx_it.h
 *
 *  Created on: 16-Oct-2026
 *      Author: Krishna
 */

#ifndef STM32F401XX_IT_H_
#define STM32F401XX_IT_H_


#include "stm32f401xx_i2c.h"

extern I2C_Handle_t I2C1Handle;

void I2C1_EV_IRQHandler(void);

void I2C1_ER_IRQHandler(void);


#endif /* STM32F401XX_IT_H_ */
