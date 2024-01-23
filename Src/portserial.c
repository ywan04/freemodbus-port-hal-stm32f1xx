/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial.c,v 1.1 2006/08/22 21:35:13 wolti Exp $
 */

#include "port.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_uart.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- global variables ---------------------------------*/
static UART_HandleTypeDef *huart;
static uint16_t TX_Enable_Pin;
static GPIO_TypeDef *TX_Enable_GPIO;

/* ----------------------- static functions ---------------------------------*/
//static void prvvUARTTxReadyISR( void );
//static void prvvUARTRxISR( void );

/* ----------------------- Start implementation -----------------------------*/
void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    /* If xRxEnable enable serial receive interrupts. If xTxENable enable
     * transmitter empty interrupts.
     */
	if (xRxEnable) {
		HAL_GPIO_WritePin(TX_Enable_GPIO, TX_Enable_Pin, GPIO_PIN_RESET);
		HAL_UART_Receive_IT(huart, (uint8_t *)&xRxEnable, 1);
	} else {
		HAL_UART_AbortReceive(huart);
	}

	if (xTxEnable) {
		HAL_GPIO_WritePin(TX_Enable_GPIO, TX_Enable_Pin, GPIO_PIN_SET);
		HAL_UART_TxCpltCallback(huart);
	} else {
		HAL_UART_AbortTransmit(huart);
	}
}

void
Modbus_UART_Init(UART_HandleTypeDef *uart, GPIO_TypeDef *GPIOx, uint16_t Pin)
{
	huart = uart;
	TX_Enable_GPIO = GPIOx;
	TX_Enable_Pin = Pin;
}

BOOL
xMBPortSerialInit( UCHAR ucPort, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
	(void) ucPort;
	(void) ulBaudRate;
	(void) ucDataBits;
	(void) eParity;
	return TRUE;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */
	HAL_UART_Transmit_IT(huart, (uint8_t *)&ucByte, 1);

    return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    /* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
     */
	HAL_UART_Receive_IT(huart, (uint8_t *)pucByte, 1);

    return TRUE;
}

/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
    ( void )pxMBFrameCBTransmitterEmpty(  );
}

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
    ( void )pxMBFrameCBByteReceived(  );
}
