/*******************************************************************************
* File Name: ESPUART.h
* Version 2.50
*
* Description:
*  Contains the function prototypes and constants available to the UART
*  user module.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_UART_ESPUART_H)
#define CY_UART_ESPUART_H

#include "cyfitter.h"
#include "cytypes.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */


/***************************************
* Conditional Compilation Parameters
***************************************/

#define ESPUART_RX_ENABLED                     (1u)
#define ESPUART_TX_ENABLED                     (1u)
#define ESPUART_HD_ENABLED                     (0u)
#define ESPUART_RX_INTERRUPT_ENABLED           (1u)
#define ESPUART_TX_INTERRUPT_ENABLED           (1u)
#define ESPUART_INTERNAL_CLOCK_USED            (1u)
#define ESPUART_RXHW_ADDRESS_ENABLED           (0u)
#define ESPUART_OVER_SAMPLE_COUNT              (8u)
#define ESPUART_PARITY_TYPE                    (0u)
#define ESPUART_PARITY_TYPE_SW                 (1u)
#define ESPUART_BREAK_DETECT                   (0u)
#define ESPUART_BREAK_BITS_TX                  (13u)
#define ESPUART_BREAK_BITS_RX                  (13u)
#define ESPUART_TXCLKGEN_DP                    (1u)
#define ESPUART_USE23POLLING                   (1u)
#define ESPUART_FLOW_CONTROL                   (0u)
#define ESPUART_CLK_FREQ                       (0u)
#define ESPUART_TX_BUFFER_SIZE                 (2000u)
#define ESPUART_RX_BUFFER_SIZE                 (2000u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component UART_v2_50 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */

#if defined(ESPUART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG)
    #define ESPUART_CONTROL_REG_REMOVED            (0u)
#else
    #define ESPUART_CONTROL_REG_REMOVED            (1u)
#endif /* End ESPUART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */


/***************************************
*      Data Structure Definition
***************************************/

/* Sleep Mode API Support */
typedef struct ESPUART_backupStruct_
{
    uint8 enableState;

    #if(ESPUART_CONTROL_REG_REMOVED == 0u)
        uint8 cr;
    #endif /* End ESPUART_CONTROL_REG_REMOVED */

} ESPUART_BACKUP_STRUCT;


/***************************************
*       Function Prototypes
***************************************/

void ESPUART_Start(void) ;
void ESPUART_Stop(void) ;
uint8 ESPUART_ReadControlRegister(void) ;
void ESPUART_WriteControlRegister(uint8 control) ;

void ESPUART_Init(void) ;
void ESPUART_Enable(void) ;
void ESPUART_SaveConfig(void) ;
void ESPUART_RestoreConfig(void) ;
void ESPUART_Sleep(void) ;
void ESPUART_Wakeup(void) ;

/* Only if RX is enabled */
#if( (ESPUART_RX_ENABLED) || (ESPUART_HD_ENABLED) )

    #if (ESPUART_RX_INTERRUPT_ENABLED)
        #define ESPUART_EnableRxInt()  CyIntEnable (ESPUART_RX_VECT_NUM)
        #define ESPUART_DisableRxInt() CyIntDisable(ESPUART_RX_VECT_NUM)
        CY_ISR_PROTO(ESPUART_RXISR);
    #endif /* ESPUART_RX_INTERRUPT_ENABLED */

    void ESPUART_SetRxAddressMode(uint8 addressMode)
                                                           ;
    void ESPUART_SetRxAddress1(uint8 address) ;
    void ESPUART_SetRxAddress2(uint8 address) ;

    void  ESPUART_SetRxInterruptMode(uint8 intSrc) ;
    uint8 ESPUART_ReadRxData(void) ;
    uint8 ESPUART_ReadRxStatus(void) ;
    uint8 ESPUART_GetChar(void) ;
    uint16 ESPUART_GetByte(void) ;
    uint16 ESPUART_GetRxBufferSize(void)
                                                            ;
    void ESPUART_ClearRxBuffer(void) ;

    /* Obsolete functions, defines for backward compatible */
    #define ESPUART_GetRxInterruptSource   ESPUART_ReadRxStatus

#endif /* End (ESPUART_RX_ENABLED) || (ESPUART_HD_ENABLED) */

/* Only if TX is enabled */
#if(ESPUART_TX_ENABLED || ESPUART_HD_ENABLED)

    #if(ESPUART_TX_INTERRUPT_ENABLED)
        #define ESPUART_EnableTxInt()  CyIntEnable (ESPUART_TX_VECT_NUM)
        #define ESPUART_DisableTxInt() CyIntDisable(ESPUART_TX_VECT_NUM)
        #define ESPUART_SetPendingTxInt() CyIntSetPending(ESPUART_TX_VECT_NUM)
        #define ESPUART_ClearPendingTxInt() CyIntClearPending(ESPUART_TX_VECT_NUM)
        CY_ISR_PROTO(ESPUART_TXISR);
    #endif /* ESPUART_TX_INTERRUPT_ENABLED */

    void ESPUART_SetTxInterruptMode(uint8 intSrc) ;
    void ESPUART_WriteTxData(uint8 txDataByte) ;
    uint8 ESPUART_ReadTxStatus(void) ;
    void ESPUART_PutChar(uint8 txDataByte) ;
    void ESPUART_PutString(const char8 string[]) ;
    void ESPUART_PutArray(const uint8 string[], uint16 byteCount)
                                                            ;
    void ESPUART_PutCRLF(uint8 txDataByte) ;
    void ESPUART_ClearTxBuffer(void) ;
    void ESPUART_SetTxAddressMode(uint8 addressMode) ;
    void ESPUART_SendBreak(uint8 retMode) ;
    uint16 ESPUART_GetTxBufferSize(void)
                                                            ;
    /* Obsolete functions, defines for backward compatible */
    #define ESPUART_PutStringConst         ESPUART_PutString
    #define ESPUART_PutArrayConst          ESPUART_PutArray
    #define ESPUART_GetTxInterruptSource   ESPUART_ReadTxStatus

#endif /* End ESPUART_TX_ENABLED || ESPUART_HD_ENABLED */

#if(ESPUART_HD_ENABLED)
    void ESPUART_LoadRxConfig(void) ;
    void ESPUART_LoadTxConfig(void) ;
#endif /* End ESPUART_HD_ENABLED */


/* Communication bootloader APIs */
#if defined(CYDEV_BOOTLOADER_IO_COMP) && ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_ESPUART) || \
                                          (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))
    /* Physical layer functions */
    void    ESPUART_CyBtldrCommStart(void) CYSMALL ;
    void    ESPUART_CyBtldrCommStop(void) CYSMALL ;
    void    ESPUART_CyBtldrCommReset(void) CYSMALL ;
    cystatus ESPUART_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;
    cystatus ESPUART_CyBtldrCommRead(uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;

    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_ESPUART)
        #define CyBtldrCommStart    ESPUART_CyBtldrCommStart
        #define CyBtldrCommStop     ESPUART_CyBtldrCommStop
        #define CyBtldrCommReset    ESPUART_CyBtldrCommReset
        #define CyBtldrCommWrite    ESPUART_CyBtldrCommWrite
        #define CyBtldrCommRead     ESPUART_CyBtldrCommRead
    #endif  /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_ESPUART) */

    /* Byte to Byte time out for detecting end of block data from host */
    #define ESPUART_BYTE2BYTE_TIME_OUT (25u)
    #define ESPUART_PACKET_EOP         (0x17u) /* End of packet defined by bootloader */
    #define ESPUART_WAIT_EOP_DELAY     (5u)    /* Additional 5ms to wait for End of packet */
    #define ESPUART_BL_CHK_DELAY_MS    (1u)    /* Time Out quantity equal 1mS */

#endif /* CYDEV_BOOTLOADER_IO_COMP */


/***************************************
*          API Constants
***************************************/
/* Parameters for SetTxAddressMode API*/
#define ESPUART_SET_SPACE      (0x00u)
#define ESPUART_SET_MARK       (0x01u)

/* Status Register definitions */
#if( (ESPUART_TX_ENABLED) || (ESPUART_HD_ENABLED) )
    #if(ESPUART_TX_INTERRUPT_ENABLED)
        #define ESPUART_TX_VECT_NUM            (uint8)ESPUART_TXInternalInterrupt__INTC_NUMBER
        #define ESPUART_TX_PRIOR_NUM           (uint8)ESPUART_TXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* ESPUART_TX_INTERRUPT_ENABLED */

    #define ESPUART_TX_STS_COMPLETE_SHIFT          (0x00u)
    #define ESPUART_TX_STS_FIFO_EMPTY_SHIFT        (0x01u)
    #define ESPUART_TX_STS_FIFO_NOT_FULL_SHIFT     (0x03u)
    #if(ESPUART_TX_ENABLED)
        #define ESPUART_TX_STS_FIFO_FULL_SHIFT     (0x02u)
    #else /* (ESPUART_HD_ENABLED) */
        #define ESPUART_TX_STS_FIFO_FULL_SHIFT     (0x05u)  /* Needs MD=0 */
    #endif /* (ESPUART_TX_ENABLED) */

    #define ESPUART_TX_STS_COMPLETE            (uint8)(0x01u << ESPUART_TX_STS_COMPLETE_SHIFT)
    #define ESPUART_TX_STS_FIFO_EMPTY          (uint8)(0x01u << ESPUART_TX_STS_FIFO_EMPTY_SHIFT)
    #define ESPUART_TX_STS_FIFO_FULL           (uint8)(0x01u << ESPUART_TX_STS_FIFO_FULL_SHIFT)
    #define ESPUART_TX_STS_FIFO_NOT_FULL       (uint8)(0x01u << ESPUART_TX_STS_FIFO_NOT_FULL_SHIFT)
#endif /* End (ESPUART_TX_ENABLED) || (ESPUART_HD_ENABLED)*/

#if( (ESPUART_RX_ENABLED) || (ESPUART_HD_ENABLED) )
    #if(ESPUART_RX_INTERRUPT_ENABLED)
        #define ESPUART_RX_VECT_NUM            (uint8)ESPUART_RXInternalInterrupt__INTC_NUMBER
        #define ESPUART_RX_PRIOR_NUM           (uint8)ESPUART_RXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* ESPUART_RX_INTERRUPT_ENABLED */
    #define ESPUART_RX_STS_MRKSPC_SHIFT            (0x00u)
    #define ESPUART_RX_STS_BREAK_SHIFT             (0x01u)
    #define ESPUART_RX_STS_PAR_ERROR_SHIFT         (0x02u)
    #define ESPUART_RX_STS_STOP_ERROR_SHIFT        (0x03u)
    #define ESPUART_RX_STS_OVERRUN_SHIFT           (0x04u)
    #define ESPUART_RX_STS_FIFO_NOTEMPTY_SHIFT     (0x05u)
    #define ESPUART_RX_STS_ADDR_MATCH_SHIFT        (0x06u)
    #define ESPUART_RX_STS_SOFT_BUFF_OVER_SHIFT    (0x07u)

    #define ESPUART_RX_STS_MRKSPC           (uint8)(0x01u << ESPUART_RX_STS_MRKSPC_SHIFT)
    #define ESPUART_RX_STS_BREAK            (uint8)(0x01u << ESPUART_RX_STS_BREAK_SHIFT)
    #define ESPUART_RX_STS_PAR_ERROR        (uint8)(0x01u << ESPUART_RX_STS_PAR_ERROR_SHIFT)
    #define ESPUART_RX_STS_STOP_ERROR       (uint8)(0x01u << ESPUART_RX_STS_STOP_ERROR_SHIFT)
    #define ESPUART_RX_STS_OVERRUN          (uint8)(0x01u << ESPUART_RX_STS_OVERRUN_SHIFT)
    #define ESPUART_RX_STS_FIFO_NOTEMPTY    (uint8)(0x01u << ESPUART_RX_STS_FIFO_NOTEMPTY_SHIFT)
    #define ESPUART_RX_STS_ADDR_MATCH       (uint8)(0x01u << ESPUART_RX_STS_ADDR_MATCH_SHIFT)
    #define ESPUART_RX_STS_SOFT_BUFF_OVER   (uint8)(0x01u << ESPUART_RX_STS_SOFT_BUFF_OVER_SHIFT)
    #define ESPUART_RX_HW_MASK                     (0x7Fu)
#endif /* End (ESPUART_RX_ENABLED) || (ESPUART_HD_ENABLED) */

/* Control Register definitions */
#define ESPUART_CTRL_HD_SEND_SHIFT                 (0x00u) /* 1 enable TX part in Half Duplex mode */
#define ESPUART_CTRL_HD_SEND_BREAK_SHIFT           (0x01u) /* 1 send BREAK signal in Half Duplez mode */
#define ESPUART_CTRL_MARK_SHIFT                    (0x02u) /* 1 sets mark, 0 sets space */
#define ESPUART_CTRL_PARITY_TYPE0_SHIFT            (0x03u) /* Defines the type of parity implemented */
#define ESPUART_CTRL_PARITY_TYPE1_SHIFT            (0x04u) /* Defines the type of parity implemented */
#define ESPUART_CTRL_RXADDR_MODE0_SHIFT            (0x05u)
#define ESPUART_CTRL_RXADDR_MODE1_SHIFT            (0x06u)
#define ESPUART_CTRL_RXADDR_MODE2_SHIFT            (0x07u)

#define ESPUART_CTRL_HD_SEND               (uint8)(0x01u << ESPUART_CTRL_HD_SEND_SHIFT)
#define ESPUART_CTRL_HD_SEND_BREAK         (uint8)(0x01u << ESPUART_CTRL_HD_SEND_BREAK_SHIFT)
#define ESPUART_CTRL_MARK                  (uint8)(0x01u << ESPUART_CTRL_MARK_SHIFT)
#define ESPUART_CTRL_PARITY_TYPE_MASK      (uint8)(0x03u << ESPUART_CTRL_PARITY_TYPE0_SHIFT)
#define ESPUART_CTRL_RXADDR_MODE_MASK      (uint8)(0x07u << ESPUART_CTRL_RXADDR_MODE0_SHIFT)

/* StatusI Register Interrupt Enable Control Bits. As defined by the Register map for the AUX Control Register */
#define ESPUART_INT_ENABLE                         (0x10u)

/* Bit Counter (7-bit) Control Register Bit Definitions. As defined by the Register map for the AUX Control Register */
#define ESPUART_CNTR_ENABLE                        (0x20u)

/*   Constants for SendBreak() "retMode" parameter  */
#define ESPUART_SEND_BREAK                         (0x00u)
#define ESPUART_WAIT_FOR_COMPLETE_REINIT           (0x01u)
#define ESPUART_REINIT                             (0x02u)
#define ESPUART_SEND_WAIT_REINIT                   (0x03u)

#define ESPUART_OVER_SAMPLE_8                      (8u)
#define ESPUART_OVER_SAMPLE_16                     (16u)

#define ESPUART_BIT_CENTER                         (ESPUART_OVER_SAMPLE_COUNT - 2u)

#define ESPUART_FIFO_LENGTH                        (4u)
#define ESPUART_NUMBER_OF_START_BIT                (1u)
#define ESPUART_MAX_BYTE_VALUE                     (0xFFu)

/* 8X always for count7 implementation */
#define ESPUART_TXBITCTR_BREAKBITS8X   ((ESPUART_BREAK_BITS_TX * ESPUART_OVER_SAMPLE_8) - 1u)
/* 8X or 16X for DP implementation */
#define ESPUART_TXBITCTR_BREAKBITS ((ESPUART_BREAK_BITS_TX * ESPUART_OVER_SAMPLE_COUNT) - 1u)

#define ESPUART_HALF_BIT_COUNT   \
                            (((ESPUART_OVER_SAMPLE_COUNT / 2u) + (ESPUART_USE23POLLING * 1u)) - 2u)
#if (ESPUART_OVER_SAMPLE_COUNT == ESPUART_OVER_SAMPLE_8)
    #define ESPUART_HD_TXBITCTR_INIT   (((ESPUART_BREAK_BITS_TX + \
                            ESPUART_NUMBER_OF_START_BIT) * ESPUART_OVER_SAMPLE_COUNT) - 1u)

    /* This parameter is increased on the 2 in 2 out of 3 mode to sample voting in the middle */
    #define ESPUART_RXBITCTR_INIT  ((((ESPUART_BREAK_BITS_RX + ESPUART_NUMBER_OF_START_BIT) \
                            * ESPUART_OVER_SAMPLE_COUNT) + ESPUART_HALF_BIT_COUNT) - 1u)

#else /* ESPUART_OVER_SAMPLE_COUNT == ESPUART_OVER_SAMPLE_16 */
    #define ESPUART_HD_TXBITCTR_INIT   ((8u * ESPUART_OVER_SAMPLE_COUNT) - 1u)
    /* 7bit counter need one more bit for OverSampleCount = 16 */
    #define ESPUART_RXBITCTR_INIT      (((7u * ESPUART_OVER_SAMPLE_COUNT) - 1u) + \
                                                      ESPUART_HALF_BIT_COUNT)
#endif /* End ESPUART_OVER_SAMPLE_COUNT */

#define ESPUART_HD_RXBITCTR_INIT                   ESPUART_RXBITCTR_INIT


/***************************************
* Global variables external identifier
***************************************/

extern uint8 ESPUART_initVar;
#if (ESPUART_TX_INTERRUPT_ENABLED && ESPUART_TX_ENABLED)
    extern volatile uint8 ESPUART_txBuffer[ESPUART_TX_BUFFER_SIZE];
    extern volatile uint16 ESPUART_txBufferRead;
    extern uint16 ESPUART_txBufferWrite;
#endif /* (ESPUART_TX_INTERRUPT_ENABLED && ESPUART_TX_ENABLED) */
#if (ESPUART_RX_INTERRUPT_ENABLED && (ESPUART_RX_ENABLED || ESPUART_HD_ENABLED))
    extern uint8 ESPUART_errorStatus;
    extern volatile uint8 ESPUART_rxBuffer[ESPUART_RX_BUFFER_SIZE];
    extern volatile uint16 ESPUART_rxBufferRead;
    extern volatile uint16 ESPUART_rxBufferWrite;
    extern volatile uint8 ESPUART_rxBufferLoopDetect;
    extern volatile uint8 ESPUART_rxBufferOverflow;
    #if (ESPUART_RXHW_ADDRESS_ENABLED)
        extern volatile uint8 ESPUART_rxAddressMode;
        extern volatile uint8 ESPUART_rxAddressDetected;
    #endif /* (ESPUART_RXHW_ADDRESS_ENABLED) */
#endif /* (ESPUART_RX_INTERRUPT_ENABLED && (ESPUART_RX_ENABLED || ESPUART_HD_ENABLED)) */


/***************************************
* Enumerated Types and Parameters
***************************************/

#define ESPUART__B_UART__AM_SW_BYTE_BYTE 1
#define ESPUART__B_UART__AM_SW_DETECT_TO_BUFFER 2
#define ESPUART__B_UART__AM_HW_BYTE_BY_BYTE 3
#define ESPUART__B_UART__AM_HW_DETECT_TO_BUFFER 4
#define ESPUART__B_UART__AM_NONE 0

#define ESPUART__B_UART__NONE_REVB 0
#define ESPUART__B_UART__EVEN_REVB 1
#define ESPUART__B_UART__ODD_REVB 2
#define ESPUART__B_UART__MARK_SPACE_REVB 3



/***************************************
*    Initial Parameter Constants
***************************************/

/* UART shifts max 8 bits, Mark/Space functionality working if 9 selected */
#define ESPUART_NUMBER_OF_DATA_BITS    ((8u > 8u) ? 8u : 8u)
#define ESPUART_NUMBER_OF_STOP_BITS    (1u)

#if (ESPUART_RXHW_ADDRESS_ENABLED)
    #define ESPUART_RX_ADDRESS_MODE    (0u)
    #define ESPUART_RX_HW_ADDRESS1     (0u)
    #define ESPUART_RX_HW_ADDRESS2     (0u)
#endif /* (ESPUART_RXHW_ADDRESS_ENABLED) */

#define ESPUART_INIT_RX_INTERRUPTS_MASK \
                                  (uint8)((1 << ESPUART_RX_STS_FIFO_NOTEMPTY_SHIFT) \
                                        | (0 << ESPUART_RX_STS_MRKSPC_SHIFT) \
                                        | (0 << ESPUART_RX_STS_ADDR_MATCH_SHIFT) \
                                        | (0 << ESPUART_RX_STS_PAR_ERROR_SHIFT) \
                                        | (0 << ESPUART_RX_STS_STOP_ERROR_SHIFT) \
                                        | (0 << ESPUART_RX_STS_BREAK_SHIFT) \
                                        | (0 << ESPUART_RX_STS_OVERRUN_SHIFT))

#define ESPUART_INIT_TX_INTERRUPTS_MASK \
                                  (uint8)((0 << ESPUART_TX_STS_COMPLETE_SHIFT) \
                                        | (1 << ESPUART_TX_STS_FIFO_EMPTY_SHIFT) \
                                        | (0 << ESPUART_TX_STS_FIFO_FULL_SHIFT) \
                                        | (0 << ESPUART_TX_STS_FIFO_NOT_FULL_SHIFT))


/***************************************
*              Registers
***************************************/

#ifdef ESPUART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define ESPUART_CONTROL_REG \
                            (* (reg8 *) ESPUART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
    #define ESPUART_CONTROL_PTR \
                            (  (reg8 *) ESPUART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
#endif /* End ESPUART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(ESPUART_TX_ENABLED)
    #define ESPUART_TXDATA_REG          (* (reg8 *) ESPUART_BUART_sTX_TxShifter_u0__F0_REG)
    #define ESPUART_TXDATA_PTR          (  (reg8 *) ESPUART_BUART_sTX_TxShifter_u0__F0_REG)
    #define ESPUART_TXDATA_AUX_CTL_REG  (* (reg8 *) ESPUART_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define ESPUART_TXDATA_AUX_CTL_PTR  (  (reg8 *) ESPUART_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define ESPUART_TXSTATUS_REG        (* (reg8 *) ESPUART_BUART_sTX_TxSts__STATUS_REG)
    #define ESPUART_TXSTATUS_PTR        (  (reg8 *) ESPUART_BUART_sTX_TxSts__STATUS_REG)
    #define ESPUART_TXSTATUS_MASK_REG   (* (reg8 *) ESPUART_BUART_sTX_TxSts__MASK_REG)
    #define ESPUART_TXSTATUS_MASK_PTR   (  (reg8 *) ESPUART_BUART_sTX_TxSts__MASK_REG)
    #define ESPUART_TXSTATUS_ACTL_REG   (* (reg8 *) ESPUART_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)
    #define ESPUART_TXSTATUS_ACTL_PTR   (  (reg8 *) ESPUART_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)

    /* DP clock */
    #if(ESPUART_TXCLKGEN_DP)
        #define ESPUART_TXBITCLKGEN_CTR_REG        \
                                        (* (reg8 *) ESPUART_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define ESPUART_TXBITCLKGEN_CTR_PTR        \
                                        (  (reg8 *) ESPUART_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define ESPUART_TXBITCLKTX_COMPLETE_REG    \
                                        (* (reg8 *) ESPUART_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
        #define ESPUART_TXBITCLKTX_COMPLETE_PTR    \
                                        (  (reg8 *) ESPUART_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
    #else     /* Count7 clock*/
        #define ESPUART_TXBITCTR_PERIOD_REG    \
                                        (* (reg8 *) ESPUART_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define ESPUART_TXBITCTR_PERIOD_PTR    \
                                        (  (reg8 *) ESPUART_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define ESPUART_TXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) ESPUART_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define ESPUART_TXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) ESPUART_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define ESPUART_TXBITCTR_COUNTER_REG   \
                                        (* (reg8 *) ESPUART_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
        #define ESPUART_TXBITCTR_COUNTER_PTR   \
                                        (  (reg8 *) ESPUART_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
    #endif /* ESPUART_TXCLKGEN_DP */

#endif /* End ESPUART_TX_ENABLED */

#if(ESPUART_HD_ENABLED)

    #define ESPUART_TXDATA_REG             (* (reg8 *) ESPUART_BUART_sRX_RxShifter_u0__F1_REG )
    #define ESPUART_TXDATA_PTR             (  (reg8 *) ESPUART_BUART_sRX_RxShifter_u0__F1_REG )
    #define ESPUART_TXDATA_AUX_CTL_REG     (* (reg8 *) ESPUART_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)
    #define ESPUART_TXDATA_AUX_CTL_PTR     (  (reg8 *) ESPUART_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define ESPUART_TXSTATUS_REG           (* (reg8 *) ESPUART_BUART_sRX_RxSts__STATUS_REG )
    #define ESPUART_TXSTATUS_PTR           (  (reg8 *) ESPUART_BUART_sRX_RxSts__STATUS_REG )
    #define ESPUART_TXSTATUS_MASK_REG      (* (reg8 *) ESPUART_BUART_sRX_RxSts__MASK_REG )
    #define ESPUART_TXSTATUS_MASK_PTR      (  (reg8 *) ESPUART_BUART_sRX_RxSts__MASK_REG )
    #define ESPUART_TXSTATUS_ACTL_REG      (* (reg8 *) ESPUART_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define ESPUART_TXSTATUS_ACTL_PTR      (  (reg8 *) ESPUART_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End ESPUART_HD_ENABLED */

#if( (ESPUART_RX_ENABLED) || (ESPUART_HD_ENABLED) )
    #define ESPUART_RXDATA_REG             (* (reg8 *) ESPUART_BUART_sRX_RxShifter_u0__F0_REG )
    #define ESPUART_RXDATA_PTR             (  (reg8 *) ESPUART_BUART_sRX_RxShifter_u0__F0_REG )
    #define ESPUART_RXADDRESS1_REG         (* (reg8 *) ESPUART_BUART_sRX_RxShifter_u0__D0_REG )
    #define ESPUART_RXADDRESS1_PTR         (  (reg8 *) ESPUART_BUART_sRX_RxShifter_u0__D0_REG )
    #define ESPUART_RXADDRESS2_REG         (* (reg8 *) ESPUART_BUART_sRX_RxShifter_u0__D1_REG )
    #define ESPUART_RXADDRESS2_PTR         (  (reg8 *) ESPUART_BUART_sRX_RxShifter_u0__D1_REG )
    #define ESPUART_RXDATA_AUX_CTL_REG     (* (reg8 *) ESPUART_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define ESPUART_RXBITCTR_PERIOD_REG    (* (reg8 *) ESPUART_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define ESPUART_RXBITCTR_PERIOD_PTR    (  (reg8 *) ESPUART_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define ESPUART_RXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) ESPUART_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define ESPUART_RXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) ESPUART_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define ESPUART_RXBITCTR_COUNTER_REG   (* (reg8 *) ESPUART_BUART_sRX_RxBitCounter__COUNT_REG )
    #define ESPUART_RXBITCTR_COUNTER_PTR   (  (reg8 *) ESPUART_BUART_sRX_RxBitCounter__COUNT_REG )

    #define ESPUART_RXSTATUS_REG           (* (reg8 *) ESPUART_BUART_sRX_RxSts__STATUS_REG )
    #define ESPUART_RXSTATUS_PTR           (  (reg8 *) ESPUART_BUART_sRX_RxSts__STATUS_REG )
    #define ESPUART_RXSTATUS_MASK_REG      (* (reg8 *) ESPUART_BUART_sRX_RxSts__MASK_REG )
    #define ESPUART_RXSTATUS_MASK_PTR      (  (reg8 *) ESPUART_BUART_sRX_RxSts__MASK_REG )
    #define ESPUART_RXSTATUS_ACTL_REG      (* (reg8 *) ESPUART_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define ESPUART_RXSTATUS_ACTL_PTR      (  (reg8 *) ESPUART_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End  (ESPUART_RX_ENABLED) || (ESPUART_HD_ENABLED) */

#if(ESPUART_INTERNAL_CLOCK_USED)
    /* Register to enable or disable the digital clocks */
    #define ESPUART_INTCLOCK_CLKEN_REG     (* (reg8 *) ESPUART_IntClock__PM_ACT_CFG)
    #define ESPUART_INTCLOCK_CLKEN_PTR     (  (reg8 *) ESPUART_IntClock__PM_ACT_CFG)

    /* Clock mask for this clock. */
    #define ESPUART_INTCLOCK_CLKEN_MASK    ESPUART_IntClock__PM_ACT_MSK
#endif /* End ESPUART_INTERNAL_CLOCK_USED */


/***************************************
*       Register Constants
***************************************/

#if(ESPUART_TX_ENABLED)
    #define ESPUART_TX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End ESPUART_TX_ENABLED */

#if(ESPUART_HD_ENABLED)
    #define ESPUART_TX_FIFO_CLR            (0x02u) /* FIFO1 CLR */
#endif /* End ESPUART_HD_ENABLED */

#if( (ESPUART_RX_ENABLED) || (ESPUART_HD_ENABLED) )
    #define ESPUART_RX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End  (ESPUART_RX_ENABLED) || (ESPUART_HD_ENABLED) */


/***************************************
* The following code is DEPRECATED and
* should not be used in new projects.
***************************************/

/* UART v2_40 obsolete definitions */
#define ESPUART_WAIT_1_MS      ESPUART_BL_CHK_DELAY_MS   

#define ESPUART_TXBUFFERSIZE   ESPUART_TX_BUFFER_SIZE
#define ESPUART_RXBUFFERSIZE   ESPUART_RX_BUFFER_SIZE

#if (ESPUART_RXHW_ADDRESS_ENABLED)
    #define ESPUART_RXADDRESSMODE  ESPUART_RX_ADDRESS_MODE
    #define ESPUART_RXHWADDRESS1   ESPUART_RX_HW_ADDRESS1
    #define ESPUART_RXHWADDRESS2   ESPUART_RX_HW_ADDRESS2
    /* Backward compatible define */
    #define ESPUART_RXAddressMode  ESPUART_RXADDRESSMODE
#endif /* (ESPUART_RXHW_ADDRESS_ENABLED) */

/* UART v2_30 obsolete definitions */
#define ESPUART_initvar                    ESPUART_initVar

#define ESPUART_RX_Enabled                 ESPUART_RX_ENABLED
#define ESPUART_TX_Enabled                 ESPUART_TX_ENABLED
#define ESPUART_HD_Enabled                 ESPUART_HD_ENABLED
#define ESPUART_RX_IntInterruptEnabled     ESPUART_RX_INTERRUPT_ENABLED
#define ESPUART_TX_IntInterruptEnabled     ESPUART_TX_INTERRUPT_ENABLED
#define ESPUART_InternalClockUsed          ESPUART_INTERNAL_CLOCK_USED
#define ESPUART_RXHW_Address_Enabled       ESPUART_RXHW_ADDRESS_ENABLED
#define ESPUART_OverSampleCount            ESPUART_OVER_SAMPLE_COUNT
#define ESPUART_ParityType                 ESPUART_PARITY_TYPE

#if( ESPUART_TX_ENABLED && (ESPUART_TXBUFFERSIZE > ESPUART_FIFO_LENGTH))
    #define ESPUART_TXBUFFER               ESPUART_txBuffer
    #define ESPUART_TXBUFFERREAD           ESPUART_txBufferRead
    #define ESPUART_TXBUFFERWRITE          ESPUART_txBufferWrite
#endif /* End ESPUART_TX_ENABLED */
#if( ( ESPUART_RX_ENABLED || ESPUART_HD_ENABLED ) && \
     (ESPUART_RXBUFFERSIZE > ESPUART_FIFO_LENGTH) )
    #define ESPUART_RXBUFFER               ESPUART_rxBuffer
    #define ESPUART_RXBUFFERREAD           ESPUART_rxBufferRead
    #define ESPUART_RXBUFFERWRITE          ESPUART_rxBufferWrite
    #define ESPUART_RXBUFFERLOOPDETECT     ESPUART_rxBufferLoopDetect
    #define ESPUART_RXBUFFER_OVERFLOW      ESPUART_rxBufferOverflow
#endif /* End ESPUART_RX_ENABLED */

#ifdef ESPUART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define ESPUART_CONTROL                ESPUART_CONTROL_REG
#endif /* End ESPUART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(ESPUART_TX_ENABLED)
    #define ESPUART_TXDATA                 ESPUART_TXDATA_REG
    #define ESPUART_TXSTATUS               ESPUART_TXSTATUS_REG
    #define ESPUART_TXSTATUS_MASK          ESPUART_TXSTATUS_MASK_REG
    #define ESPUART_TXSTATUS_ACTL          ESPUART_TXSTATUS_ACTL_REG
    /* DP clock */
    #if(ESPUART_TXCLKGEN_DP)
        #define ESPUART_TXBITCLKGEN_CTR        ESPUART_TXBITCLKGEN_CTR_REG
        #define ESPUART_TXBITCLKTX_COMPLETE    ESPUART_TXBITCLKTX_COMPLETE_REG
    #else     /* Count7 clock*/
        #define ESPUART_TXBITCTR_PERIOD        ESPUART_TXBITCTR_PERIOD_REG
        #define ESPUART_TXBITCTR_CONTROL       ESPUART_TXBITCTR_CONTROL_REG
        #define ESPUART_TXBITCTR_COUNTER       ESPUART_TXBITCTR_COUNTER_REG
    #endif /* ESPUART_TXCLKGEN_DP */
#endif /* End ESPUART_TX_ENABLED */

#if(ESPUART_HD_ENABLED)
    #define ESPUART_TXDATA                 ESPUART_TXDATA_REG
    #define ESPUART_TXSTATUS               ESPUART_TXSTATUS_REG
    #define ESPUART_TXSTATUS_MASK          ESPUART_TXSTATUS_MASK_REG
    #define ESPUART_TXSTATUS_ACTL          ESPUART_TXSTATUS_ACTL_REG
#endif /* End ESPUART_HD_ENABLED */

#if( (ESPUART_RX_ENABLED) || (ESPUART_HD_ENABLED) )
    #define ESPUART_RXDATA                 ESPUART_RXDATA_REG
    #define ESPUART_RXADDRESS1             ESPUART_RXADDRESS1_REG
    #define ESPUART_RXADDRESS2             ESPUART_RXADDRESS2_REG
    #define ESPUART_RXBITCTR_PERIOD        ESPUART_RXBITCTR_PERIOD_REG
    #define ESPUART_RXBITCTR_CONTROL       ESPUART_RXBITCTR_CONTROL_REG
    #define ESPUART_RXBITCTR_COUNTER       ESPUART_RXBITCTR_COUNTER_REG
    #define ESPUART_RXSTATUS               ESPUART_RXSTATUS_REG
    #define ESPUART_RXSTATUS_MASK          ESPUART_RXSTATUS_MASK_REG
    #define ESPUART_RXSTATUS_ACTL          ESPUART_RXSTATUS_ACTL_REG
#endif /* End  (ESPUART_RX_ENABLED) || (ESPUART_HD_ENABLED) */

#if(ESPUART_INTERNAL_CLOCK_USED)
    #define ESPUART_INTCLOCK_CLKEN         ESPUART_INTCLOCK_CLKEN_REG
#endif /* End ESPUART_INTERNAL_CLOCK_USED */

#define ESPUART_WAIT_FOR_COMLETE_REINIT    ESPUART_WAIT_FOR_COMPLETE_REINIT

#endif  /* CY_UART_ESPUART_H */


/* [] END OF FILE */
