/*******************************************************************************
* File Name: mistTimer.h
* Version 2.80
*
*  Description:
*     Contains the function prototypes and constants available to the timer
*     user module.
*
*   Note:
*     None
*
********************************************************************************
* Copyright 2008-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#if !defined(CY_TIMER_mistTimer_H)
#define CY_TIMER_mistTimer_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */

extern uint8 mistTimer_initVar;

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component Timer_v2_80 requires cy_boot v3.0 or later
#endif /* (CY_ PSOC5LP) */


/**************************************
*           Parameter Defaults
**************************************/

#define mistTimer_Resolution                 16u
#define mistTimer_UsingFixedFunction         1u
#define mistTimer_UsingHWCaptureCounter      0u
#define mistTimer_SoftwareCaptureMode        0u
#define mistTimer_SoftwareTriggerMode        0u
#define mistTimer_UsingHWEnable              0u
#define mistTimer_EnableTriggerMode          0u
#define mistTimer_InterruptOnCaptureCount    0u
#define mistTimer_RunModeUsed                0u
#define mistTimer_ControlRegRemoved          0u

#if defined(mistTimer_TimerUDB_sCTRLReg_SyncCtl_ctrlreg__CONTROL_REG)
    #define mistTimer_UDB_CONTROL_REG_REMOVED            (0u)
#elif  (mistTimer_UsingFixedFunction)
    #define mistTimer_UDB_CONTROL_REG_REMOVED            (0u)
#else 
    #define mistTimer_UDB_CONTROL_REG_REMOVED            (1u)
#endif /* End mistTimer_TimerUDB_sCTRLReg_SyncCtl_ctrlreg__CONTROL_REG */


/***************************************
*       Type defines
***************************************/


/**************************************************************************
 * Sleep Wakeup Backup structure for Timer Component
 *************************************************************************/
typedef struct
{
    uint8 TimerEnableState;
    #if(!mistTimer_UsingFixedFunction)

        uint16 TimerUdb;
        uint8 InterruptMaskValue;
        #if (mistTimer_UsingHWCaptureCounter)
            uint8 TimerCaptureCounter;
        #endif /* variable declarations for backing up non retention registers in CY_UDB_V1 */

        #if (!mistTimer_UDB_CONTROL_REG_REMOVED)
            uint8 TimerControlRegister;
        #endif /* variable declaration for backing up enable state of the Timer */
    #endif /* define backup variables only for UDB implementation. Fixed function registers are all retention */

}mistTimer_backupStruct;


/***************************************
*       Function Prototypes
***************************************/

void    mistTimer_Start(void) ;
void    mistTimer_Stop(void) ;

void    mistTimer_SetInterruptMode(uint8 interruptMode) ;
uint8   mistTimer_ReadStatusRegister(void) ;
/* Deprecated function. Do not use this in future. Retained for backward compatibility */
#define mistTimer_GetInterruptSource() mistTimer_ReadStatusRegister()

#if(!mistTimer_UDB_CONTROL_REG_REMOVED)
    uint8   mistTimer_ReadControlRegister(void) ;
    void    mistTimer_WriteControlRegister(uint8 control) ;
#endif /* (!mistTimer_UDB_CONTROL_REG_REMOVED) */

uint16  mistTimer_ReadPeriod(void) ;
void    mistTimer_WritePeriod(uint16 period) ;
uint16  mistTimer_ReadCounter(void) ;
void    mistTimer_WriteCounter(uint16 counter) ;
uint16  mistTimer_ReadCapture(void) ;
void    mistTimer_SoftwareCapture(void) ;

#if(!mistTimer_UsingFixedFunction) /* UDB Prototypes */
    #if (mistTimer_SoftwareCaptureMode)
        void    mistTimer_SetCaptureMode(uint8 captureMode) ;
    #endif /* (!mistTimer_UsingFixedFunction) */

    #if (mistTimer_SoftwareTriggerMode)
        void    mistTimer_SetTriggerMode(uint8 triggerMode) ;
    #endif /* (mistTimer_SoftwareTriggerMode) */

    #if (mistTimer_EnableTriggerMode)
        void    mistTimer_EnableTrigger(void) ;
        void    mistTimer_DisableTrigger(void) ;
    #endif /* (mistTimer_EnableTriggerMode) */


    #if(mistTimer_InterruptOnCaptureCount)
        void    mistTimer_SetInterruptCount(uint8 interruptCount) ;
    #endif /* (mistTimer_InterruptOnCaptureCount) */

    #if (mistTimer_UsingHWCaptureCounter)
        void    mistTimer_SetCaptureCount(uint8 captureCount) ;
        uint8   mistTimer_ReadCaptureCount(void) ;
    #endif /* (mistTimer_UsingHWCaptureCounter) */

    void mistTimer_ClearFIFO(void) ;
#endif /* UDB Prototypes */

/* Sleep Retention APIs */
void mistTimer_Init(void)          ;
void mistTimer_Enable(void)        ;
void mistTimer_SaveConfig(void)    ;
void mistTimer_RestoreConfig(void) ;
void mistTimer_Sleep(void)         ;
void mistTimer_Wakeup(void)        ;


/***************************************
*   Enumerated Types and Parameters
***************************************/

/* Enumerated Type B_Timer__CaptureModes, Used in Capture Mode */
#define mistTimer__B_TIMER__CM_NONE 0
#define mistTimer__B_TIMER__CM_RISINGEDGE 1
#define mistTimer__B_TIMER__CM_FALLINGEDGE 2
#define mistTimer__B_TIMER__CM_EITHEREDGE 3
#define mistTimer__B_TIMER__CM_SOFTWARE 4



/* Enumerated Type B_Timer__TriggerModes, Used in Trigger Mode */
#define mistTimer__B_TIMER__TM_NONE 0x00u
#define mistTimer__B_TIMER__TM_RISINGEDGE 0x04u
#define mistTimer__B_TIMER__TM_FALLINGEDGE 0x08u
#define mistTimer__B_TIMER__TM_EITHEREDGE 0x0Cu
#define mistTimer__B_TIMER__TM_SOFTWARE 0x10u


/***************************************
*    Initialial Parameter Constants
***************************************/

#define mistTimer_INIT_PERIOD             599u
#define mistTimer_INIT_CAPTURE_MODE       ((uint8)((uint8)1u << mistTimer_CTRL_CAP_MODE_SHIFT))
#define mistTimer_INIT_TRIGGER_MODE       ((uint8)((uint8)0u << mistTimer_CTRL_TRIG_MODE_SHIFT))
#if (mistTimer_UsingFixedFunction)
    #define mistTimer_INIT_INTERRUPT_MODE (((uint8)((uint8)0u << mistTimer_STATUS_TC_INT_MASK_SHIFT)) | \
                                                  ((uint8)((uint8)0 << mistTimer_STATUS_CAPTURE_INT_MASK_SHIFT)))
#else
    #define mistTimer_INIT_INTERRUPT_MODE (((uint8)((uint8)0u << mistTimer_STATUS_TC_INT_MASK_SHIFT)) | \
                                                 ((uint8)((uint8)0 << mistTimer_STATUS_CAPTURE_INT_MASK_SHIFT)) | \
                                                 ((uint8)((uint8)0 << mistTimer_STATUS_FIFOFULL_INT_MASK_SHIFT)))
#endif /* (mistTimer_UsingFixedFunction) */
#define mistTimer_INIT_CAPTURE_COUNT      (2u)
#define mistTimer_INIT_INT_CAPTURE_COUNT  ((uint8)((uint8)(1u - 1u) << mistTimer_CTRL_INTCNT_SHIFT))


/***************************************
*           Registers
***************************************/

#if (mistTimer_UsingFixedFunction) /* Implementation Specific Registers and Register Constants */


    /***************************************
    *    Fixed Function Registers
    ***************************************/

    #define mistTimer_STATUS         (*(reg8 *) mistTimer_TimerHW__SR0 )
    /* In Fixed Function Block Status and Mask are the same register */
    #define mistTimer_STATUS_MASK    (*(reg8 *) mistTimer_TimerHW__SR0 )
    #define mistTimer_CONTROL        (*(reg8 *) mistTimer_TimerHW__CFG0)
    #define mistTimer_CONTROL2       (*(reg8 *) mistTimer_TimerHW__CFG1)
    #define mistTimer_CONTROL2_PTR   ( (reg8 *) mistTimer_TimerHW__CFG1)
    #define mistTimer_RT1            (*(reg8 *) mistTimer_TimerHW__RT1)
    #define mistTimer_RT1_PTR        ( (reg8 *) mistTimer_TimerHW__RT1)

    #if (CY_PSOC3 || CY_PSOC5LP)
        #define mistTimer_CONTROL3       (*(reg8 *) mistTimer_TimerHW__CFG2)
        #define mistTimer_CONTROL3_PTR   ( (reg8 *) mistTimer_TimerHW__CFG2)
    #endif /* (CY_PSOC3 || CY_PSOC5LP) */
    #define mistTimer_GLOBAL_ENABLE  (*(reg8 *) mistTimer_TimerHW__PM_ACT_CFG)
    #define mistTimer_GLOBAL_STBY_ENABLE  (*(reg8 *) mistTimer_TimerHW__PM_STBY_CFG)

    #define mistTimer_CAPTURE_LSB         (* (reg16 *) mistTimer_TimerHW__CAP0 )
    #define mistTimer_CAPTURE_LSB_PTR       ((reg16 *) mistTimer_TimerHW__CAP0 )
    #define mistTimer_PERIOD_LSB          (* (reg16 *) mistTimer_TimerHW__PER0 )
    #define mistTimer_PERIOD_LSB_PTR        ((reg16 *) mistTimer_TimerHW__PER0 )
    #define mistTimer_COUNTER_LSB         (* (reg16 *) mistTimer_TimerHW__CNT_CMP0 )
    #define mistTimer_COUNTER_LSB_PTR       ((reg16 *) mistTimer_TimerHW__CNT_CMP0 )


    /***************************************
    *    Register Constants
    ***************************************/

    /* Fixed Function Block Chosen */
    #define mistTimer_BLOCK_EN_MASK                     mistTimer_TimerHW__PM_ACT_MSK
    #define mistTimer_BLOCK_STBY_EN_MASK                mistTimer_TimerHW__PM_STBY_MSK

    /* Control Register Bit Locations */
    /* Interrupt Count - Not valid for Fixed Function Block */
    #define mistTimer_CTRL_INTCNT_SHIFT                  0x00u
    /* Trigger Polarity - Not valid for Fixed Function Block */
    #define mistTimer_CTRL_TRIG_MODE_SHIFT               0x00u
    /* Trigger Enable - Not valid for Fixed Function Block */
    #define mistTimer_CTRL_TRIG_EN_SHIFT                 0x00u
    /* Capture Polarity - Not valid for Fixed Function Block */
    #define mistTimer_CTRL_CAP_MODE_SHIFT                0x00u
    /* Timer Enable - As defined in Register Map, part of TMRX_CFG0 register */
    #define mistTimer_CTRL_ENABLE_SHIFT                  0x00u

    /* Control Register Bit Masks */
    #define mistTimer_CTRL_ENABLE                        ((uint8)((uint8)0x01u << mistTimer_CTRL_ENABLE_SHIFT))

    /* Control2 Register Bit Masks */
    /* As defined in Register Map, Part of the TMRX_CFG1 register */
    #define mistTimer_CTRL2_IRQ_SEL_SHIFT                 0x00u
    #define mistTimer_CTRL2_IRQ_SEL                      ((uint8)((uint8)0x01u << mistTimer_CTRL2_IRQ_SEL_SHIFT))

    #if (CY_PSOC5A)
        /* Use CFG1 Mode bits to set run mode */
        /* As defined by Verilog Implementation */
        #define mistTimer_CTRL_MODE_SHIFT                 0x01u
        #define mistTimer_CTRL_MODE_MASK                 ((uint8)((uint8)0x07u << mistTimer_CTRL_MODE_SHIFT))
    #endif /* (CY_PSOC5A) */
    #if (CY_PSOC3 || CY_PSOC5LP)
        /* Control3 Register Bit Locations */
        #define mistTimer_CTRL_RCOD_SHIFT        0x02u
        #define mistTimer_CTRL_ENBL_SHIFT        0x00u
        #define mistTimer_CTRL_MODE_SHIFT        0x00u

        /* Control3 Register Bit Masks */
        #define mistTimer_CTRL_RCOD_MASK  ((uint8)((uint8)0x03u << mistTimer_CTRL_RCOD_SHIFT)) /* ROD and COD bit masks */
        #define mistTimer_CTRL_ENBL_MASK  ((uint8)((uint8)0x80u << mistTimer_CTRL_ENBL_SHIFT)) /* HW_EN bit mask */
        #define mistTimer_CTRL_MODE_MASK  ((uint8)((uint8)0x03u << mistTimer_CTRL_MODE_SHIFT)) /* Run mode bit mask */

        #define mistTimer_CTRL_RCOD       ((uint8)((uint8)0x03u << mistTimer_CTRL_RCOD_SHIFT))
        #define mistTimer_CTRL_ENBL       ((uint8)((uint8)0x80u << mistTimer_CTRL_ENBL_SHIFT))
    #endif /* (CY_PSOC3 || CY_PSOC5LP) */

    /*RT1 Synch Constants: Applicable for PSoC3 and PSoC5LP */
    #define mistTimer_RT1_SHIFT                       0x04u
    /* Sync TC and CMP bit masks */
    #define mistTimer_RT1_MASK                        ((uint8)((uint8)0x03u << mistTimer_RT1_SHIFT))
    #define mistTimer_SYNC                            ((uint8)((uint8)0x03u << mistTimer_RT1_SHIFT))
    #define mistTimer_SYNCDSI_SHIFT                   0x00u
    /* Sync all DSI inputs with Mask  */
    #define mistTimer_SYNCDSI_MASK                    ((uint8)((uint8)0x0Fu << mistTimer_SYNCDSI_SHIFT))
    /* Sync all DSI inputs */
    #define mistTimer_SYNCDSI_EN                      ((uint8)((uint8)0x0Fu << mistTimer_SYNCDSI_SHIFT))

    #define mistTimer_CTRL_MODE_PULSEWIDTH            ((uint8)((uint8)0x01u << mistTimer_CTRL_MODE_SHIFT))
    #define mistTimer_CTRL_MODE_PERIOD                ((uint8)((uint8)0x02u << mistTimer_CTRL_MODE_SHIFT))
    #define mistTimer_CTRL_MODE_CONTINUOUS            ((uint8)((uint8)0x00u << mistTimer_CTRL_MODE_SHIFT))

    /* Status Register Bit Locations */
    /* As defined in Register Map, part of TMRX_SR0 register */
    #define mistTimer_STATUS_TC_SHIFT                 0x07u
    /* As defined in Register Map, part of TMRX_SR0 register, Shared with Compare Status */
    #define mistTimer_STATUS_CAPTURE_SHIFT            0x06u
    /* As defined in Register Map, part of TMRX_SR0 register */
    #define mistTimer_STATUS_TC_INT_MASK_SHIFT        (mistTimer_STATUS_TC_SHIFT - 0x04u)
    /* As defined in Register Map, part of TMRX_SR0 register, Shared with Compare Status */
    #define mistTimer_STATUS_CAPTURE_INT_MASK_SHIFT   (mistTimer_STATUS_CAPTURE_SHIFT - 0x04u)

    /* Status Register Bit Masks */
    #define mistTimer_STATUS_TC                       ((uint8)((uint8)0x01u << mistTimer_STATUS_TC_SHIFT))
    #define mistTimer_STATUS_CAPTURE                  ((uint8)((uint8)0x01u << mistTimer_STATUS_CAPTURE_SHIFT))
    /* Interrupt Enable Bit-Mask for interrupt on TC */
    #define mistTimer_STATUS_TC_INT_MASK              ((uint8)((uint8)0x01u << mistTimer_STATUS_TC_INT_MASK_SHIFT))
    /* Interrupt Enable Bit-Mask for interrupt on Capture */
    #define mistTimer_STATUS_CAPTURE_INT_MASK         ((uint8)((uint8)0x01u << mistTimer_STATUS_CAPTURE_INT_MASK_SHIFT))

#else   /* UDB Registers and Register Constants */


    /***************************************
    *           UDB Registers
    ***************************************/

    #define mistTimer_STATUS              (* (reg8 *) mistTimer_TimerUDB_rstSts_stsreg__STATUS_REG )
    #define mistTimer_STATUS_MASK         (* (reg8 *) mistTimer_TimerUDB_rstSts_stsreg__MASK_REG)
    #define mistTimer_STATUS_AUX_CTRL     (* (reg8 *) mistTimer_TimerUDB_rstSts_stsreg__STATUS_AUX_CTL_REG)
    #define mistTimer_CONTROL             (* (reg8 *) mistTimer_TimerUDB_sCTRLReg_SyncCtl_ctrlreg__CONTROL_REG )
    
    #if(mistTimer_Resolution <= 8u) /* 8-bit Timer */
        #define mistTimer_CAPTURE_LSB         (* (reg8 *) mistTimer_TimerUDB_sT16_timerdp_u0__F0_REG )
        #define mistTimer_CAPTURE_LSB_PTR       ((reg8 *) mistTimer_TimerUDB_sT16_timerdp_u0__F0_REG )
        #define mistTimer_PERIOD_LSB          (* (reg8 *) mistTimer_TimerUDB_sT16_timerdp_u0__D0_REG )
        #define mistTimer_PERIOD_LSB_PTR        ((reg8 *) mistTimer_TimerUDB_sT16_timerdp_u0__D0_REG )
        #define mistTimer_COUNTER_LSB         (* (reg8 *) mistTimer_TimerUDB_sT16_timerdp_u0__A0_REG )
        #define mistTimer_COUNTER_LSB_PTR       ((reg8 *) mistTimer_TimerUDB_sT16_timerdp_u0__A0_REG )
    #elif(mistTimer_Resolution <= 16u) /* 8-bit Timer */
        #if(CY_PSOC3) /* 8-bit addres space */
            #define mistTimer_CAPTURE_LSB         (* (reg16 *) mistTimer_TimerUDB_sT16_timerdp_u0__F0_REG )
            #define mistTimer_CAPTURE_LSB_PTR       ((reg16 *) mistTimer_TimerUDB_sT16_timerdp_u0__F0_REG )
            #define mistTimer_PERIOD_LSB          (* (reg16 *) mistTimer_TimerUDB_sT16_timerdp_u0__D0_REG )
            #define mistTimer_PERIOD_LSB_PTR        ((reg16 *) mistTimer_TimerUDB_sT16_timerdp_u0__D0_REG )
            #define mistTimer_COUNTER_LSB         (* (reg16 *) mistTimer_TimerUDB_sT16_timerdp_u0__A0_REG )
            #define mistTimer_COUNTER_LSB_PTR       ((reg16 *) mistTimer_TimerUDB_sT16_timerdp_u0__A0_REG )
        #else /* 16-bit address space */
            #define mistTimer_CAPTURE_LSB         (* (reg16 *) mistTimer_TimerUDB_sT16_timerdp_u0__16BIT_F0_REG )
            #define mistTimer_CAPTURE_LSB_PTR       ((reg16 *) mistTimer_TimerUDB_sT16_timerdp_u0__16BIT_F0_REG )
            #define mistTimer_PERIOD_LSB          (* (reg16 *) mistTimer_TimerUDB_sT16_timerdp_u0__16BIT_D0_REG )
            #define mistTimer_PERIOD_LSB_PTR        ((reg16 *) mistTimer_TimerUDB_sT16_timerdp_u0__16BIT_D0_REG )
            #define mistTimer_COUNTER_LSB         (* (reg16 *) mistTimer_TimerUDB_sT16_timerdp_u0__16BIT_A0_REG )
            #define mistTimer_COUNTER_LSB_PTR       ((reg16 *) mistTimer_TimerUDB_sT16_timerdp_u0__16BIT_A0_REG )
        #endif /* CY_PSOC3 */
    #elif(mistTimer_Resolution <= 24u)/* 24-bit Timer */
        #define mistTimer_CAPTURE_LSB         (* (reg32 *) mistTimer_TimerUDB_sT16_timerdp_u0__F0_REG )
        #define mistTimer_CAPTURE_LSB_PTR       ((reg32 *) mistTimer_TimerUDB_sT16_timerdp_u0__F0_REG )
        #define mistTimer_PERIOD_LSB          (* (reg32 *) mistTimer_TimerUDB_sT16_timerdp_u0__D0_REG )
        #define mistTimer_PERIOD_LSB_PTR        ((reg32 *) mistTimer_TimerUDB_sT16_timerdp_u0__D0_REG )
        #define mistTimer_COUNTER_LSB         (* (reg32 *) mistTimer_TimerUDB_sT16_timerdp_u0__A0_REG )
        #define mistTimer_COUNTER_LSB_PTR       ((reg32 *) mistTimer_TimerUDB_sT16_timerdp_u0__A0_REG )
    #else /* 32-bit Timer */
        #if(CY_PSOC3 || CY_PSOC5) /* 8-bit address space */
            #define mistTimer_CAPTURE_LSB         (* (reg32 *) mistTimer_TimerUDB_sT16_timerdp_u0__F0_REG )
            #define mistTimer_CAPTURE_LSB_PTR       ((reg32 *) mistTimer_TimerUDB_sT16_timerdp_u0__F0_REG )
            #define mistTimer_PERIOD_LSB          (* (reg32 *) mistTimer_TimerUDB_sT16_timerdp_u0__D0_REG )
            #define mistTimer_PERIOD_LSB_PTR        ((reg32 *) mistTimer_TimerUDB_sT16_timerdp_u0__D0_REG )
            #define mistTimer_COUNTER_LSB         (* (reg32 *) mistTimer_TimerUDB_sT16_timerdp_u0__A0_REG )
            #define mistTimer_COUNTER_LSB_PTR       ((reg32 *) mistTimer_TimerUDB_sT16_timerdp_u0__A0_REG )
        #else /* 32-bit address space */
            #define mistTimer_CAPTURE_LSB         (* (reg32 *) mistTimer_TimerUDB_sT16_timerdp_u0__32BIT_F0_REG )
            #define mistTimer_CAPTURE_LSB_PTR       ((reg32 *) mistTimer_TimerUDB_sT16_timerdp_u0__32BIT_F0_REG )
            #define mistTimer_PERIOD_LSB          (* (reg32 *) mistTimer_TimerUDB_sT16_timerdp_u0__32BIT_D0_REG )
            #define mistTimer_PERIOD_LSB_PTR        ((reg32 *) mistTimer_TimerUDB_sT16_timerdp_u0__32BIT_D0_REG )
            #define mistTimer_COUNTER_LSB         (* (reg32 *) mistTimer_TimerUDB_sT16_timerdp_u0__32BIT_A0_REG )
            #define mistTimer_COUNTER_LSB_PTR       ((reg32 *) mistTimer_TimerUDB_sT16_timerdp_u0__32BIT_A0_REG )
        #endif /* CY_PSOC3 || CY_PSOC5 */ 
    #endif

    #define mistTimer_COUNTER_LSB_PTR_8BIT       ((reg8 *) mistTimer_TimerUDB_sT16_timerdp_u0__A0_REG )
    
    #if (mistTimer_UsingHWCaptureCounter)
        #define mistTimer_CAP_COUNT              (*(reg8 *) mistTimer_TimerUDB_sCapCount_counter__PERIOD_REG )
        #define mistTimer_CAP_COUNT_PTR          ( (reg8 *) mistTimer_TimerUDB_sCapCount_counter__PERIOD_REG )
        #define mistTimer_CAPTURE_COUNT_CTRL     (*(reg8 *) mistTimer_TimerUDB_sCapCount_counter__CONTROL_AUX_CTL_REG )
        #define mistTimer_CAPTURE_COUNT_CTRL_PTR ( (reg8 *) mistTimer_TimerUDB_sCapCount_counter__CONTROL_AUX_CTL_REG )
    #endif /* (mistTimer_UsingHWCaptureCounter) */


    /***************************************
    *       Register Constants
    ***************************************/

    /* Control Register Bit Locations */
    #define mistTimer_CTRL_INTCNT_SHIFT              0x00u       /* As defined by Verilog Implementation */
    #define mistTimer_CTRL_TRIG_MODE_SHIFT           0x02u       /* As defined by Verilog Implementation */
    #define mistTimer_CTRL_TRIG_EN_SHIFT             0x04u       /* As defined by Verilog Implementation */
    #define mistTimer_CTRL_CAP_MODE_SHIFT            0x05u       /* As defined by Verilog Implementation */
    #define mistTimer_CTRL_ENABLE_SHIFT              0x07u       /* As defined by Verilog Implementation */

    /* Control Register Bit Masks */
    #define mistTimer_CTRL_INTCNT_MASK               ((uint8)((uint8)0x03u << mistTimer_CTRL_INTCNT_SHIFT))
    #define mistTimer_CTRL_TRIG_MODE_MASK            ((uint8)((uint8)0x03u << mistTimer_CTRL_TRIG_MODE_SHIFT))
    #define mistTimer_CTRL_TRIG_EN                   ((uint8)((uint8)0x01u << mistTimer_CTRL_TRIG_EN_SHIFT))
    #define mistTimer_CTRL_CAP_MODE_MASK             ((uint8)((uint8)0x03u << mistTimer_CTRL_CAP_MODE_SHIFT))
    #define mistTimer_CTRL_ENABLE                    ((uint8)((uint8)0x01u << mistTimer_CTRL_ENABLE_SHIFT))

    /* Bit Counter (7-bit) Control Register Bit Definitions */
    /* As defined by the Register map for the AUX Control Register */
    #define mistTimer_CNTR_ENABLE                    0x20u

    /* Status Register Bit Locations */
    #define mistTimer_STATUS_TC_SHIFT                0x00u  /* As defined by Verilog Implementation */
    #define mistTimer_STATUS_CAPTURE_SHIFT           0x01u  /* As defined by Verilog Implementation */
    #define mistTimer_STATUS_TC_INT_MASK_SHIFT       mistTimer_STATUS_TC_SHIFT
    #define mistTimer_STATUS_CAPTURE_INT_MASK_SHIFT  mistTimer_STATUS_CAPTURE_SHIFT
    #define mistTimer_STATUS_FIFOFULL_SHIFT          0x02u  /* As defined by Verilog Implementation */
    #define mistTimer_STATUS_FIFONEMP_SHIFT          0x03u  /* As defined by Verilog Implementation */
    #define mistTimer_STATUS_FIFOFULL_INT_MASK_SHIFT mistTimer_STATUS_FIFOFULL_SHIFT

    /* Status Register Bit Masks */
    /* Sticky TC Event Bit-Mask */
    #define mistTimer_STATUS_TC                      ((uint8)((uint8)0x01u << mistTimer_STATUS_TC_SHIFT))
    /* Sticky Capture Event Bit-Mask */
    #define mistTimer_STATUS_CAPTURE                 ((uint8)((uint8)0x01u << mistTimer_STATUS_CAPTURE_SHIFT))
    /* Interrupt Enable Bit-Mask */
    #define mistTimer_STATUS_TC_INT_MASK             ((uint8)((uint8)0x01u << mistTimer_STATUS_TC_SHIFT))
    /* Interrupt Enable Bit-Mask */
    #define mistTimer_STATUS_CAPTURE_INT_MASK        ((uint8)((uint8)0x01u << mistTimer_STATUS_CAPTURE_SHIFT))
    /* NOT-Sticky FIFO Full Bit-Mask */
    #define mistTimer_STATUS_FIFOFULL                ((uint8)((uint8)0x01u << mistTimer_STATUS_FIFOFULL_SHIFT))
    /* NOT-Sticky FIFO Not Empty Bit-Mask */
    #define mistTimer_STATUS_FIFONEMP                ((uint8)((uint8)0x01u << mistTimer_STATUS_FIFONEMP_SHIFT))
    /* Interrupt Enable Bit-Mask */
    #define mistTimer_STATUS_FIFOFULL_INT_MASK       ((uint8)((uint8)0x01u << mistTimer_STATUS_FIFOFULL_SHIFT))

    #define mistTimer_STATUS_ACTL_INT_EN             0x10u   /* As defined for the ACTL Register */

    /* Datapath Auxillary Control Register definitions */
    #define mistTimer_AUX_CTRL_FIFO0_CLR             0x01u   /* As defined by Register map */
    #define mistTimer_AUX_CTRL_FIFO1_CLR             0x02u   /* As defined by Register map */
    #define mistTimer_AUX_CTRL_FIFO0_LVL             0x04u   /* As defined by Register map */
    #define mistTimer_AUX_CTRL_FIFO1_LVL             0x08u   /* As defined by Register map */
    #define mistTimer_STATUS_ACTL_INT_EN_MASK        0x10u   /* As defined for the ACTL Register */

#endif /* Implementation Specific Registers and Register Constants */

#endif  /* CY_TIMER_mistTimer_H */


/* [] END OF FILE */
