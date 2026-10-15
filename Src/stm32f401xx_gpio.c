/*
 * stm32f401xx_gpio.c
 *
 *  Created on: 15-Oct-2026
 *      Author: Krishna
 */

#include <stddef.h>
#include "stm32f401xx.h"
#include "stm32f401xx_gpio.h"

void GPIO_ClockEnable(GPIO_RegDef_t *port)
{
	if (port == GPIOA)
	{
		RCC->AHB1ENR |= (1U << GPIOA_CLK_EN_BIT);
	}
	else if (port == GPIOB)
	{
		RCC->AHB1ENR |= (1U << GPIOB_CLK_EN_BIT);
	}
	else if (port == GPIOC)
	{
		RCC->AHB1ENR |= (1U << GPIOC_CLK_EN_BIT);
	}
	else if (port == GPIOD)
	{
		RCC->AHB1ENR |= (1U << GPIOD_CLK_EN_BIT);
	}
	else if (port == GPIOE)
	{
		RCC->AHB1ENR |= (1U << GPIOE_CLK_EN_BIT);
	}
	else if (port == GPIOH)
	{
		RCC->AHB1ENR |= (1U  << GPIOH_CLK_EN_BIT);
	}
}

void GPIO_ClockDisable(GPIO_RegDef_t *port)
{
	if (port == GPIOA)
	{
		RCC->AHB1ENR &= ~(1U << GPIOA_CLK_EN_BIT);
	}
	else if (port == GPIOB)
	{
		RCC->AHB1ENR &= ~(1U << GPIOB_CLK_EN_BIT);
	}
	else if (port == GPIOC)
	{
		RCC->AHB1ENR &= ~(1U << GPIOC_CLK_EN_BIT);
	}
	else if (port == GPIOD)
	{
		RCC->AHB1ENR &= ~(1U << GPIOD_CLK_EN_BIT);
	}
	else if (port == GPIOE)
	{
		RCC->AHB1ENR &= ~(1U << GPIOE_CLK_EN_BIT);
	}
	else if (port == GPIOH)
	{
		RCC->AHB1ENR &= ~(1U  << GPIOH_CLK_EN_BIT);
	}
}

void GPIO_Init(GPIO_Handle_t *handle)
{
	if (handle == NULL)
		return;

	if (handle->port != GPIOA &&
		handle->port != GPIOB &&
		handle->port != GPIOC &&
		handle->port != GPIOD &&
		handle->port != GPIOE &&
		handle->port != GPIOH)
	{
		return;
	}


	if ((uint32_t)handle->config.pin > 15U ||
	    (uint32_t)handle->config.mode > 3U ||
	    (uint32_t)handle->config.output_type > 1U ||
	    (uint32_t)handle->config.speed > 3U ||
	    (uint32_t)handle->config.pull > 2U)
	{
	    return;
	}

	if ((handle->config.mode == GPIO_MODE_AF) && (handle->config.alternate_function > 15U))
	{
		return;
	}


	GPIO_ClockEnable(handle->port);


	//------------------------------------- General initialization --------------------------//

	uint8_t pin = (uint8_t)handle->config.pin;

	handle->port->MODER &= ~(3U << (pin * 2U));
	handle->port->MODER |= ((uint32_t)handle->config.mode << (pin * 2U));

	handle->port->OTYPER &= ~(1U << pin);
	handle->port->OTYPER |= ((uint32_t)handle->config.output_type << pin);

	handle->port->OSPEEDR &= ~(3U << (pin * 2U));
	handle->port->OSPEEDR |= ((uint32_t)handle->config.speed << (pin * 2U));

	handle->port->PUPDR &= ~(3U << (pin * 2U));
	handle->port->PUPDR |= ((uint32_t)handle->config.pull << (pin * 2U));

	//------------------------------------- Setting Alternate Function ----------------------//

	if (handle->config.mode == GPIO_MODE_AF)
	{
		uint8_t af = handle->config.alternate_function;

		if (pin <= 7U) // pin 0 to 7
		{
			handle->port->AFRL &= ~(0xFU << (pin * 4U));
			handle->port->AFRL |= ((uint32_t)af << (pin * 4U));
		}
		else  // pin 8 to 15
		{
			uint8_t pin_af = pin - 8;

			handle->port->AFRH &= ~(0xFU << (pin_af * 4U));
			handle->port->AFRH |= ((uint32_t)af << (pin_af * 4U));
		}
	}

}

void GPIO_DeInit(GPIO_RegDef_t *port)
{
	if (port == GPIOA)
	{
		RCC->AHB1RSTR |= (1U << GPIOA_RESET_BIT);
		RCC->AHB1RSTR &= ~(1U << GPIOA_RESET_BIT);
	}
	else if (port == GPIOB)
	{
		RCC->AHB1RSTR |= (1U << GPIOB_RESET_BIT);
		RCC->AHB1RSTR &= ~(1U << GPIOB_RESET_BIT);
	}
	else if (port == GPIOC)
	{
		RCC->AHB1RSTR |= (1U << GPIOC_RESET_BIT);
		RCC->AHB1RSTR &= ~(1U << GPIOC_RESET_BIT);
	}
	else if (port == GPIOD)
	{
		RCC->AHB1RSTR |= (1U << GPIOD_RESET_BIT);
		RCC->AHB1RSTR &= ~(1U << GPIOD_RESET_BIT);
	}
	else if (port == GPIOE)
	{
		RCC->AHB1RSTR |= (1U << GPIOE_RESET_BIT);
		RCC->AHB1RSTR &= ~(1U << GPIOE_RESET_BIT);
	}
	else if (port == GPIOH)
	{
		RCC->AHB1RSTR |= (1U << GPIOH_RESET_BIT);
		RCC->AHB1RSTR &= ~(1U << GPIOH_RESET_BIT);
	}
}



//------------------------------------------ GPIO APIs --------------------------------------------

uint8_t GPIO_ReadPin(GPIO_RegDef_t *port, GPIO_Pin_t pin)
{
	if (port != GPIOA &&
		port != GPIOB &&
		port != GPIOC &&
		port != GPIOD &&
		port != GPIOE &&
		port != GPIOH)
	{
		return 0U;
	}

	if ((uint32_t)pin > 15U)
	{
		return 0U;
	}

	return ((uint8_t)(port->IDR >> pin) & 1U);
}

uint16_t GPIO_ReadPort(GPIO_RegDef_t *port)
{
	if (port != GPIOA &&
		port != GPIOB &&
		port != GPIOC &&
		port != GPIOD &&
		port != GPIOE &&
		port != GPIOH)
	{
		return 0U;
	}

	return ((uint16_t)port->IDR & 0xFFFFU);
}


void GPIO_WritePin(GPIO_RegDef_t *port, GPIO_Pin_t pin, uint8_t value)
{
	if (port != GPIOA &&
		port != GPIOB &&
		port != GPIOC &&
		port != GPIOD &&
		port != GPIOE &&
		port != GPIOH)
	{
		return;
	}

	if (((uint32_t)pin > 15U) || (value > 1U))
	{
		return;
	}

	if (value == 1U)
	{
		port->BSRR = (1U << pin);
	}
	else
	{
		port->BSRR = (1U << (16U + pin));
	}
}

void GPIO_WritePort(GPIO_RegDef_t *port, uint16_t value)
{
	if (port != GPIOA &&
		port != GPIOB &&
		port != GPIOC &&
		port != GPIOD &&
		port != GPIOE &&
		port != GPIOH)
	{
		return;
	}

	uint32_t set_mask = (uint32_t)value;
	uint32_t reset_mask = ((uint32_t)(~value) & 0xFFFFU) << 16U;

	port->BSRR = set_mask | reset_mask;
}

void GPIO_TogglePin(GPIO_RegDef_t *port, GPIO_Pin_t pin)
{
	if (port != GPIOA &&
		port != GPIOB &&
		port != GPIOC &&
		port != GPIOD &&
		port != GPIOE &&
		port != GPIOH)
	{
		return;
	}

	if ((uint32_t)pin > 15U)
	{
		return;
	}

	port->ODR ^= (1U << pin);
}
