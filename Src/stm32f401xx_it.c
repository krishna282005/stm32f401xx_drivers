/*
 * stm32f401xx_it.c
 *
 *  Created on: 16-Oct-2026
 *      Author: Krishna
 */

#include "stm32f401xx_it.h"
#include "stm32f401xx_i2c.h"

extern I2C_Handle_t I2C1Handle;

void I2C1_EV_IRQHandler(void)
{
    I2C_EV_IRQHandling(&I2C1Handle);
}

void I2C1_ER_IRQHandler(void)
{
    I2C_ER_IRQHandling(&I2C1Handle);
}
