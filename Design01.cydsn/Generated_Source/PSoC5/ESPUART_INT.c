/*******************************************************************************
* File Name: ESPUARTINT.c
* Version 2.50
*
* Description:
*  This file provides all Interrupt Service functionality of the UART component
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "ESPUART.h"
#include "cyapicallbacks.h"


/***************************************
* Custom Declarations
***************************************/
/* `#START CUSTOM_DECLARATIONS` Place your declaration here */

/* `#END` */

#if (ESPUART_RX_INTERRUPT_ENABLED && (ESPUART_RX_ENABLED || ESPUART_HD_ENABLED))
    /*******************************************************************************
    * Function Name: ESPUART_RXISR
    ********************************************************************************
    *
    * Summary:
    *  Interrupt Service Routine for RX portion of the UART
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  ESPUART_rxBuffer - RAM buffer pointer for save received data.
    *  ESPUART_rxBufferWrite - cyclic index for write to rxBuffer,
    *     increments after each byte saved to buffer.
    *  ESPUART_rxBufferRead - cyclic index for read from rxBuffer,
    *     checked to detect overflow condition.
    *  ESPUART_rxBufferOverflow - software overflow flag. Set to one
    *     when ESPUART_rxBufferWrite index overtakes
    *     ESPUART_rxBufferRead index.
    *  ESPUART_rxBufferLoopDetect - additional variable to detect overflow.
    *     Set to one when ESPUART_rxBufferWrite is equal to
    *    ESPUART_rxBufferRead
    *  ESPUART_rxAddressMode - this variable contains the Address mode,
    *     selected in customizer or set by UART_SetRxAddressMode() API.
    *  ESPUART_rxAddressDetected - set to 1 when correct address received,
    *     and analysed to store following addressed data bytes to the buffer.
    *     When not correct address received, set to 0 to skip following data bytes.
    *
    *******************************************************************************/
    CY_ISR(ESPUART_RXISR)
    {
        uint8 readData;
        uint8 readStatus;
        uint8 increment_pointer = 0u;

    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef ESPUART_RXISR_ENTRY_CALLBACK
        ESPUART_RXISR_EntryCallback();
    #endif /* ESPUART_RXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START ESPUART_RXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        do
        {
            /* Read receiver status register */
            readStatus = ESPUART_RXSTATUS_REG;
            /* Copy the same status to readData variable for backward compatibility support 
            *  of the user code in ESPUART_RXISR_ERROR` section. 
            */
            readData = readStatus;

            if((readStatus & (ESPUART_RX_STS_BREAK | 
                            ESPUART_RX_STS_PAR_ERROR |
                            ESPUART_RX_STS_STOP_ERROR | 
                            ESPUART_RX_STS_OVERRUN)) != 0u)
            {
                /* ERROR handling. */
                ESPUART_errorStatus |= readStatus & ( ESPUART_RX_STS_BREAK | 
                                                            ESPUART_RX_STS_PAR_ERROR | 
                                                            ESPUART_RX_STS_STOP_ERROR | 
                                                            ESPUART_RX_STS_OVERRUN);
                /* `#START ESPUART_RXISR_ERROR` */

                /* `#END` */
                
            #ifdef ESPUART_RXISR_ERROR_CALLBACK
                ESPUART_RXISR_ERROR_Callback();
            #endif /* ESPUART_RXISR_ERROR_CALLBACK */
            }
            
            if((readStatus & ESPUART_RX_STS_FIFO_NOTEMPTY) != 0u)
            {
                /* Read data from the RX data register */
                readData = ESPUART_RXDATA_REG;
            #if (ESPUART_RXHW_ADDRESS_ENABLED)
                if(ESPUART_rxAddressMode == (uint8)ESPUART__B_UART__AM_SW_DETECT_TO_BUFFER)
                {
                    if((readStatus & ESPUART_RX_STS_MRKSPC) != 0u)
                    {
                        if ((readStatus & ESPUART_RX_STS_ADDR_MATCH) != 0u)
                        {
                            ESPUART_rxAddressDetected = 1u;
                        }
                        else
                        {
                            ESPUART_rxAddressDetected = 0u;
                        }
                    }
                    if(ESPUART_rxAddressDetected != 0u)
                    {   /* Store only addressed data */
                        ESPUART_rxBuffer[ESPUART_rxBufferWrite] = readData;
                        increment_pointer = 1u;
                    }
                }
                else /* Without software addressing */
                {
                    ESPUART_rxBuffer[ESPUART_rxBufferWrite] = readData;
                    increment_pointer = 1u;
                }
            #else  /* Without addressing */
                ESPUART_rxBuffer[ESPUART_rxBufferWrite] = readData;
                increment_pointer = 1u;
            #endif /* (ESPUART_RXHW_ADDRESS_ENABLED) */

                /* Do not increment buffer pointer when skip not addressed data */
                if(increment_pointer != 0u)
                {
                    if(ESPUART_rxBufferLoopDetect != 0u)
                    {   /* Set Software Buffer status Overflow */
                        ESPUART_rxBufferOverflow = 1u;
                    }
                    /* Set next pointer. */
                    ESPUART_rxBufferWrite++;

                    /* Check pointer for a loop condition */
                    if(ESPUART_rxBufferWrite >= ESPUART_RX_BUFFER_SIZE)
                    {
                        ESPUART_rxBufferWrite = 0u;
                    }

                    /* Detect pre-overload condition and set flag */
                    if(ESPUART_rxBufferWrite == ESPUART_rxBufferRead)
                    {
                        ESPUART_rxBufferLoopDetect = 1u;
                        /* When Hardware Flow Control selected */
                        #if (ESPUART_FLOW_CONTROL != 0u)
                            /* Disable RX interrupt mask, it is enabled when user read data from the buffer using APIs */
                            ESPUART_RXSTATUS_MASK_REG  &= (uint8)~ESPUART_RX_STS_FIFO_NOTEMPTY;
                            CyIntClearPending(ESPUART_RX_VECT_NUM);
                            break; /* Break the reading of the FIFO loop, leave the data there for generating RTS signal */
                        #endif /* (ESPUART_FLOW_CONTROL != 0u) */
                    }
                }
            }
        }while((readStatus & ESPUART_RX_STS_FIFO_NOTEMPTY) != 0u);

        /* User code required at end of ISR (Optional) */
        /* `#START ESPUART_RXISR_END` */

        /* `#END` */

    #ifdef ESPUART_RXISR_EXIT_CALLBACK
        ESPUART_RXISR_ExitCallback();
    #endif /* ESPUART_RXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
    }
    
#endif /* (ESPUART_RX_INTERRUPT_ENABLED && (ESPUART_RX_ENABLED || ESPUART_HD_ENABLED)) */


#if (ESPUART_TX_INTERRUPT_ENABLED && ESPUART_TX_ENABLED)
    /*******************************************************************************
    * Function Name: ESPUART_TXISR
    ********************************************************************************
    *
    * Summary:
    * Interrupt Service Routine for the TX portion of the UART
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  ESPUART_txBuffer - RAM buffer pointer for transmit data from.
    *  ESPUART_txBufferRead - cyclic index for read and transmit data
    *     from txBuffer, increments after each transmitted byte.
    *  ESPUART_rxBufferWrite - cyclic index for write to txBuffer,
    *     checked to detect available for transmission bytes.
    *
    *******************************************************************************/
    CY_ISR(ESPUART_TXISR)
    {
    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef ESPUART_TXISR_ENTRY_CALLBACK
        ESPUART_TXISR_EntryCallback();
    #endif /* ESPUART_TXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START ESPUART_TXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        while((ESPUART_txBufferRead != ESPUART_txBufferWrite) &&
             ((ESPUART_TXSTATUS_REG & ESPUART_TX_STS_FIFO_FULL) == 0u))
        {
            /* Check pointer wrap around */
            if(ESPUART_txBufferRead >= ESPUART_TX_BUFFER_SIZE)
            {
                ESPUART_txBufferRead = 0u;
            }

            ESPUART_TXDATA_REG = ESPUART_txBuffer[ESPUART_txBufferRead];

            /* Set next pointer */
            ESPUART_txBufferRead++;
        }

        /* User code required at end of ISR (Optional) */
        /* `#START ESPUART_TXISR_END` */

        /* `#END` */

    #ifdef ESPUART_TXISR_EXIT_CALLBACK
        ESPUART_TXISR_ExitCallback();
    #endif /* ESPUART_TXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
   }
#endif /* (ESPUART_TX_INTERRUPT_ENABLED && ESPUART_TX_ENABLED) */


/* [] END OF FILE */
