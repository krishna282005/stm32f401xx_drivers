/*
 * stm32f401xx.h
 *
 *  Created on: 14-Oct-2026
 *      Author: Krishna
 */

#ifndef STM32F401XX_H_
#define STM32F401XX_H_

#include <stdint.h>

typedef struct
{
	volatile uint32_t CR;
	volatile uint32_t PLLCFGR;
	volatile uint32_t CFGR;
	volatile uint32_t CIR;
	volatile uint32_t AHB1RSTR;
	volatile uint32_t AHB2RSTR;
	uint32_t reserved0[2];
	volatile uint32_t APB1RSTR;
	volatile uint32_t APB2RSTR;
	uint32_t reserved1[2];
	volatile uint32_t AHB1ENR;
	volatile uint32_t AHB2ENR;
	uint32_t reserved2[2];
	volatile uint32_t APB1ENR;
	volatile uint32_t APB2ENR;
	uint32_t reserved3[2];
	volatile uint32_t AHB1LPENR;
	volatile uint32_t AHB2LPENR;
	uint32_t reserved4[2];
	volatile uint32_t APB1LPENR;
	volatile uint32_t APB2LPENR;
	uint32_t reserved5[2];
	volatile uint32_t BDCR;
	volatile uint32_t CSR;
	uint32_t reserved6[2];
	volatile uint32_t SSCGR;
	volatile uint32_t PLLI2SCFGR;
	uint32_t reserved7[2];
	volatile uint32_t DCKCFGR;
}RCC_RegDef_t;

typedef struct
{
	volatile uint32_t MODER;
	volatile uint32_t OTYPER;
	volatile uint32_t OSPEEDR;
	volatile uint32_t PUPDR;
	volatile uint32_t IDR;
	volatile uint32_t ODR;
	volatile uint32_t BSRR;
	volatile uint32_t LCKR;
	volatile uint32_t AFRL;
	volatile uint32_t AFRH;
}GPIO_RegDef_t;

typedef struct
{
	volatile uint32_t CR1;
	volatile uint32_t CR2;
	volatile uint32_t OAR1;
	volatile uint32_t OAR2;
	volatile uint32_t DR;
	volatile uint32_t SR1;
	volatile uint32_t SR2;
	volatile uint32_t CCR;
	volatile uint32_t TRISE;
	volatile uint32_t FLTR;
}I2C_RegDef_t;

//////////////////////// Peripheral address ////////////////////////////////////////////////////////

#define PERIPH_BASE     	0x40000000UL

#define APB1PERIPH_BASE 	0x40000000UL

#define AHB1PERIPH_BASE     0x40020000UL

#define RCC_BASEADDR		0x40023800UL

#define GPIOA_BASEADDR 		(AHB1PERIPH_BASE + 0x00000000UL)
#define GPIOB_BASEADDR 		(AHB1PERIPH_BASE + 0x00000400UL)
#define GPIOC_BASEADDR 		(AHB1PERIPH_BASE + 0x00000800UL)
#define GPIOD_BASEADDR 		(AHB1PERIPH_BASE + 0x00000C00UL)
#define GPIOE_BASEADDR 		(AHB1PERIPH_BASE + 0x00001000UL)
#define GPIOH_BASEADDR 		(AHB1PERIPH_BASE + 0x00001C00UL)

#define I2C1_BASEADDR    	(APB1PERIPH_BASE + 0x00005400UL)
#define I2C2_BASEADDR    	(APB1PERIPH_BASE + 0x00005800UL)
#define I2C3_BASEADDR    	(APB1PERIPH_BASE + 0x00005C00UL)

/////////////////////// Peripheral pointers ///////////////////////////////////////////////////////

#define RCC					((RCC_RegDef_t *)RCC_BASEADDR)

#define GPIOA 				((GPIO_RegDef_t *)GPIOA_BASEADDR)
#define GPIOB 				((GPIO_RegDef_t *)GPIOB_BASEADDR)
#define GPIOC 				((GPIO_RegDef_t *)GPIOC_BASEADDR)
#define GPIOD 				((GPIO_RegDef_t *)GPIOD_BASEADDR)
#define GPIOE 				((GPIO_RegDef_t *)GPIOE_BASEADDR)
#define GPIOH 				((GPIO_RegDef_t *)GPIOH_BASEADDR)

#define I2C1				((I2C_RegDef_t *)I2C1_BASEADDR)
#define I2C2				((I2C_RegDef_t *)I2C1_BASEADDR)
#define I2C3				((I2C_RegDef_t *)I2C1_BASEADDR)

//////////////////////// CLK-GPIO MACRO /////////////////////////////////////////////////////////

#define GPIOA_CLK_EN_BIT 0U
#define GPIOB_CLK_EN_BIT 1U
#define GPIOC_CLK_EN_BIT 2U
#define GPIOD_CLK_EN_BIT 3U
#define GPIOE_CLK_EN_BIT 4U
#define GPIOH_CLK_EN_BIT 7U

#define GPIOA_RESET_BIT  0U
#define GPIOB_RESET_BIT  1U
#define GPIOC_RESET_BIT  2U
#define GPIOD_RESET_BIT  3U
#define GPIOE_RESET_BIT  4U
#define GPIOH_RESET_BIT  7U

#define I2C1_CLK_EN_BIT  21U
#define I2C2_CLK_EN_BIT  22U
#define I2C3_CLK_EN_BIT  23U

#endif /* STM32F401XX_H_ */
