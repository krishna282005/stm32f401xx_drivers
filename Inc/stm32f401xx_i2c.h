/*
 * stm32f401xx_i2c.h
 *
 *  Created on: 15-Oct-2026
 *      Author: Krishna
 */

#ifndef STM32F401XX_I2C_H_
#define STM32F401XX_I2C_H_

#include "stm32f401xx.h"


#define PE    (1U << 0)
#define START (1U << 8)
#define STOP  (1U << 9)
#define ACK   (1U << 10)
#define POS   (1U << 11)

#define SB    (1U << 0)
#define ADDR  (1U << 1)
#define BTF   (1U << 2)
#define STOPF (1U << 4)
#define RXNE  (1U << 6)
#define TXE   (1U << 7)
#define BERR  (1U << 8)
#define ARLO  (1U << 9)
#define AF    (1U << 10)

#define BUSY (1U << 1)

#define ITERREN (1U << 8)
#define ITEVTEN (1U << 9)
#define ITBUFEN (1U << 10)

#define FM         (1U << 15)
#define SM         (0U)
#define DUTY_2     (0U)
#define DUTY_16_9  (1U << 14)

#define I2C_POLL_TIMEOUT  1000000U
											//--------------//
											//  Speed mode  //
											//--------------//

typedef enum
{
    I2C_SCL_SPEED_SM = 100000U,
    I2C_SCL_SPEED_FM = 400000U

} I2C_SCLSpeed_t;


										//--------------------------//
										//  Acknowledgment control  //
										//--------------------------//

typedef enum
{
    I2C_ACK_DISABLE = 0U,
    I2C_ACK_ENABLE  = 1U

} I2C_AckControl_t;

											//----------------------//
											//  Duty Cycle control  //
											//----------------------//

typedef enum
{
    I2C_FM_DUTY_2    = 0U,
    I2C_FM_DUTY_16_9 = 1U

} I2C_FMDutyCycle_t;


												//-------------------//
												//  I2C STATUS CODE  //
												//-------------------//

typedef enum
{
    I2C_STATUS_OK = 0U,
    I2C_STATUS_ERR_BERR,
    I2C_STATUS_ERR_ARLO,
    I2C_STATUS_ERR_AF,
    I2C_STATUS_ERR_INVALID,
    I2C_STATUS_ERR_TIMEOUT,
    I2C_STATUS_ERR_BUSY

} I2C_Status_t;

												//--------------//
												//  I2C CONFIG  //
												//--------------//

typedef struct
{
    I2C_SCLSpeed_t    I2C_SCLSpeed;
    I2C_AckControl_t  I2C_AckControl;
    I2C_FMDutyCycle_t I2C_FMDutyCycle;

    uint8_t           I2C_DeviceAddress;
    uint32_t          I2C_PCLK1Frequency;

} I2C_Config_t;

											//----------------------//
											//  I2C Transfer State  //
											//----------------------//


typedef enum
{
    I2C_STATE_IDLE = 0U,
    I2C_STATE_MASTER_TX,
    I2C_STATE_MASTER_RX,
    I2C_STATE_SLAVE_TX,
    I2C_STATE_SLAVE_RX

} I2C_State_t;

											//------------------//
											//  I2C PIN HANDLE  //
											//------------------//

typedef struct
{
    I2C_RegDef_t *pI2Cx;
    I2C_Config_t  I2C_Config;

    const uint8_t *pTxBuffer;
    uint8_t       *pRxBuffer;

    uint32_t TxLength;
    uint32_t RxLength;
    uint32_t TxIndex;
    uint32_t RxIndex;

    uint8_t SlaveAddress;

    volatile I2C_State_t  State;
    volatile I2C_Status_t Status;

} I2C_Handle_t;


												//------------//
												//  I2C APIs  //
												//------------//



void I2C_ClockEnable(I2C_RegDef_t *pI2Cx);
void I2C_ClockDisable(I2C_RegDef_t *pI2Cx);

I2C_Status_t I2C_Init(I2C_Handle_t *pI2CHandle);
I2C_Status_t I2C_DeInit(I2C_RegDef_t *pI2Cx);

void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx, uint8_t enable);
void I2C_ManageAcking(I2C_RegDef_t *pI2Cx, uint8_t enable);


I2C_Status_t I2C_MasterSendData(I2C_Handle_t *pI2CHandle, const uint8_t *pTxBuffer,uint32_t length, uint8_t slaveAddress);
I2C_Status_t I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint32_t length, uint8_t slaveAddress);

I2C_Status_t I2C_TargetTransmitData(I2C_Handle_t *pI2CHandle, const uint8_t *pTxBuffer, uint32_t length);
I2C_Status_t I2C_TargetReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint32_t length);

I2C_Status_t I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle, const uint8_t *pTxBuffer, uint32_t length, uint8_t slaveAddress);
I2C_Status_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint32_t length, uint8_t slaveAddress);


											//------------------//
											//  I2C Interrupts  //
											//------------------//


void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle);
void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle);

void I2C_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi);


#endif /* STM32F401XX_I2C_H_ */
