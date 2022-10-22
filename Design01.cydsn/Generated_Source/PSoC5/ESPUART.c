/*******************************************************************************
* File Name: ESPUART.c
* Version 2.50
*
* Description:
*  This file provides all API functionality of the UART component
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "ESPUART.h"
#if (ESPUART_INTERNAL_CLOCK_USED)
    #include "ESPUART_IntClock.h"
#endif /* End ESPUART_INTERNAL_CLOCK_USED */


/***************************************
* Global data allocation
***************************************/

uint8 ESPUART_initVar = 0u;

#if (ESPUART_TX_INTERRUPT_ENABLED && ESPUART_TX_ENABLED)
    volatile uint8 ESPUART_txBuffer[ESPUART_TX_BUFFER_SIZE];
    volatile uint16 ESPUART_txBufferRead = 0u;
    uint16 ESPUART_txBufferWrite = 0u;
#endif /* (ESPUART_TX_INTERRUPT_ENABLED && ESPUART_TX_ENABLED) */

#if (ESPUART_RX_INTERRUPT_ENABLED && (ESPUART_RX_ENABLED || ESPUART_HD_ENABLED))
    uint8 ESPUART_errorStatus = 0u;
    volatile uint8 ESPUART_rxBuffer[ESPUART_RX_BUFFER_SIZE];
    volatile uint16 ESPUART_rxBufferRead  = 0u;
    volatile uint16 ESPUART_rxBufferWrite = 0u;
    volatile uint8 ESPUART_rxBufferLoopDetect = 0u;
    volatile uint8 ESPUART_rxBufferOverflow   = 0u;
    #if (ESPUART_RXHW_ADDRESS_ENABLED)
        volatile uint8 ESPUART_rxAddressMode = ESPUART_RX_ADDRESS_MODE;
        volatile uint8 ESPUART_rxAddressDetected = 0u;
    #endif /* (ESPUART_RXHW_ADDRESS_ENABLED) */
#endif /* (ESPUART_RX_INTERRUPT_ENABLED && (ESPUART_RX_ENABLED || ESPUART_HD_ENABLED)) */


/*******************************************************************************
* Function Name: ESPUART_Start
********************************************************************************
*
* Summary:
*  This is the preferred method to begin component operation.
*  ESPUART_Start() sets the initVar variable, calls the
*  ESPUART_Init() function, and then calls the
*  ESPUART_Enable() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  The ESPUART_intiVar variable is used to indicate initial
*  configuration of this component. The variable is initialized to zero (0u)
*  and set to one (1u) the first time ESPUART_Start() is called. This
*  allows for component initialization without re-initialization in all
*  subsequent calls to the ESPUART_Start() routine.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void ESPUART_Start(void) 
{
    /* If not initialized then initialize all required hardware and software */
    if(ESPUART_initVar == 0u)
    {
        ESPUART_Init();
        ESPUART_initVar = 1u;
    }

    ESPUART_Enable();
}


/*******************************************************************************
* Function Name: ESPUART_Init
********************************************************************************
*
* Summary:
*  Initializes or restores the component according to the customizer Configure
*  dialog settings. It is not necessary to call ESPUART_Init() because
*  the ESPUART_Start() API calls this function and is the preferred
*  method to begin component operation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void ESPUART_Init(void) 
{
    #if(ESPUART_RX_ENABLED || ESPUART_HD_ENABLED)

        #if (ESPUART_RX_INTERRUPT_ENABLED)
            /* Set RX interrupt vector and priority */
            (void) CyIntSetVector(ESPUART_RX_VECT_NUM, &ESPUART_RXISR);
            CyIntSetPriority(ESPUART_RX_VECT_NUM, ESPUART_RX_PRIOR_NUM);
            ESPUART_errorStatus = 0u;
        #endif /* (ESPUART_RX_INTERRUPT_ENABLED) */

        #if (ESPUART_RXHW_ADDRESS_ENABLED)
            ESPUART_SetRxAddressMode(ESPUART_RX_ADDRESS_MODE);
            ESPUART_SetRxAddress1(ESPUART_RX_HW_ADDRESS1);
            ESPUART_SetRxAddress2(ESPUART_RX_HW_ADDRESS2);
        #endif /* End ESPUART_RXHW_ADDRESS_ENABLED */

        /* Init Count7 period */
        ESPUART_RXBITCTR_PERIOD_REG = ESPUART_RXBITCTR_INIT;
        /* Configure the Initial RX interrupt mask */
        ESPUART_RXSTATUS_MASK_REG  = ESPUART_INIT_RX_INTERRUPTS_MASK;
    #endif /* End ESPUART_RX_ENABLED || ESPUART_HD_ENABLED*/

    #if(ESPUART_TX_ENABLED)
        #if (ESPUART_TX_INTERRUPT_ENABLED)
            /* Set TX interrupt vector and priority */
            (void) CyIntSetVector(ESPUART_TX_VECT_NUM, &ESPUART_TXISR);
            CyIntSetPriority(ESPUART_TX_VECT_NUM, ESPUART_TX_PRIOR_NUM);
        #endif /* (ESPUART_TX_INTERRUPT_ENABLED) */

        /* Write Counter Value for TX Bit Clk Generator*/
        #if (ESPUART_TXCLKGEN_DP)
            ESPUART_TXBITCLKGEN_CTR_REG = ESPUART_BIT_CENTER;
            ESPUART_TXBITCLKTX_COMPLETE_REG = ((ESPUART_NUMBER_OF_DATA_BITS +
                        ESPUART_NUMBER_OF_START_BIT) * ESPUART_OVER_SAMPLE_COUNT) - 1u;
        #else
            ESPUART_TXBITCTR_PERIOD_REG = ((ESPUART_NUMBER_OF_DATA_BITS +
                        ESPUART_NUMBER_OF_START_BIT) * ESPUART_OVER_SAMPLE_8) - 1u;
        #endif /* End ESPUART_TXCLKGEN_DP */

        /* Configure the Initial TX interrupt mask */
        #if (ESPUART_TX_INTERRUPT_ENABLED)
            ESPUART_TXSTATUS_MASK_REG = ESPUART_TX_STS_FIFO_EMPTY;
        #else
            ESPUART_TXSTATUS_MASK_REG = ESPUART_INIT_TX_INTERRUPTS_MASK;
        #endif /*End ESPUART_TX_INTERRUPT_ENABLED*/

    #endif /* End ESPUART_TX_ENABLED */

    #if(ESPUART_PARITY_TYPE_SW)  /* Write Parity to Control Register */
        ESPUART_WriteControlRegister( \
            (ESPUART_ReadControlRegister() & (uint8)~ESPUART_CTRL_PARITY_TYPE_MASK) | \
            (uint8)(ESPUART_PARITY_TYPE << ESPUART_CTRL_PARITY_TYPE0_SHIFT) );
    #endif /* End ESPUART_PARITY_TYPE_SW */
}


/*******************************************************************************
* Function Name: ESPUART_Enable
********************************************************************************
*
* Summary:
*  Activates the hardware and begins component operation. It is not necessary
*  to call ESPUART_Enable() because the ESPUART_Start() API
*  calls this function, which is the preferred method to begin component
*  operation.

* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  ESPUART_rxAddressDetected - set to initial state (0).
*
*******************************************************************************/
void ESPUART_Enable(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    #if (ESPUART_RX_ENABLED || ESPUART_HD_ENABLED)
        /* RX Counter (Count7) Enable */
        ESPUART_RXBITCTR_CONTROL_REG |= ESPUART_CNTR_ENABLE;

        /* Enable the RX Interrupt */
        ESPUART_RXSTATUS_ACTL_REG  |= ESPUART_INT_ENABLE;

        #if (ESPUART_RX_INTERRUPT_ENABLED)
            ESPUART_EnableRxInt();

            #if (ESPUART_RXHW_ADDRESS_ENABLED)
                ESPUART_rxAddressDetected = 0u;
            #endif /* (ESPUART_RXHW_ADDRESS_ENABLED) */
        #endif /* (ESPUART_RX_INTERRUPT_ENABLED) */
    #endif /* (ESPUART_RX_ENABLED || ESPUART_HD_ENABLED) */

    #if(ESPUART_TX_ENABLED)
        /* TX Counter (DP/Count7) Enable */
        #if(!ESPUART_TXCLKGEN_DP)
            ESPUART_TXBITCTR_CONTROL_REG |= ESPUART_CNTR_ENABLE;
        #endif /* End ESPUART_TXCLKGEN_DP */

        /* Enable the TX Interrupt */
        ESPUART_TXSTATUS_ACTL_REG |= ESPUART_INT_ENABLE;
        #if (ESPUART_TX_INTERRUPT_ENABLED)
            ESPUART_ClearPendingTxInt(); /* Clear history of TX_NOT_EMPTY */
            ESPUART_EnableTxInt();
        #endif /* (ESPUART_TX_INTERRUPT_ENABLED) */
     #endif /* (ESPUART_TX_INTERRUPT_ENABLED) */

    #if (ESPUART_INTERNAL_CLOCK_USED)
        ESPUART_IntClock_Start();  /* Enable the clock */
    #endif /* (ESPUART_INTERNAL_CLOCK_USED) */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: ESPUART_Stop
********************************************************************************
*
* Summary:
*  Disables the UART operation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void ESPUART_Stop(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    /* Write Bit Counter Disable */
    #if (ESPUART_RX_ENABLED || ESPUART_HD_ENABLED)
        ESPUART_RXBITCTR_CONTROL_REG &= (uint8) ~ESPUART_CNTR_ENABLE;
    #endif /* (ESPUART_RX_ENABLED || ESPUART_HD_ENABLED) */

    #if (ESPUART_TX_ENABLED)
        #if(!ESPUART_TXCLKGEN_DP)
            ESPUART_TXBITCTR_CONTROL_REG &= (uint8) ~ESPUART_CNTR_ENABLE;
        #endif /* (!ESPUART_TXCLKGEN_DP) */
    #endif /* (ESPUART_TX_ENABLED) */

    #if (ESPUART_INTERNAL_CLOCK_USED)
        ESPUART_IntClock_Stop();   /* Disable the clock */
    #endif /* (ESPUART_INTERNAL_CLOCK_USED) */

    /* Disable internal interrupt component */
    #if (ESPUART_RX_ENABLED || ESPUART_HD_ENABLED)
        ESPUART_RXSTATUS_ACTL_REG  &= (uint8) ~ESPUART_INT_ENABLE;

        #if (ESPUART_RX_INTERRUPT_ENABLED)
            ESPUART_DisableRxInt();
        #endif /* (ESPUART_RX_INTERRUPT_ENABLED) */
    #endif /* (ESPUART_RX_ENABLED || ESPUART_HD_ENABLED) */

    #if (ESPUART_TX_ENABLED)
        ESPUART_TXSTATUS_ACTL_REG &= (uint8) ~ESPUART_INT_ENABLE;

        #if (ESPUART_TX_INTERRUPT_ENABLED)
            ESPUART_DisableTxInt();
        #endif /* (ESPUART_TX_INTERRUPT_ENABLED) */
    #endif /* (ESPUART_TX_ENABLED) */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: ESPUART_ReadControlRegister
********************************************************************************
*
* Summary:
*  Returns the current value of the control register.
*
* Parameters:
*  None.
*
* Return:
*  Contents of the control register.
*
*******************************************************************************/
uint8 ESPUART_ReadControlRegister(void) 
{
    #if (ESPUART_CONTROL_REG_REMOVED)
        return(0u);
    #else
        return(ESPUART_CONTROL_REG);
    #endif /* (ESPUART_CONTROL_REG_REMOVED) */
}


/*******************************************************************************
* Function Name: ESPUART_WriteControlRegister
********************************************************************************
*
* Summary:
*  Writes an 8-bit value into the control register
*
* Parameters:
*  control:  control register value
*
* Return:
*  None.
*
*******************************************************************************/
void  ESPUART_WriteControlRegister(uint8 control) 
{
    #if (ESPUART_CONTROL_REG_REMOVED)
        if(0u != control)
        {
            /* Suppress compiler warning */
        }
    #else
       ESPUART_CONTROL_REG = control;
    #endif /* (ESPUART_CONTROL_REG_REMOVED) */
}


#if(ESPUART_RX_ENABLED || ESPUART_HD_ENABLED)
    /*******************************************************************************
    * Function Name: ESPUART_SetRxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the RX interrupt sources enabled.
    *
    * Parameters:
    *  IntSrc:  Bit field containing the RX interrupts to enable. Based on the 
    *  bit-field arrangement of the status register. This value must be a 
    *  combination of status register bit-masks shown below:
    *      ESPUART_RX_STS_FIFO_NOTEMPTY    Interrupt on byte received.
    *      ESPUART_RX_STS_PAR_ERROR        Interrupt on parity error.
    *      ESPUART_RX_STS_STOP_ERROR       Interrupt on stop error.
    *      ESPUART_RX_STS_BREAK            Interrupt on break.
    *      ESPUART_RX_STS_OVERRUN          Interrupt on overrun error.
    *      ESPUART_RX_STS_ADDR_MATCH       Interrupt on address match.
    *      ESPUART_RX_STS_MRKSPC           Interrupt on address detect.
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void ESPUART_SetRxInterruptMode(uint8 intSrc) 
    {
        ESPUART_RXSTATUS_MASK_REG  = intSrc;
    }


    /*******************************************************************************
    * Function Name: ESPUART_ReadRxData
    ********************************************************************************
    *
    * Summary:
    *  Returns the next byte of received data. This function returns data without
    *  checking the status. You must check the status separately.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Received data from RX register
    *
    * Global Variables:
    *  ESPUART_rxBuffer - RAM buffer pointer for save received data.
    *  ESPUART_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  ESPUART_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  ESPUART_rxBufferLoopDetect - cleared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 ESPUART_ReadRxData(void) 
    {
        uint8 rxData;

    #if (ESPUART_RX_INTERRUPT_ENABLED)

        uint16 locRxBufferRead;
        uint16 locRxBufferWrite;

        /* Protect variables that could change on interrupt */
        ESPUART_DisableRxInt();

        locRxBufferRead  = ESPUART_rxBufferRead;
        locRxBufferWrite = ESPUART_rxBufferWrite;

        if( (ESPUART_rxBufferLoopDetect != 0u) || (locRxBufferRead != locRxBufferWrite) )
        {
            rxData = ESPUART_rxBuffer[locRxBufferRead];
            locRxBufferRead++;

            if(locRxBufferRead >= ESPUART_RX_BUFFER_SIZE)
            {
                locRxBufferRead = 0u;
            }
            /* Update the real pointer */
            ESPUART_rxBufferRead = locRxBufferRead;

            if(ESPUART_rxBufferLoopDetect != 0u)
            {
                ESPUART_rxBufferLoopDetect = 0u;
                #if ((ESPUART_RX_INTERRUPT_ENABLED) && (ESPUART_FLOW_CONTROL != 0u))
                    /* When Hardware Flow Control selected - return RX mask */
                    #if( ESPUART_HD_ENABLED )
                        if((ESPUART_CONTROL_REG & ESPUART_CTRL_HD_SEND) == 0u)
                        {   /* In Half duplex mode return RX mask only in RX
                            *  configuration set, otherwise
                            *  mask will be returned in LoadRxConfig() API.
                            */
                            ESPUART_RXSTATUS_MASK_REG  |= ESPUART_RX_STS_FIFO_NOTEMPTY;
                        }
                    #else
                        ESPUART_RXSTATUS_MASK_REG  |= ESPUART_RX_STS_FIFO_NOTEMPTY;
                    #endif /* end ESPUART_HD_ENABLED */
                #endif /* ((ESPUART_RX_INTERRUPT_ENABLED) && (ESPUART_FLOW_CONTROL != 0u)) */
            }
        }
        else
        {   /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit */
            rxData = ESPUART_RXDATA_REG;
        }

        ESPUART_EnableRxInt();

    #else

        /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit */
        rxData = ESPUART_RXDATA_REG;

    #endif /* (ESPUART_RX_INTERRUPT_ENABLED) */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: ESPUART_ReadRxStatus
    ********************************************************************************
    *
    * Summary:
    *  Returns the current state of the receiver status register and the software
    *  buffer overflow status.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Current state of the status register.
    *
    * Side Effect:
    *  All status register bits are clear-on-read except
    *  ESPUART_RX_STS_FIFO_NOTEMPTY.
    *  ESPUART_RX_STS_FIFO_NOTEMPTY clears immediately after RX data
    *  register read.
    *
    * Global Variables:
    *  ESPUART_rxBufferOverflow - used to indicate overload condition.
    *   It set to one in RX interrupt when there isn't free space in
    *   ESPUART_rxBufferRead to write new data. This condition returned
    *   and cleared to zero by this API as an
    *   ESPUART_RX_STS_SOFT_BUFF_OVER bit along with RX Status register
    *   bits.
    *
    *******************************************************************************/
    uint8 ESPUART_ReadRxStatus(void) 
    {
        uint8 status;

        status = ESPUART_RXSTATUS_REG & ESPUART_RX_HW_MASK;

    #if (ESPUART_RX_INTERRUPT_ENABLED)
        if(ESPUART_rxBufferOverflow != 0u)
        {
            status |= ESPUART_RX_STS_SOFT_BUFF_OVER;
            ESPUART_rxBufferOverflow = 0u;
        }
    #endif /* (ESPUART_RX_INTERRUPT_ENABLED) */

        return(status);
    }


    /*******************************************************************************
    * Function Name: ESPUART_GetChar
    ********************************************************************************
    *
    * Summary:
    *  Returns the last received byte of data. ESPUART_GetChar() is
    *  designed for ASCII characters and returns a uint8 where 1 to 255 are values
    *  for valid characters and 0 indicates an error occurred or no data is present.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Character read from UART RX buffer. ASCII characters from 1 to 255 are valid.
    *  A returned zero signifies an error condition or no data available.
    *
    * Global Variables:
    *  ESPUART_rxBuffer - RAM buffer pointer for save received data.
    *  ESPUART_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  ESPUART_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  ESPUART_rxBufferLoopDetect - cleared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 ESPUART_GetChar(void) 
    {
        uint8 rxData = 0u;
        uint8 rxStatus;

    #if (ESPUART_RX_INTERRUPT_ENABLED)
        uint16 locRxBufferRead;
        uint16 locRxBufferWrite;

        /* Protect variables that could change on interrupt */
        ESPUART_DisableRxInt();

        locRxBufferRead  = ESPUART_rxBufferRead;
        locRxBufferWrite = ESPUART_rxBufferWrite;

        if( (ESPUART_rxBufferLoopDetect != 0u) || (locRxBufferRead != locRxBufferWrite) )
        {
            rxData = ESPUART_rxBuffer[locRxBufferRead];
            locRxBufferRead++;
            if(locRxBufferRead >= ESPUART_RX_BUFFER_SIZE)
            {
                locRxBufferRead = 0u;
            }
            /* Update the real pointer */
            ESPUART_rxBufferRead = locRxBufferRead;

            if(ESPUART_rxBufferLoopDetect != 0u)
            {
                ESPUART_rxBufferLoopDetect = 0u;
                #if( (ESPUART_RX_INTERRUPT_ENABLED) && (ESPUART_FLOW_CONTROL != 0u) )
                    /* When Hardware Flow Control selected - return RX mask */
                    #if( ESPUART_HD_ENABLED )
                        if((ESPUART_CONTROL_REG & ESPUART_CTRL_HD_SEND) == 0u)
                        {   /* In Half duplex mode return RX mask only if
                            *  RX configuration set, otherwise
                            *  mask will be returned in LoadRxConfig() API.
                            */
                            ESPUART_RXSTATUS_MASK_REG |= ESPUART_RX_STS_FIFO_NOTEMPTY;
                        }
                    #else
                        ESPUART_RXSTATUS_MASK_REG |= ESPUART_RX_STS_FIFO_NOTEMPTY;
                    #endif /* end ESPUART_HD_ENABLED */
                #endif /* ESPUART_RX_INTERRUPT_ENABLED and Hardware flow control*/
            }

        }
        else
        {   rxStatus = ESPUART_RXSTATUS_REG;
            if((rxStatus & ESPUART_RX_STS_FIFO_NOTEMPTY) != 0u)
            {   /* Read received data from FIFO */
                rxData = ESPUART_RXDATA_REG;
                /*Check status on error*/
                if((rxStatus & (ESPUART_RX_STS_BREAK | ESPUART_RX_STS_PAR_ERROR |
                                ESPUART_RX_STS_STOP_ERROR | ESPUART_RX_STS_OVERRUN)) != 0u)
                {
                    rxData = 0u;
                }
            }
        }

        ESPUART_EnableRxInt();

    #else

        rxStatus =ESPUART_RXSTATUS_REG;
        if((rxStatus & ESPUART_RX_STS_FIFO_NOTEMPTY) != 0u)
        {
            /* Read received data from FIFO */
            rxData = ESPUART_RXDATA_REG;

            /*Check status on error*/
            if((rxStatus & (ESPUART_RX_STS_BREAK | ESPUART_RX_STS_PAR_ERROR |
                            ESPUART_RX_STS_STOP_ERROR | ESPUART_RX_STS_OVERRUN)) != 0u)
            {
                rxData = 0u;
            }
        }
    #endif /* (ESPUART_RX_INTERRUPT_ENABLED) */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: ESPUART_GetByte
    ********************************************************************************
    *
    * Summary:
    *  Reads UART RX buffer immediately, returns received character and error
    *  condition.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  MSB contains status and LSB contains UART RX data. If the MSB is nonzero,
    *  an error has occurred.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint16 ESPUART_GetByte(void) 
    {
        
    #if (ESPUART_RX_INTERRUPT_ENABLED)
        uint16 locErrorStatus;
        /* Protect variables that could change on interrupt */
        ESPUART_DisableRxInt();
        locErrorStatus = (uint16)ESPUART_errorStatus;
        ESPUART_errorStatus = 0u;
        ESPUART_EnableRxInt();
        return ( (uint16)(locErrorStatus << 8u) | ESPUART_ReadRxData() );
    #else
        return ( ((uint16)ESPUART_ReadRxStatus() << 8u) | ESPUART_ReadRxData() );
    #endif /* ESPUART_RX_INTERRUPT_ENABLED */
        
    }


    /*******************************************************************************
    * Function Name: ESPUART_GetRxBufferSize
    ********************************************************************************
    *
    * Summary:
    *  Returns the number of received bytes available in the RX buffer.
    *  * RX software buffer is disabled (RX Buffer Size parameter is equal to 4): 
    *    returns 0 for empty RX FIFO or 1 for not empty RX FIFO.
    *  * RX software buffer is enabled: returns the number of bytes available in 
    *    the RX software buffer. Bytes available in the RX FIFO do not take to 
    *    account.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  uint16: Number of bytes in the RX buffer. 
    *    Return value type depends on RX Buffer Size parameter.
    *
    * Global Variables:
    *  ESPUART_rxBufferWrite - used to calculate left bytes.
    *  ESPUART_rxBufferRead - used to calculate left bytes.
    *  ESPUART_rxBufferLoopDetect - checked to decide left bytes amount.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the RX Buffer is.
    *
    *******************************************************************************/
    uint16 ESPUART_GetRxBufferSize(void)
                                                            
    {
        uint16 size;

    #if (ESPUART_RX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt */
        ESPUART_DisableRxInt();

        if(ESPUART_rxBufferRead == ESPUART_rxBufferWrite)
        {
            if(ESPUART_rxBufferLoopDetect != 0u)
            {
                size = ESPUART_RX_BUFFER_SIZE;
            }
            else
            {
                size = 0u;
            }
        }
        else if(ESPUART_rxBufferRead < ESPUART_rxBufferWrite)
        {
            size = (ESPUART_rxBufferWrite - ESPUART_rxBufferRead);
        }
        else
        {
            size = (ESPUART_RX_BUFFER_SIZE - ESPUART_rxBufferRead) + ESPUART_rxBufferWrite;
        }

        ESPUART_EnableRxInt();

    #else

        /* We can only know if there is data in the fifo. */
        size = ((ESPUART_RXSTATUS_REG & ESPUART_RX_STS_FIFO_NOTEMPTY) != 0u) ? 1u : 0u;

    #endif /* (ESPUART_RX_INTERRUPT_ENABLED) */

        return(size);
    }


    /*******************************************************************************
    * Function Name: ESPUART_ClearRxBuffer
    ********************************************************************************
    *
    * Summary:
    *  Clears the receiver memory buffer and hardware RX FIFO of all received data.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  ESPUART_rxBufferWrite - cleared to zero.
    *  ESPUART_rxBufferRead - cleared to zero.
    *  ESPUART_rxBufferLoopDetect - cleared to zero.
    *  ESPUART_rxBufferOverflow - cleared to zero.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Setting the pointers to zero makes the system believe there is no data to
    *  read and writing will resume at address 0 overwriting any data that may
    *  have remained in the RAM.
    *
    * Side Effects:
    *  Any received data not read from the RAM or FIFO buffer will be lost.
    *
    *******************************************************************************/
    void ESPUART_ClearRxBuffer(void) 
    {
        uint8 enableInterrupts;

        /* Clear the HW FIFO */
        enableInterrupts = CyEnterCriticalSection();
        ESPUART_RXDATA_AUX_CTL_REG |= (uint8)  ESPUART_RX_FIFO_CLR;
        ESPUART_RXDATA_AUX_CTL_REG &= (uint8) ~ESPUART_RX_FIFO_CLR;
        CyExitCriticalSection(enableInterrupts);

    #if (ESPUART_RX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        ESPUART_DisableRxInt();

        ESPUART_rxBufferRead = 0u;
        ESPUART_rxBufferWrite = 0u;
        ESPUART_rxBufferLoopDetect = 0u;
        ESPUART_rxBufferOverflow = 0u;

        ESPUART_EnableRxInt();

    #endif /* (ESPUART_RX_INTERRUPT_ENABLED) */

    }


    /*******************************************************************************
    * Function Name: ESPUART_SetRxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Sets the software controlled Addressing mode used by the RX portion of the
    *  UART.
    *
    * Parameters:
    *  addressMode: Enumerated value indicating the mode of RX addressing
    *  ESPUART__B_UART__AM_SW_BYTE_BYTE -  Software Byte-by-Byte address
    *                                               detection
    *  ESPUART__B_UART__AM_SW_DETECT_TO_BUFFER - Software Detect to Buffer
    *                                               address detection
    *  ESPUART__B_UART__AM_HW_BYTE_BY_BYTE - Hardware Byte-by-Byte address
    *                                               detection
    *  ESPUART__B_UART__AM_HW_DETECT_TO_BUFFER - Hardware Detect to Buffer
    *                                               address detection
    *  ESPUART__B_UART__AM_NONE - No address detection
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  ESPUART_rxAddressMode - the parameter stored in this variable for
    *   the farther usage in RX ISR.
    *  ESPUART_rxAddressDetected - set to initial state (0).
    *
    *******************************************************************************/
    void ESPUART_SetRxAddressMode(uint8 addressMode)
                                                        
    {
        #if(ESPUART_RXHW_ADDRESS_ENABLED)
            #if(ESPUART_CONTROL_REG_REMOVED)
                if(0u != addressMode)
                {
                    /* Suppress compiler warning */
                }
            #else /* ESPUART_CONTROL_REG_REMOVED */
                uint8 tmpCtrl;
                tmpCtrl = ESPUART_CONTROL_REG & (uint8)~ESPUART_CTRL_RXADDR_MODE_MASK;
                tmpCtrl |= (uint8)(addressMode << ESPUART_CTRL_RXADDR_MODE0_SHIFT);
                ESPUART_CONTROL_REG = tmpCtrl;

                #if(ESPUART_RX_INTERRUPT_ENABLED && \
                   (ESPUART_RXBUFFERSIZE > ESPUART_FIFO_LENGTH) )
                    ESPUART_rxAddressMode = addressMode;
                    ESPUART_rxAddressDetected = 0u;
                #endif /* End ESPUART_RXBUFFERSIZE > ESPUART_FIFO_LENGTH*/
            #endif /* End ESPUART_CONTROL_REG_REMOVED */
        #else /* ESPUART_RXHW_ADDRESS_ENABLED */
            if(0u != addressMode)
            {
                /* Suppress compiler warning */
            }
        #endif /* End ESPUART_RXHW_ADDRESS_ENABLED */
    }


    /*******************************************************************************
    * Function Name: ESPUART_SetRxAddress1
    ********************************************************************************
    *
    * Summary:
    *  Sets the first of two hardware-detectable receiver addresses.
    *
    * Parameters:
    *  address: Address #1 for hardware address detection.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void ESPUART_SetRxAddress1(uint8 address) 
    {
        ESPUART_RXADDRESS1_REG = address;
    }


    /*******************************************************************************
    * Function Name: ESPUART_SetRxAddress2
    ********************************************************************************
    *
    * Summary:
    *  Sets the second of two hardware-detectable receiver addresses.
    *
    * Parameters:
    *  address: Address #2 for hardware address detection.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void ESPUART_SetRxAddress2(uint8 address) 
    {
        ESPUART_RXADDRESS2_REG = address;
    }

#endif  /* ESPUART_RX_ENABLED || ESPUART_HD_ENABLED*/


#if( (ESPUART_TX_ENABLED) || (ESPUART_HD_ENABLED) )
    /*******************************************************************************
    * Function Name: ESPUART_SetTxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the TX interrupt sources to be enabled, but does not enable the
    *  interrupt.
    *
    * Parameters:
    *  intSrc: Bit field containing the TX interrupt sources to enable
    *   ESPUART_TX_STS_COMPLETE        Interrupt on TX byte complete
    *   ESPUART_TX_STS_FIFO_EMPTY      Interrupt when TX FIFO is empty
    *   ESPUART_TX_STS_FIFO_FULL       Interrupt when TX FIFO is full
    *   ESPUART_TX_STS_FIFO_NOT_FULL   Interrupt when TX FIFO is not full
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void ESPUART_SetTxInterruptMode(uint8 intSrc) 
    {
        ESPUART_TXSTATUS_MASK_REG = intSrc;
    }


    /*******************************************************************************
    * Function Name: ESPUART_WriteTxData
    ********************************************************************************
    *
    * Summary:
    *  Places a byte of data into the transmit buffer to be sent when the bus is
    *  available without checking the TX status register. You must check status
    *  separately.
    *
    * Parameters:
    *  txDataByte: data byte
    *
    * Return:
    * None.
    *
    * Global Variables:
    *  ESPUART_txBuffer - RAM buffer pointer for save data for transmission
    *  ESPUART_txBufferWrite - cyclic index for write to txBuffer,
    *    incremented after each byte saved to buffer.
    *  ESPUART_txBufferRead - cyclic index for read from txBuffer,
    *    checked to identify the condition to write to FIFO directly or to TX buffer
    *  ESPUART_initVar - checked to identify that the component has been
    *    initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void ESPUART_WriteTxData(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function*/
        if(ESPUART_initVar != 0u)
        {
        #if (ESPUART_TX_INTERRUPT_ENABLED)

            /* Protect variables that could change on interrupt. */
            ESPUART_DisableTxInt();

            if( (ESPUART_txBufferRead == ESPUART_txBufferWrite) &&
                ((ESPUART_TXSTATUS_REG & ESPUART_TX_STS_FIFO_FULL) == 0u) )
            {
                /* Add directly to the FIFO. */
                ESPUART_TXDATA_REG = txDataByte;
            }
            else
            {
                if(ESPUART_txBufferWrite >= ESPUART_TX_BUFFER_SIZE)
                {
                    ESPUART_txBufferWrite = 0u;
                }

                ESPUART_txBuffer[ESPUART_txBufferWrite] = txDataByte;

                /* Add to the software buffer. */
                ESPUART_txBufferWrite++;
            }

            ESPUART_EnableTxInt();

        #else

            /* Add directly to the FIFO. */
            ESPUART_TXDATA_REG = txDataByte;

        #endif /*(ESPUART_TX_INTERRUPT_ENABLED) */
        }
    }


    /*******************************************************************************
    * Function Name: ESPUART_ReadTxStatus
    ********************************************************************************
    *
    * Summary:
    *  Reads the status register for the TX portion of the UART.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Contents of the status register
    *
    * Theory:
    *  This function reads the TX status register, which is cleared on read.
    *  It is up to the user to handle all bits in this return value accordingly,
    *  even if the bit was not enabled as an interrupt source the event happened
    *  and must be handled accordingly.
    *
    *******************************************************************************/
    uint8 ESPUART_ReadTxStatus(void) 
    {
        return(ESPUART_TXSTATUS_REG);
    }


    /*******************************************************************************
    * Function Name: ESPUART_PutChar
    ********************************************************************************
    *
    * Summary:
    *  Puts a byte of data into the transmit buffer to be sent when the bus is
    *  available. This is a blocking API that waits until the TX buffer has room to
    *  hold the data.
    *
    * Parameters:
    *  txDataByte: Byte containing the data to transmit
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  ESPUART_txBuffer - RAM buffer pointer for save data for transmission
    *  ESPUART_txBufferWrite - cyclic index for write to txBuffer,
    *     checked to identify free space in txBuffer and incremented after each byte
    *     saved to buffer.
    *  ESPUART_txBufferRead - cyclic index for read from txBuffer,
    *     checked to identify free space in txBuffer.
    *  ESPUART_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to transmit any byte of data in a single transfer
    *
    *******************************************************************************/
    void ESPUART_PutChar(uint8 txDataByte) 
    {
    #if (ESPUART_TX_INTERRUPT_ENABLED)
        /* The temporary output pointer is used since it takes two instructions
        *  to increment with a wrap, and we can't risk doing that with the real
        *  pointer and getting an interrupt in between instructions.
        */
        uint16 locTxBufferWrite;
        uint16 locTxBufferRead;

        do
        { /* Block if software buffer is full, so we don't overwrite. */

        #if ((ESPUART_TX_BUFFER_SIZE > ESPUART_MAX_BYTE_VALUE) && (CY_PSOC3))
            /* Disable TX interrupt to protect variables from modification */
            ESPUART_DisableTxInt();
        #endif /* (ESPUART_TX_BUFFER_SIZE > ESPUART_MAX_BYTE_VALUE) && (CY_PSOC3) */

            locTxBufferWrite = ESPUART_txBufferWrite;
            locTxBufferRead  = ESPUART_txBufferRead;

        #if ((ESPUART_TX_BUFFER_SIZE > ESPUART_MAX_BYTE_VALUE) && (CY_PSOC3))
            /* Enable interrupt to continue transmission */
            ESPUART_EnableTxInt();
        #endif /* (ESPUART_TX_BUFFER_SIZE > ESPUART_MAX_BYTE_VALUE) && (CY_PSOC3) */
        }
        while( (locTxBufferWrite < locTxBufferRead) ? (locTxBufferWrite == (locTxBufferRead - 1u)) :
                                ((locTxBufferWrite - locTxBufferRead) ==
                                (uint16)(ESPUART_TX_BUFFER_SIZE - 1u)) );

        if( (locTxBufferRead == locTxBufferWrite) &&
            ((ESPUART_TXSTATUS_REG & ESPUART_TX_STS_FIFO_FULL) == 0u) )
        {
            /* Add directly to the FIFO */
            ESPUART_TXDATA_REG = txDataByte;
        }
        else
        {
            if(locTxBufferWrite >= ESPUART_TX_BUFFER_SIZE)
            {
                locTxBufferWrite = 0u;
            }
            /* Add to the software buffer. */
            ESPUART_txBuffer[locTxBufferWrite] = txDataByte;
            locTxBufferWrite++;

            /* Finally, update the real output pointer */
        #if ((ESPUART_TX_BUFFER_SIZE > ESPUART_MAX_BYTE_VALUE) && (CY_PSOC3))
            ESPUART_DisableTxInt();
        #endif /* (ESPUART_TX_BUFFER_SIZE > ESPUART_MAX_BYTE_VALUE) && (CY_PSOC3) */

            ESPUART_txBufferWrite = locTxBufferWrite;

        #if ((ESPUART_TX_BUFFER_SIZE > ESPUART_MAX_BYTE_VALUE) && (CY_PSOC3))
            ESPUART_EnableTxInt();
        #endif /* (ESPUART_TX_BUFFER_SIZE > ESPUART_MAX_BYTE_VALUE) && (CY_PSOC3) */

            if(0u != (ESPUART_TXSTATUS_REG & ESPUART_TX_STS_FIFO_EMPTY))
            {
                /* Trigger TX interrupt to send software buffer */
                ESPUART_SetPendingTxInt();
            }
        }

    #else

        while((ESPUART_TXSTATUS_REG & ESPUART_TX_STS_FIFO_FULL) != 0u)
        {
            /* Wait for room in the FIFO */
        }

        /* Add directly to the FIFO */
        ESPUART_TXDATA_REG = txDataByte;

    #endif /* ESPUART_TX_INTERRUPT_ENABLED */
    }


    /*******************************************************************************
    * Function Name: ESPUART_PutString
    ********************************************************************************
    *
    * Summary:
    *  Sends a NULL terminated string to the TX buffer for transmission.
    *
    * Parameters:
    *  string[]: Pointer to the null terminated string array residing in RAM or ROM
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  ESPUART_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  If there is not enough memory in the TX buffer for the entire string, this
    *  function blocks until the last character of the string is loaded into the
    *  TX buffer.
    *
    *******************************************************************************/
    void ESPUART_PutString(const char8 string[]) 
    {
        uint16 bufIndex = 0u;

        /* If not Initialized then skip this function */
        if(ESPUART_initVar != 0u)
        {
            /* This is a blocking function, it will not exit until all data is sent */
            while(string[bufIndex] != (char8) 0)
            {
                ESPUART_PutChar((uint8)string[bufIndex]);
                bufIndex++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: ESPUART_PutArray
    ********************************************************************************
    *
    * Summary:
    *  Places N bytes of data from a memory array into the TX buffer for
    *  transmission.
    *
    * Parameters:
    *  string[]: Address of the memory array residing in RAM or ROM.
    *  byteCount: Number of bytes to be transmitted. The type depends on TX Buffer
    *             Size parameter.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  ESPUART_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  If there is not enough memory in the TX buffer for the entire string, this
    *  function blocks until the last character of the string is loaded into the
    *  TX buffer.
    *
    *******************************************************************************/
    void ESPUART_PutArray(const uint8 string[], uint16 byteCount)
                                                                    
    {
        uint16 bufIndex = 0u;

        /* If not Initialized then skip this function */
        if(ESPUART_initVar != 0u)
        {
            while(bufIndex < byteCount)
            {
                ESPUART_PutChar(string[bufIndex]);
                bufIndex++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: ESPUART_PutCRLF
    ********************************************************************************
    *
    * Summary:
    *  Writes a byte of data followed by a carriage return (0x0D) and line feed
    *  (0x0A) to the transmit buffer.
    *
    * Parameters:
    *  txDataByte: Data byte to transmit before the carriage return and line feed.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  ESPUART_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void ESPUART_PutCRLF(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function */
        if(ESPUART_initVar != 0u)
        {
            ESPUART_PutChar(txDataByte);
            ESPUART_PutChar(0x0Du);
            ESPUART_PutChar(0x0Au);
        }
    }


    /*******************************************************************************
    * Function Name: ESPUART_GetTxBufferSize
    ********************************************************************************
    *
    * Summary:
    *  Returns the number of bytes in the TX buffer which are waiting to be 
    *  transmitted.
    *  * TX software buffer is disabled (TX Buffer Size parameter is equal to 4): 
    *    returns 0 for empty TX FIFO, 1 for not full TX FIFO or 4 for full TX FIFO.
    *  * TX software buffer is enabled: returns the number of bytes in the TX 
    *    software buffer which are waiting to be transmitted. Bytes available in the
    *    TX FIFO do not count.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Number of bytes used in the TX buffer. Return value type depends on the TX 
    *  Buffer Size parameter.
    *
    * Global Variables:
    *  ESPUART_txBufferWrite - used to calculate left space.
    *  ESPUART_txBufferRead - used to calculate left space.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the TX Buffer is.
    *
    *******************************************************************************/
    uint16 ESPUART_GetTxBufferSize(void)
                                                            
    {
        uint16 size;

    #if (ESPUART_TX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        ESPUART_DisableTxInt();

        if(ESPUART_txBufferRead == ESPUART_txBufferWrite)
        {
            size = 0u;
        }
        else if(ESPUART_txBufferRead < ESPUART_txBufferWrite)
        {
            size = (ESPUART_txBufferWrite - ESPUART_txBufferRead);
        }
        else
        {
            size = (ESPUART_TX_BUFFER_SIZE - ESPUART_txBufferRead) +
                    ESPUART_txBufferWrite;
        }

        ESPUART_EnableTxInt();

    #else

        size = ESPUART_TXSTATUS_REG;

        /* Is the fifo is full. */
        if((size & ESPUART_TX_STS_FIFO_FULL) != 0u)
        {
            size = ESPUART_FIFO_LENGTH;
        }
        else if((size & ESPUART_TX_STS_FIFO_EMPTY) != 0u)
        {
            size = 0u;
        }
        else
        {
            /* We only know there is data in the fifo. */
            size = 1u;
        }

    #endif /* (ESPUART_TX_INTERRUPT_ENABLED) */

    return(size);
    }


    /*******************************************************************************
    * Function Name: ESPUART_ClearTxBuffer
    ********************************************************************************
    *
    * Summary:
    *  Clears all data from the TX buffer and hardware TX FIFO.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  ESPUART_txBufferWrite - cleared to zero.
    *  ESPUART_txBufferRead - cleared to zero.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Setting the pointers to zero makes the system believe there is no data to
    *  read and writing will resume at address 0 overwriting any data that may have
    *  remained in the RAM.
    *
    * Side Effects:
    *  Data waiting in the transmit buffer is not sent; a byte that is currently
    *  transmitting finishes transmitting.
    *
    *******************************************************************************/
    void ESPUART_ClearTxBuffer(void) 
    {
        uint8 enableInterrupts;

        enableInterrupts = CyEnterCriticalSection();
        /* Clear the HW FIFO */
        ESPUART_TXDATA_AUX_CTL_REG |= (uint8)  ESPUART_TX_FIFO_CLR;
        ESPUART_TXDATA_AUX_CTL_REG &= (uint8) ~ESPUART_TX_FIFO_CLR;
        CyExitCriticalSection(enableInterrupts);

    #if (ESPUART_TX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        ESPUART_DisableTxInt();

        ESPUART_txBufferRead = 0u;
        ESPUART_txBufferWrite = 0u;

        /* Enable Tx interrupt. */
        ESPUART_EnableTxInt();

    #endif /* (ESPUART_TX_INTERRUPT_ENABLED) */
    }


    /*******************************************************************************
    * Function Name: ESPUART_SendBreak
    ********************************************************************************
    *
    * Summary:
    *  Transmits a break signal on the bus.
    *
    * Parameters:
    *  uint8 retMode:  Send Break return mode. See the following table for options.
    *   ESPUART_SEND_BREAK - Initialize registers for break, send the Break
    *       signal and return immediately.
    *   ESPUART_WAIT_FOR_COMPLETE_REINIT - Wait until break transmission is
    *       complete, reinitialize registers to normal transmission mode then return
    *   ESPUART_REINIT - Reinitialize registers to normal transmission mode
    *       then return.
    *   ESPUART_SEND_WAIT_REINIT - Performs both options: 
    *      ESPUART_SEND_BREAK and ESPUART_WAIT_FOR_COMPLETE_REINIT.
    *      This option is recommended for most cases.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  ESPUART_initVar - checked to identify that the component has been
    *     initialized.
    *  txPeriod - static variable, used for keeping TX period configuration.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  SendBreak function initializes registers to send 13-bit break signal. It is
    *  important to return the registers configuration to normal for continue 8-bit
    *  operation.
    *  There are 3 variants for this API usage:
    *  1) SendBreak(3) - function will send the Break signal and take care on the
    *     configuration returning. Function will block CPU until transmission
    *     complete.
    *  2) User may want to use blocking time if UART configured to the low speed
    *     operation
    *     Example for this case:
    *     SendBreak(0);     - initialize Break signal transmission
    *         Add your code here to use CPU time
    *     SendBreak(1);     - complete Break operation
    *  3) Same to 2) but user may want to initialize and use the interrupt to
    *     complete break operation.
    *     Example for this case:
    *     Initialize TX interrupt with "TX - On TX Complete" parameter
    *     SendBreak(0);     - initialize Break signal transmission
    *         Add your code here to use CPU time
    *     When interrupt appear with ESPUART_TX_STS_COMPLETE status:
    *     SendBreak(2);     - complete Break operation
    *
    * Side Effects:
    *  The ESPUART_SendBreak() function initializes registers to send a
    *  break signal.
    *  Break signal length depends on the break signal bits configuration.
    *  The register configuration should be reinitialized before normal 8-bit
    *  communication can continue.
    *
    *******************************************************************************/
    void ESPUART_SendBreak(uint8 retMode) 
    {

        /* If not Initialized then skip this function*/
        if(ESPUART_initVar != 0u)
        {
            /* Set the Counter to 13-bits and transmit a 00 byte */
            /* When that is done then reset the counter value back */
            uint8 tmpStat;

        #if(ESPUART_HD_ENABLED) /* Half Duplex mode*/

            if( (retMode == ESPUART_SEND_BREAK) ||
                (retMode == ESPUART_SEND_WAIT_REINIT ) )
            {
                /* CTRL_HD_SEND_BREAK - sends break bits in HD mode */
                ESPUART_WriteControlRegister(ESPUART_ReadControlRegister() |
                                                      ESPUART_CTRL_HD_SEND_BREAK);
                /* Send zeros */
                ESPUART_TXDATA_REG = 0u;

                do /* Wait until transmit starts */
                {
                    tmpStat = ESPUART_TXSTATUS_REG;
                }
                while((tmpStat & ESPUART_TX_STS_FIFO_EMPTY) != 0u);
            }

            if( (retMode == ESPUART_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == ESPUART_SEND_WAIT_REINIT) )
            {
                do /* Wait until transmit complete */
                {
                    tmpStat = ESPUART_TXSTATUS_REG;
                }
                while(((uint8)~tmpStat & ESPUART_TX_STS_COMPLETE) != 0u);
            }

            if( (retMode == ESPUART_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == ESPUART_REINIT) ||
                (retMode == ESPUART_SEND_WAIT_REINIT) )
            {
                ESPUART_WriteControlRegister(ESPUART_ReadControlRegister() &
                                              (uint8)~ESPUART_CTRL_HD_SEND_BREAK);
            }

        #else /* ESPUART_HD_ENABLED Full Duplex mode */

            static uint8 txPeriod;

            if( (retMode == ESPUART_SEND_BREAK) ||
                (retMode == ESPUART_SEND_WAIT_REINIT) )
            {
                /* CTRL_HD_SEND_BREAK - skip to send parity bit at Break signal in Full Duplex mode */
                #if( (ESPUART_PARITY_TYPE != ESPUART__B_UART__NONE_REVB) || \
                                    (ESPUART_PARITY_TYPE_SW != 0u) )
                    ESPUART_WriteControlRegister(ESPUART_ReadControlRegister() |
                                                          ESPUART_CTRL_HD_SEND_BREAK);
                #endif /* End ESPUART_PARITY_TYPE != ESPUART__B_UART__NONE_REVB  */

                #if(ESPUART_TXCLKGEN_DP)
                    txPeriod = ESPUART_TXBITCLKTX_COMPLETE_REG;
                    ESPUART_TXBITCLKTX_COMPLETE_REG = ESPUART_TXBITCTR_BREAKBITS;
                #else
                    txPeriod = ESPUART_TXBITCTR_PERIOD_REG;
                    ESPUART_TXBITCTR_PERIOD_REG = ESPUART_TXBITCTR_BREAKBITS8X;
                #endif /* End ESPUART_TXCLKGEN_DP */

                /* Send zeros */
                ESPUART_TXDATA_REG = 0u;

                do /* Wait until transmit starts */
                {
                    tmpStat = ESPUART_TXSTATUS_REG;
                }
                while((tmpStat & ESPUART_TX_STS_FIFO_EMPTY) != 0u);
            }

            if( (retMode == ESPUART_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == ESPUART_SEND_WAIT_REINIT) )
            {
                do /* Wait until transmit complete */
                {
                    tmpStat = ESPUART_TXSTATUS_REG;
                }
                while(((uint8)~tmpStat & ESPUART_TX_STS_COMPLETE) != 0u);
            }

            if( (retMode == ESPUART_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == ESPUART_REINIT) ||
                (retMode == ESPUART_SEND_WAIT_REINIT) )
            {

            #if(ESPUART_TXCLKGEN_DP)
                ESPUART_TXBITCLKTX_COMPLETE_REG = txPeriod;
            #else
                ESPUART_TXBITCTR_PERIOD_REG = txPeriod;
            #endif /* End ESPUART_TXCLKGEN_DP */

            #if( (ESPUART_PARITY_TYPE != ESPUART__B_UART__NONE_REVB) || \
                 (ESPUART_PARITY_TYPE_SW != 0u) )
                ESPUART_WriteControlRegister(ESPUART_ReadControlRegister() &
                                                      (uint8) ~ESPUART_CTRL_HD_SEND_BREAK);
            #endif /* End ESPUART_PARITY_TYPE != NONE */
            }
        #endif    /* End ESPUART_HD_ENABLED */
        }
    }


    /*******************************************************************************
    * Function Name: ESPUART_SetTxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the transmitter to signal the next bytes is address or data.
    *
    * Parameters:
    *  addressMode: 
    *       ESPUART_SET_SPACE - Configure the transmitter to send the next
    *                                    byte as a data.
    *       ESPUART_SET_MARK  - Configure the transmitter to send the next
    *                                    byte as an address.
    *
    * Return:
    *  None.
    *
    * Side Effects:
    *  This function sets and clears ESPUART_CTRL_MARK bit in the Control
    *  register.
    *
    *******************************************************************************/
    void ESPUART_SetTxAddressMode(uint8 addressMode) 
    {
        /* Mark/Space sending enable */
        if(addressMode != 0u)
        {
        #if( ESPUART_CONTROL_REG_REMOVED == 0u )
            ESPUART_WriteControlRegister(ESPUART_ReadControlRegister() |
                                                  ESPUART_CTRL_MARK);
        #endif /* End ESPUART_CONTROL_REG_REMOVED == 0u */
        }
        else
        {
        #if( ESPUART_CONTROL_REG_REMOVED == 0u )
            ESPUART_WriteControlRegister(ESPUART_ReadControlRegister() &
                                                  (uint8) ~ESPUART_CTRL_MARK);
        #endif /* End ESPUART_CONTROL_REG_REMOVED == 0u */
        }
    }

#endif  /* EndESPUART_TX_ENABLED */

#if(ESPUART_HD_ENABLED)


    /*******************************************************************************
    * Function Name: ESPUART_LoadRxConfig
    ********************************************************************************
    *
    * Summary:
    *  Loads the receiver configuration in half duplex mode. After calling this
    *  function, the UART is ready to receive data.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Side Effects:
    *  Valid only in half duplex mode. You must make sure that the previous
    *  transaction is complete and it is safe to unload the transmitter
    *  configuration.
    *
    *******************************************************************************/
    void ESPUART_LoadRxConfig(void) 
    {
        ESPUART_WriteControlRegister(ESPUART_ReadControlRegister() &
                                                (uint8)~ESPUART_CTRL_HD_SEND);
        ESPUART_RXBITCTR_PERIOD_REG = ESPUART_HD_RXBITCTR_INIT;

    #if (ESPUART_RX_INTERRUPT_ENABLED)
        /* Enable RX interrupt after set RX configuration */
        ESPUART_SetRxInterruptMode(ESPUART_INIT_RX_INTERRUPTS_MASK);
    #endif /* (ESPUART_RX_INTERRUPT_ENABLED) */
    }


    /*******************************************************************************
    * Function Name: ESPUART_LoadTxConfig
    ********************************************************************************
    *
    * Summary:
    *  Loads the transmitter configuration in half duplex mode. After calling this
    *  function, the UART is ready to transmit data.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Side Effects:
    *  Valid only in half duplex mode. You must make sure that the previous
    *  transaction is complete and it is safe to unload the receiver configuration.
    *
    *******************************************************************************/
    void ESPUART_LoadTxConfig(void) 
    {
    #if (ESPUART_RX_INTERRUPT_ENABLED)
        /* Disable RX interrupts before set TX configuration */
        ESPUART_SetRxInterruptMode(0u);
    #endif /* (ESPUART_RX_INTERRUPT_ENABLED) */

        ESPUART_WriteControlRegister(ESPUART_ReadControlRegister() | ESPUART_CTRL_HD_SEND);
        ESPUART_RXBITCTR_PERIOD_REG = ESPUART_HD_TXBITCTR_INIT;
    }

#endif  /* ESPUART_HD_ENABLED */


/* [] END OF FILE */
