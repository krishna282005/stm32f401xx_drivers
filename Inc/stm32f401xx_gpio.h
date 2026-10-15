/*
 * stm32f401xx_gpio.h
 *
 *  Created on: 14-Oct-2026
 *      Author: Krishna
 */

#ifndef STM32F401XX_GPIO_H_
#define STM32F401XX_GPIO_H_

#include "stm32f401xx.h"

											//--------------//
											//  Pin Number  //
											//--------------//
typedef enum
{
    GPIO_PIN_0  = 0U,
    GPIO_PIN_1  = 1U,
    GPIO_PIN_2  = 2U,
    GPIO_PIN_3  = 3U,
    GPIO_PIN_4  = 4U,
    GPIO_PIN_5  = 5U,
    GPIO_PIN_6  = 6U,
    GPIO_PIN_7  = 7U,
    GPIO_PIN_8  = 8U,
    GPIO_PIN_9  = 9U,
    GPIO_PIN_10 = 10U,
    GPIO_PIN_11 = 11U,
    GPIO_PIN_12 = 12U,
    GPIO_PIN_13 = 13U,
    GPIO_PIN_14 = 14U,
    GPIO_PIN_15 = 15U

} GPIO_Pin_t;

											//--------------//
											//  Pin Modes   //
											//--------------//

typedef enum
{
	GPIO_MODE_INPUT  = 0U,
	GPIO_MODE_OUTPUT = 1U,
	GPIO_MODE_AF	 = 2U,
	GPIO_MODE_ANALOG = 3U

} GPIO_Mode_t;


										//----------------------//
										//  GPIO OUTPUT CONFIG  //
										//----------------------//

typedef enum
{
	GPIO_OUTPUT_PUSH_PULL  = 0U,
	GPIO_OUTPUT_OPEN_DRAIN = 1U

} GPIO_OutputType_t;


typedef enum
{
    GPIO_SPEED_LOW    = 0U,
    GPIO_SPEED_MEDIUM = 1U,
    GPIO_SPEED_HIGH   = 2U,
    GPIO_SPEED_VHIGH  = 3U

} GPIO_Speed_t;


typedef enum
{
    GPIO_NO_PULL   = 0U,
    GPIO_PULL_UP   = 1U,
    GPIO_PULL_DOWN = 2U

} GPIO_Pull_t;

											//-------------------//
											//  GPIO PIN CONFIG  //
											//-------------------//

typedef struct
{
	GPIO_Pin_t 			pin;
	GPIO_Mode_t 		mode;
	GPIO_OutputType_t 	output_type;
	GPIO_Speed_t 		speed;
	GPIO_Pull_t 		pull;
	uint8_t 			alternate_function;

} GPIO_PinConfig_t;


											//-------------------//
											//  GPIO PIN HANDLE  //
											//-------------------//

typedef struct
{
	GPIO_RegDef_t 		*port;
	GPIO_PinConfig_t 	config;

} GPIO_Handle_t;


												//-------------//
												//  GPIO APIs  //
												//-------------//


void GPIO_ClockEnable(GPIO_RegDef_t *port);
void GPIO_ClockDisable(GPIO_RegDef_t *port);

void GPIO_Init(GPIO_Handle_t *handle);
void GPIO_DeInit(GPIO_RegDef_t *port);

uint8_t GPIO_ReadPin(GPIO_RegDef_t *port, GPIO_Pin_t pin);
uint16_t GPIO_ReadPort(GPIO_RegDef_t *port);

void GPIO_WritePin(GPIO_RegDef_t *port, GPIO_Pin_t pin, uint8_t value);
void GPIO_WritePort(GPIO_RegDef_t *port, uint16_t value);

void GPIO_TogglePin(GPIO_RegDef_t *port, GPIO_Pin_t pin);

#endif /* STM32F401XX_GPIO_H_ */
