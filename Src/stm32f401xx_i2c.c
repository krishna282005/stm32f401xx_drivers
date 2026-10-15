/*
 * stm32f401xx_i2c.c
 *
 *  Created on: 16-Oct-2026
 *      Author: heerc
 */

#include <stddef.h>
#include "stm32f401xx.h"
#include "stm32f401xx_i2c.h"

static I2C_Status_t I2C_WaitForFlag(I2C_RegDef_t *pI2Cx, uint32_t flag);

void I2C_ClockEnable(I2C_RegDef_t *pI2Cx)
{
	if (pI2Cx == I2C1)
	{
		RCC->APB1ENR |= (1U << I2C1_CLK_EN_BIT);
	}
	else if (pI2Cx == I2C2)
	{
		RCC->APB1ENR |= (1U << I2C2_CLK_EN_BIT);
	}
	else if (pI2Cx == I2C3)
	{
		RCC->APB1ENR |= (1U << I2C3_CLK_EN_BIT);
	}
}

void I2C_ClockDisable(I2C_RegDef_t *pI2Cx)
{
	if (pI2Cx == I2C1)
	{
		RCC->APB1ENR &= ~(1U << I2C1_CLK_EN_BIT);
	}
	else if (pI2Cx == I2C2)
	{
		RCC->APB1ENR &= ~(1U << I2C2_CLK_EN_BIT);
	}
	else if (pI2Cx == I2C3)
	{
		RCC->APB1ENR &= ~(1U << I2C3_CLK_EN_BIT);
	}
}

I2C_Status_t I2C_Init(I2C_Handle_t *pI2CHandle)
{
	I2C_RegDef_t *pI2Cx;
	uint32_t pclk1;
	uint32_t freqMHz;
	uint32_t ccrValue;
	uint32_t triseValue;

	//----------------- Check for all invalid possibilities ---------
	if (pI2CHandle == NULL)
	{
		return I2C_STATUS_ERR_INVALID;
	}

	pI2Cx = pI2CHandle->pI2Cx;

	if ((pI2Cx != I2C1) &&
		(pI2Cx != I2C2) &&
		(pI2Cx != I2C3))
	{
		return I2C_STATUS_ERR_INVALID;
	}

	pclk1 = pI2CHandle->I2C_Config.I2C_PCLK1Frequency;

    if ((pclk1 < 2000000U)  ||
        (pclk1 > 50000000U) ||
        ((pclk1 % 1000000U) != 0U))
    {
        return I2C_STATUS_ERR_INVALID;
    }

    if(pI2CHandle->I2C_Config.I2C_DeviceAddress > 0x7FU)
    {
    	return I2C_STATUS_ERR_INVALID;
    }

    if ((pI2CHandle->I2C_Config.I2C_FMDutyCycle != I2C_FM_DUTY_2) &&
        (pI2CHandle->I2C_Config.I2C_FMDutyCycle != I2C_FM_DUTY_16_9))
    {
        return I2C_STATUS_ERR_INVALID;
    }

    if ((pI2CHandle->I2C_Config.I2C_SCLSpeed != I2C_SCL_SPEED_SM) &&
    	(pI2CHandle->I2C_Config.I2C_SCLSpeed != I2C_SCL_SPEED_FM))
    {
    	return I2C_STATUS_ERR_INVALID;
    }

    if ((pI2CHandle->I2C_Config.I2C_AckControl != I2C_ACK_ENABLE) &&
    	(pI2CHandle->I2C_Config.I2C_AckControl != I2C_ACK_DISABLE))
    {
    	return I2C_STATUS_ERR_INVALID;
    }

    //------------------------------- Enable the peripheral after verification-----------------

    I2C_ClockEnable(pI2Cx);

    //------------------------------ Disable driver for configuring it ----------------

    pI2Cx->CR1 &= ~PE;

    freqMHz = pclk1/1000000U;

    pI2Cx->CR2 = 0U;
    pI2Cx->CR2 = freqMHz;

    if (pI2CHandle->I2C_Config.I2C_SCLSpeed == I2C_SCL_SPEED_SM)
    {
    	ccrValue = pclk1/(2U * I2C_SCL_SPEED_SM);
    	triseValue = freqMHz + 1;
    }
    else
    {
    	if (pI2CHandle->I2C_Config.I2C_FMDutyCycle == I2C_FM_DUTY_2)
    	{
    		ccrValue = pclk1/(3U * I2C_SCL_SPEED_FM);
    	}
    	else
    	{
    		ccrValue = pclk1/(25 * I2C_SCL_SPEED_FM);
    	}

    	triseValue = ((freqMHz * 300U)/1000U) + 1U;
    }

    if ((ccrValue == 0U)||(ccrValue > 0xFFFU))
    {
    	return I2C_STATUS_ERR_INVALID;
    }

    pI2Cx->CCR = 0U;

    if (pI2CHandle->I2C_Config.I2C_SCLSpeed == I2C_SCL_SPEED_FM)
    {
    	pI2Cx->CCR |= FM;
    	if (pI2CHandle->I2C_Config.I2C_FMDutyCycle == I2C_FM_DUTY_16_9)
    	{
    		pI2Cx->CCR |= DUTY_16_9;
    	}
    }

    pI2Cx->CCR |= ccrValue;

    pI2Cx->TRISE = triseValue;

    pI2Cx->OAR1 = (1U << 14) | (pI2CHandle->I2C_Config.I2C_DeviceAddress << 1);

    if (pI2CHandle->I2C_Config.I2C_AckControl == I2C_ACK_ENABLE)
    {
    	pI2Cx->CR1 |= ACK;
    }
    else if (pI2CHandle->I2C_Config.I2C_AckControl == I2C_ACK_DISABLE)
    {
    	pI2Cx->CR1 &= ~ACK;
    }

    pI2Cx->CR1 |= PE;

    pI2CHandle->State = I2C_STATE_IDLE;
    pI2CHandle->Status = I2C_STATUS_OK;

    return I2C_STATUS_OK;

}

I2C_Status_t I2C_DeInit(I2C_RegDef_t *pI2Cx)
{
    if (pI2Cx == NULL)
    {
        return I2C_STATUS_ERR_INVALID;
    }

    if (pI2Cx == I2C1)
    {
        RCC->APB1RSTR |= (1U << 21);
        RCC->APB1RSTR &= ~(1U << 21);
    }
    else if (pI2Cx == I2C2)
    {
        RCC->APB1RSTR |= (1U << 22);
        RCC->APB1RSTR &= ~(1U << 22);
    }
    else if (pI2Cx == I2C3)
    {
        RCC->APB1RSTR |= (1U << 23);
        RCC->APB1RSTR &= ~(1U << 23);
    }
    else
    {
        return I2C_STATUS_ERR_INVALID;
    }

    return I2C_STATUS_OK;
}

void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx, uint8_t enable)
{
    if ((pI2Cx != I2C1) &&
        (pI2Cx != I2C2) &&
        (pI2Cx != I2C3))
    {
        return;
    }

    if (enable != 0U)
    {
        pI2Cx->CR1 |= PE;
    }
    else
    {
        pI2Cx->CR1 &= ~PE;
    }
}

void I2C_ManageAcking(I2C_RegDef_t *pI2Cx, uint8_t enable)
{
    if ((pI2Cx != I2C1) &&
        (pI2Cx != I2C2) &&
        (pI2Cx != I2C3))
    {
        return;
    }

    if (enable != 0U)
    {
        pI2Cx->CR1 |= ACK;
    }
    else
    {
        pI2Cx->CR1 &= ~ACK;
    }
}


I2C_Status_t I2C_MasterSendData(I2C_Handle_t *pI2CHandle, const uint8_t *pTxBuffer, uint32_t length, uint8_t slaveAddress)
{
    I2C_RegDef_t *pI2Cx;
    I2C_Status_t status;
    uint32_t i;

    //------------------------------------Checking validity-------------------------------------

    if ((pI2CHandle == NULL) ||
        (pTxBuffer == NULL)  ||
        (length == 0U)       ||
        (slaveAddress > 0x7FU))  //means slave address greater than 7 bits
    {
        return I2C_STATUS_ERR_INVALID;
    }

    pI2Cx = pI2CHandle->pI2Cx;

    if ((pI2Cx != I2C1) &&
        (pI2Cx != I2C2) &&
        (pI2Cx != I2C3))
    {
        return I2C_STATUS_ERR_INVALID;
    }

    if (pI2CHandle->State != I2C_STATE_IDLE)
    {
        return I2C_STATUS_ERR_BUSY;
    }

    if ((pI2Cx->SR2 & BUSY) != 0U)
    {
        return I2C_STATUS_ERR_BUSY;
    }

    //-----------------------------------------------------------------------------------------

    pI2CHandle->State = I2C_STATE_MASTER_TX;
    pI2CHandle->Status = I2C_STATUS_OK;

    pI2CHandle->pTxBuffer = pTxBuffer;
    pI2CHandle->TxLength = length;
    pI2CHandle->TxIndex = 0U;
    pI2CHandle->SlaveAddress = slaveAddress;

    pI2Cx->CR1 |= START; //start peripheral

    status = I2C_WaitForFlag(pI2Cx, SB);

    if (status != I2C_STATUS_OK)
    {
        goto error;
    }

    pI2Cx->DR = (uint32_t)(slaveAddress << 1); //set address

    status = I2C_WaitForFlag(pI2Cx, ADDR);

    if (status != I2C_STATUS_OK)
    {
        goto error;
    }

    // clear ADDR as address successfully sent/matched/validated
    (void)pI2Cx->SR1;
    (void)pI2Cx->SR2;

    // now only transmit data
    for (i = 0U; i < length; i++)
    {
        status = I2C_WaitForFlag(pI2Cx, TXE);

        if (status != I2C_STATUS_OK)
        {
            goto error;
        }

        pI2Cx->DR = pTxBuffer[i];
        pI2CHandle->TxIndex = i + 1U;
    }

    // Wait until the final byte has completed
    status = I2C_WaitForFlag(pI2Cx, BTF);

    if (status != I2C_STATUS_OK)
    {
        goto error;
    }

    // Generate STOP condition
    pI2Cx->CR1 |= STOP;

    pI2CHandle->State = I2C_STATE_IDLE;
    pI2CHandle->Status = I2C_STATUS_OK;

    return I2C_STATUS_OK;

error:

    if (status != I2C_STATUS_ERR_ARLO)
    {
        pI2Cx->CR1 |= STOP;
    }

    pI2CHandle->State = I2C_STATE_IDLE;
    pI2CHandle->Status = status;

    return status;
}

I2C_Status_t I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint32_t length, uint8_t slaveAddress)
{
    I2C_RegDef_t *pI2Cx;
    I2C_Status_t status;
    uint32_t remaining;

    //------------------------------------Checking validity-------------------------------------

    if ((pI2CHandle == NULL) ||
        (pRxBuffer == NULL)  ||
        (length == 0U)       ||
        (slaveAddress > 0x7FU))
    {
        return I2C_STATUS_ERR_INVALID;
    }

    pI2Cx = pI2CHandle->pI2Cx;

    if ((pI2Cx != I2C1) &&
        (pI2Cx != I2C2) &&
        (pI2Cx != I2C3))
    {
        return I2C_STATUS_ERR_INVALID;
    }

    if (pI2CHandle->State != I2C_STATE_IDLE)
    {
        return I2C_STATUS_ERR_BUSY;
    }

    if ((pI2Cx->SR2 & BUSY) != 0U)
    {
        return I2C_STATUS_ERR_BUSY;
    }

    //-----------------------------------------------------------------------------------------

    pI2CHandle->State = I2C_STATE_MASTER_RX;
    pI2CHandle->Status = I2C_STATUS_OK;

    pI2CHandle->pRxBuffer = pRxBuffer;
    pI2CHandle->RxLength = length;
    pI2CHandle->RxIndex = 0U;
    pI2CHandle->SlaveAddress = slaveAddress;

    // Reset POS and enable ACK before starting reception
    pI2Cx->CR1 &= ~POS;
    pI2Cx->CR1 |= ACK;

    // Generate START condition
    pI2Cx->CR1 |= START;

    status = I2C_WaitForFlag(pI2Cx, SB);

    if (status != I2C_STATUS_OK)
    {
        goto error;
    }

    pI2Cx->DR = ((uint32_t)slaveAddress << 1) | 1U; // Send slave address with the READ bit set

    status = I2C_WaitForFlag(pI2Cx, ADDR);

    if (status != I2C_STATUS_OK)
    {
        goto error;
    }

    //------------------------------------Receiving data-----------------------------------------

    if (length == 1U)   // for receiving 1-byte
    {
        // Disable ACK first and clear ADDR
        pI2Cx->CR1 &= ~ACK;

        (void)pI2Cx->SR1;
        (void)pI2Cx->SR2;

        // Generate STOP before reading the received byte
        pI2Cx->CR1 |= STOP;

        status = I2C_WaitForFlag(pI2Cx, RXNE);

        if (status != I2C_STATUS_OK)
        {
            goto error;
        }

        *pRxBuffer = (uint8_t)pI2Cx->DR;
        pI2CHandle->RxIndex = 1U;
    }
    else if (length == 2U) // for receiving 2-bytes
    {
        pI2Cx->CR1 |= POS;
        pI2Cx->CR1 &= ~ACK;

        (void)pI2Cx->SR1;
        (void)pI2Cx->SR2;

        status = I2C_WaitForFlag(pI2Cx, BTF);

        if (status != I2C_STATUS_OK)
        {
            goto error;
        }

        // Generate STOP and read both bytes
        pI2Cx->CR1 |= STOP;

        pRxBuffer[0] = (uint8_t)pI2Cx->DR;
        pRxBuffer[1] = (uint8_t)pI2Cx->DR;

        pI2CHandle->RxIndex = 2U;
    }
    else  // for receiving n-bytes
    {
        // Clear ADDR
        (void)pI2Cx->SR1;
        (void)pI2Cx->SR2;

        remaining = length;

        // Receive bytes until three bytes remain
        while (remaining > 3U)
        {
            status = I2C_WaitForFlag(pI2Cx, RXNE);

            if (status != I2C_STATUS_OK)
            {
                goto error;
            }

            *pRxBuffer++ = (uint8_t)pI2Cx->DR;
            pI2CHandle->RxIndex++;
            remaining--;
        }

        // Wait until the final three bytes are ready
        status = I2C_WaitForFlag(pI2Cx, BTF);

        if (status != I2C_STATUS_OK)
        {
            goto error;
        }

        pI2Cx->CR1 &= ~ACK;

        *pRxBuffer++ = (uint8_t)pI2Cx->DR;
        pI2CHandle->RxIndex++;
        remaining--;

        status = I2C_WaitForFlag(pI2Cx, BTF);

        if (status != I2C_STATUS_OK)
        {
            goto error;
        }

        // Generate STOP before reading the final two bytes
        pI2Cx->CR1 |= STOP;

        *pRxBuffer++ = (uint8_t)pI2Cx->DR;
        pI2CHandle->RxIndex++;

        *pRxBuffer = (uint8_t)pI2Cx->DR;
        pI2CHandle->RxIndex++;
    }

    // Restore default ACK/POS configuration
    pI2Cx->CR1 &= ~POS;
    pI2Cx->CR1 |= ACK;

    pI2CHandle->State = I2C_STATE_IDLE;
    pI2CHandle->Status = I2C_STATUS_OK;

    return I2C_STATUS_OK;

error:

    if (status != I2C_STATUS_ERR_ARLO)
    {
        pI2Cx->CR1 |= STOP;
    }

    // Restore ACK/POS configuration
    pI2Cx->CR1 &= ~POS;
    pI2Cx->CR1 |= ACK;

    pI2CHandle->State = I2C_STATE_IDLE;
    pI2CHandle->Status = status;

    return status;
}

I2C_Status_t I2C_TargetTransmitData(I2C_Handle_t *pI2CHandle, const uint8_t *pTxBuffer, uint32_t length)
{
    I2C_RegDef_t *pI2Cx;
    I2C_Status_t status;
    uint32_t i;
    uint32_t timeout;

    //------------------------------------Checking validity-------------------------------------

    if ((pI2CHandle == NULL) ||
        (pTxBuffer == NULL)  ||
        (length == 0U))
    {
        return I2C_STATUS_ERR_INVALID;
    }

    pI2Cx = pI2CHandle->pI2Cx;

    if ((pI2Cx != I2C1) &&
        (pI2Cx != I2C2) &&
        (pI2Cx != I2C3))
    {
        return I2C_STATUS_ERR_INVALID;
    }

    if (pI2CHandle->State != I2C_STATE_IDLE)
    {
        return I2C_STATUS_ERR_BUSY;
    }

    //-----------------------------------------------------------------------------------------

    pI2CHandle->State = I2C_STATE_SLAVE_TX;
    pI2CHandle->Status = I2C_STATUS_OK;

    pI2CHandle->pTxBuffer = pTxBuffer;
    pI2CHandle->TxLength = length;
    pI2CHandle->TxIndex = 0U;

    //------------------------------------Waiting for ADDR--------------------------------------

    // Wait until the master addresses this device for transmission
    status = I2C_WaitForFlag(pI2Cx, ADDR);

    if (status != I2C_STATUS_OK)
    {
        goto error;
    }

    // Clear ADDR
    (void)pI2Cx->SR1;
    (void)pI2Cx->SR2;

    //------------------------------------Transmitting data-------------------------------------

    for (i = 0U; i < length; i++)
    {
        status = I2C_WaitForFlag(pI2Cx, TXE); //wait for txe==1

        if (status != I2C_STATUS_OK)
        {
            goto error;
        }

        pI2Cx->DR = pTxBuffer[i];

        pI2CHandle->TxIndex = i + 1U;
    }

    timeout = I2C_POLL_TIMEOUT;

    while (1)
    {
        if ((pI2Cx->SR1 & STOPF) != 0U)
        {
            // Clear STOPF: read SR1, then write CR1
            (void)pI2Cx->SR1;
            pI2Cx->CR1 = pI2Cx->CR1;

            break;
        }

        if ((pI2Cx->SR1 & AF) != 0U)
        {
            pI2Cx->SR1 &= ~AF;
        }

        if ((pI2Cx->SR1 & BERR) != 0U)
        {
            pI2Cx->SR1 &= ~BERR;
            status = I2C_STATUS_ERR_BERR;
            goto error;
        }

        if ((pI2Cx->SR1 & ARLO) != 0U)
        {
            pI2Cx->SR1 &= ~ARLO;
            status = I2C_STATUS_ERR_ARLO;
            goto error;
        }

        if (--timeout == 0U)
        {
            status = I2C_STATUS_ERR_TIMEOUT;
            goto error;
        }
    }


    pI2CHandle->State = I2C_STATE_IDLE;
    pI2CHandle->Status = I2C_STATUS_OK;

    return I2C_STATUS_OK;

error:

    pI2CHandle->State = I2C_STATE_IDLE;
    pI2CHandle->Status = status;

    return status;
}


I2C_Status_t I2C_TargetReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint32_t length)
{
    I2C_RegDef_t *pI2Cx;
    I2C_Status_t status;
    uint32_t i;

    //------------------------------------Checking validity-------------------------------------

    if ((pI2CHandle == NULL) ||
        (pRxBuffer == NULL)  ||
        (length == 0U))
    {
        return I2C_STATUS_ERR_INVALID;
    }

    pI2Cx = pI2CHandle->pI2Cx;

    if ((pI2Cx != I2C1) &&
        (pI2Cx != I2C2) &&
        (pI2Cx != I2C3))
    {
        return I2C_STATUS_ERR_INVALID;
    }

    if (pI2CHandle->State != I2C_STATE_IDLE)
    {
        return I2C_STATUS_ERR_BUSY;
    }

    //-----------------------------------------------------------------------------------------

    pI2CHandle->State = I2C_STATE_SLAVE_RX;
    pI2CHandle->Status = I2C_STATUS_OK;

    pI2CHandle->pRxBuffer = pRxBuffer;
    pI2CHandle->RxLength = length;
    pI2CHandle->RxIndex = 0U;

    //------------------------------------Waiting for master------------------------------------

    // Wait until the master addresses this device
    status = I2C_WaitForFlag(pI2Cx, ADDR);

    if (status != I2C_STATUS_OK)
    {
        goto error;
    }

    (void)pI2Cx->SR1;
    (void)pI2Cx->SR2;

    //------------------------------------Receiving data----------------------------------------

    for (i = 0U; i < length; i++)
    {
        status = I2C_WaitForFlag(pI2Cx, RXNE);

        if (status != I2C_STATUS_OK)
        {
            goto error;
        }

        pRxBuffer[i] = (uint8_t)pI2Cx->DR;
        pI2CHandle->RxIndex = i + 1U;
    }

    status = I2C_WaitForFlag(pI2Cx, STOPF);

    if (status != I2C_STATUS_OK)
    {
        goto error;
    }

    (void)pI2Cx->SR1;			// for clearing STOPF
    pI2Cx->CR1 = pI2Cx->CR1;	// this also


    pI2CHandle->State = I2C_STATE_IDLE;
    pI2CHandle->Status = I2C_STATUS_OK;

    return I2C_STATUS_OK;

error:

    pI2CHandle->State = I2C_STATE_IDLE;
    pI2CHandle->Status = status;

    return status;
}


I2C_Status_t I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle, const uint8_t *pTxBuffer, uint32_t length, uint8_t slaveAddress)
{
    I2C_RegDef_t *pI2Cx;

    //------------------------------------Checking validity-------------------------------------

    if ((pI2CHandle == NULL) ||
        (pTxBuffer == NULL)  ||
        (length == 0U)       ||
        (slaveAddress > 0x7FU))
    {
        return I2C_STATUS_ERR_INVALID;
    }

    pI2Cx = pI2CHandle->pI2Cx;

    if ((pI2Cx != I2C1) &&
        (pI2Cx != I2C2) &&
        (pI2Cx != I2C3))
    {
        return I2C_STATUS_ERR_INVALID;
    }

    if (pI2CHandle->State != I2C_STATE_IDLE)
    {
        return I2C_STATUS_ERR_BUSY;
    }

    if ((pI2Cx->SR2 & BUSY) != 0U)
    {
        return I2C_STATUS_ERR_BUSY;
    }

    //-----------------------------------------------------------------------------------------

    pI2CHandle->pTxBuffer = pTxBuffer;
    pI2CHandle->TxLength = length;
    pI2CHandle->TxIndex = 0U;
    pI2CHandle->SlaveAddress = slaveAddress;

    pI2CHandle->State = I2C_STATE_MASTER_TX;
    pI2CHandle->Status = I2C_STATUS_OK;

    // Enable error, event, and buffer interrupts
    pI2Cx->CR2 |= ITERREN;
    pI2Cx->CR2 |= ITEVTEN;
    pI2Cx->CR2 |= ITBUFEN;

    pI2Cx->CR1 |= START;

    return I2C_STATUS_OK;
}



I2C_Status_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint32_t length, uint8_t slaveAddress)
{
    I2C_RegDef_t *pI2Cx;

    //------------------------------------Checking validity-------------------------------------

    if ((pI2CHandle == NULL) ||
        (pRxBuffer == NULL)  ||
        (length == 0U)       ||
        (slaveAddress > 0x7FU))
    {
        return I2C_STATUS_ERR_INVALID;
    }

    pI2Cx = pI2CHandle->pI2Cx;

    if ((pI2Cx != I2C1) &&
        (pI2Cx != I2C2) &&
        (pI2Cx != I2C3))
    {
        return I2C_STATUS_ERR_INVALID;
    }

    if (pI2CHandle->State != I2C_STATE_IDLE)
    {
        return I2C_STATUS_ERR_BUSY;
    }

    if ((pI2Cx->SR2 & BUSY) != 0U)
    {
        return I2C_STATUS_ERR_BUSY;
    }

    //-----------------------------------------------------------------------------------------

    pI2CHandle->pRxBuffer = pRxBuffer;
    pI2CHandle->RxLength = length;
    pI2CHandle->RxIndex = 0U;
    pI2CHandle->SlaveAddress = slaveAddress;

    pI2CHandle->State = I2C_STATE_MASTER_RX;
    pI2CHandle->Status = I2C_STATUS_OK;

    // Enable error, event, and buffer interrupts
    pI2Cx->CR2 |= ITERREN;
    pI2Cx->CR2 |= ITEVTEN;
    pI2Cx->CR2 |= ITBUFEN;


    pI2Cx->CR1 &= ~POS;
    pI2Cx->CR1 |= ACK;
    pI2Cx->CR1 |= START;

    return I2C_STATUS_OK;
}





/******************************************************************************
 * Function: I2C_EV_IRQHandling
 *
 * Description:
 * Handles event interrupts for interrupt-driven I2C master transmission
 * and reception.
 *
 * Note:
 * Target/slave interrupt handling is not implemented in this version.
 ******************************************************************************/

void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle)
{
    I2C_RegDef_t *pI2Cx;
    uint32_t sr1;
    uint32_t remaining;

    /*--------------------------- Validate handle ---------------------------*/

    if (pI2CHandle == NULL)
    {
        return;
    }

    pI2Cx = pI2CHandle->pI2Cx;

    if ((pI2Cx != I2C1) &&
        (pI2Cx != I2C2) &&
        (pI2Cx != I2C3))
    {
        return;
    }

    sr1 = pI2Cx->SR1;

    /*------------------------- Master transmission -------------------------*/

    if (pI2CHandle->State == I2C_STATE_MASTER_TX)
    {

        if ((sr1 & SB) != 0U)
        {
            pI2Cx->DR =
                ((uint32_t)pI2CHandle->SlaveAddress << 1U);
        }


        else if ((sr1 & ADDR) != 0U)
        {
            (void)pI2Cx->SR1;
            (void)pI2Cx->SR2;
        }

        else if (((sr1 & BTF) != 0U) &&
                 (pI2CHandle->TxIndex == pI2CHandle->TxLength))
        {
            pI2Cx->CR1 |= STOP;

            pI2Cx->CR2 &= ~(ITERREN | ITEVTEN | ITBUFEN);

            pI2CHandle->State = I2C_STATE_IDLE;
            pI2CHandle->Status = I2C_STATUS_OK;
        }

        else if (((sr1 & TXE) != 0U) &&
                 (pI2CHandle->TxIndex < pI2CHandle->TxLength))
        {
            pI2Cx->DR =
                pI2CHandle->pTxBuffer[pI2CHandle->TxIndex];

            pI2CHandle->TxIndex++;
        }
    }

    /*-------------------------- Master reception ---------------------------*/

    else if (pI2CHandle->State == I2C_STATE_MASTER_RX)
    {
        if ((sr1 & SB) != 0U)
        {
            pI2Cx->DR =
                (((uint32_t)pI2CHandle->SlaveAddress << 1U) | 1U);
        }

        else if ((sr1 & ADDR) != 0U)
        {
            if (pI2CHandle->RxLength == 1U)
            {
                pI2Cx->CR1 &= ~ACK;
                pI2Cx->CR1 &= ~POS;

                (void)pI2Cx->SR1;
                (void)pI2Cx->SR2;

                pI2Cx->CR1 |= STOP;
            }
            else if (pI2CHandle->RxLength == 2U)
            {
                pI2Cx->CR1 |= POS;
                pI2Cx->CR1 &= ~ACK;

                pI2Cx->CR2 &= ~ITBUFEN;

                (void)pI2Cx->SR1;
                (void)pI2Cx->SR2;
            }
            else
            {
                pI2Cx->CR1 &= ~POS;
                pI2Cx->CR1 |= ACK;

                (void)pI2Cx->SR1;
                (void)pI2Cx->SR2;

                if (pI2CHandle->RxLength == 3U)
                {
                    pI2Cx->CR2 &= ~ITBUFEN;
                }
            }
        }

        else if (((sr1 & BTF) != 0U) &&
                 (pI2CHandle->RxLength == 2U))
        {
            pI2Cx->CR1 |= STOP;

            pI2CHandle->pRxBuffer[0] = (uint8_t)pI2Cx->DR;
            pI2CHandle->pRxBuffer[1] = (uint8_t)pI2Cx->DR;
            pI2CHandle->RxIndex = 2U;

            pI2Cx->CR1 &= ~POS;
            pI2Cx->CR1 |= ACK;

            pI2Cx->CR2 &= ~(ITERREN | ITEVTEN | ITBUFEN);

            pI2CHandle->State = I2C_STATE_IDLE;
            pI2CHandle->Status = I2C_STATUS_OK;
        }

        else if (((sr1 & BTF) != 0U) &&
                 (pI2CHandle->RxIndex <= pI2CHandle->RxLength) &&
                 ((pI2CHandle->RxLength - pI2CHandle->RxIndex) == 3U))
        {
            pI2Cx->CR1 &= ~ACK;

            pI2CHandle->pRxBuffer[pI2CHandle->RxIndex] =
                (uint8_t)pI2Cx->DR;

            pI2CHandle->RxIndex++;
        }

        else if (((sr1 & BTF) != 0U) &&
                 (pI2CHandle->RxIndex < pI2CHandle->RxLength) &&
                 ((pI2CHandle->RxLength - pI2CHandle->RxIndex) == 2U))
        {
            pI2Cx->CR1 |= STOP;

            pI2CHandle->pRxBuffer[pI2CHandle->RxIndex] =
                (uint8_t)pI2Cx->DR;
            pI2CHandle->RxIndex++;

            pI2CHandle->pRxBuffer[pI2CHandle->RxIndex] =
                (uint8_t)pI2Cx->DR;
            pI2CHandle->RxIndex++;

            pI2Cx->CR1 &= ~POS;
            pI2Cx->CR1 |= ACK;

            pI2Cx->CR2 &= ~(ITERREN | ITEVTEN | ITBUFEN);

            pI2CHandle->State = I2C_STATE_IDLE;
            pI2CHandle->Status = I2C_STATUS_OK;
        }

        else if (((sr1 & RXNE) != 0U) &&
                 (pI2CHandle->RxIndex < pI2CHandle->RxLength))
        {
            remaining =
                pI2CHandle->RxLength - pI2CHandle->RxIndex;

            if (remaining > 3U)
            {
                pI2CHandle->pRxBuffer[pI2CHandle->RxIndex] =
                    (uint8_t)pI2Cx->DR;

                pI2CHandle->RxIndex++;

                if ((pI2CHandle->RxLength - pI2CHandle->RxIndex) == 3U)
                {
                    pI2Cx->CR2 &= ~ITBUFEN;
                }
            }
            else if (pI2CHandle->RxLength == 1U)
            {
                pI2CHandle->pRxBuffer[0] = (uint8_t)pI2Cx->DR;
                pI2CHandle->RxIndex = 1U;

                pI2Cx->CR1 &= ~POS;
                pI2Cx->CR1 |= ACK;

                pI2Cx->CR2 &= ~(ITERREN | ITEVTEN | ITBUFEN);

                pI2CHandle->State = I2C_STATE_IDLE;
                pI2CHandle->Status = I2C_STATUS_OK;
            }
        }
    }
}



void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle)
{
    I2C_RegDef_t *pI2Cx;
    uint32_t sr1;
    I2C_Status_t status = I2C_STATUS_OK;
    uint8_t isMaster;

    if (pI2CHandle == NULL)
    {
        return;
    }

    pI2Cx = pI2CHandle->pI2Cx;

    if ((pI2Cx != I2C1) &&
        (pI2Cx != I2C2) &&
        (pI2Cx != I2C3))
    {
        return;
    }

    sr1 = pI2Cx->SR1;

    isMaster =
        ((pI2CHandle->State == I2C_STATE_MASTER_TX) ||
         (pI2CHandle->State == I2C_STATE_MASTER_RX));


    if ((sr1 & BERR) != 0U)
    {
        pI2Cx->SR1 &= ~BERR;
    }

    if ((sr1 & ARLO) != 0U)
    {
        pI2Cx->SR1 &= ~ARLO;
    }

    if ((sr1 & AF) != 0U)
    {
        pI2Cx->SR1 &= ~AF;
    }


    if ((sr1 & ARLO) != 0U)
    {
        status = I2C_STATUS_ERR_ARLO;
    }
    else if ((sr1 & BERR) != 0U)
    {
        status = I2C_STATUS_ERR_BERR;
    }
    else if ((sr1 & AF) != 0U)
    {
        status = I2C_STATUS_ERR_AF;
    }

    /*------------------------- Recover from error -------------------------*/

    if (status != I2C_STATUS_OK)
    {
        /*
         * Only request STOP for a master transfer that has not
         * lost arbitration. Do not force STOP on a target transfer.
         */
        if ((isMaster != 0U) &&
            (status != I2C_STATUS_ERR_ARLO))
        {
            pI2Cx->CR1 |= STOP;
        }

        pI2Cx->CR2 &= ~(ITERREN | ITEVTEN | ITBUFEN);

        pI2Cx->CR1 &= ~POS;
        pI2Cx->CR1 |= ACK;

        pI2CHandle->State = I2C_STATE_IDLE;
        pI2CHandle->Status = status;
    }
}


void I2C_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
{
    volatile uint32_t *pNVICRegister;
    uint32_t registerIndex;
    uint32_t bitPosition;

    if (IRQNumber > 85U)
    {
        return;
    }

    registerIndex = IRQNumber / 32U;
    bitPosition = IRQNumber % 32U;

    if (EnorDi != 0U)
    {
        pNVICRegister = (volatile uint32_t *)(0xE000E100UL + (registerIndex * 4U));
    }
    else
    {
        pNVICRegister = (volatile uint32_t *)(0xE000E180UL + (registerIndex * 4U));
    }

    *pNVICRegister = (1UL << bitPosition);
}



//--------------------------------------- Helper functions -----------------------------------

static I2C_Status_t I2C_WaitForFlag(I2C_RegDef_t *pI2Cx, uint32_t flag)
{
    uint32_t timeout = I2C_POLL_TIMEOUT;

    while ((pI2Cx->SR1 & flag) == 0U)
    {
        if ((pI2Cx->SR1 & BERR) != 0U)
        {
            pI2Cx->SR1 &= ~BERR;
            return I2C_STATUS_ERR_BERR;
        }

        if ((pI2Cx->SR1 & ARLO) != 0U)
        {
            pI2Cx->SR1 &= ~ARLO;
            return I2C_STATUS_ERR_ARLO;
        }

        if ((pI2Cx->SR1 & AF) != 0U)
        {
            pI2Cx->SR1 &= ~AF;
            return I2C_STATUS_ERR_AF;
        }

        if (--timeout == 0U)
        {
            return I2C_STATUS_ERR_TIMEOUT;
        }
    }
    return I2C_STATUS_OK;
}
