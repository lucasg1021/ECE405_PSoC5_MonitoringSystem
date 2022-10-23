/*******************************************************************************
* File Name: DEBOUNCE_CLK.h
* Version 2.20
*
*  Description:
*   Provides the function and constant definitions for the clock component.
*
*  Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CLOCK_DEBOUNCE_CLK_H)
#define CY_CLOCK_DEBOUNCE_CLK_H

#include <cytypes.h>
#include <cyfitter.h>


/***************************************
* Conditional Compilation Parameters
***************************************/

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component cy_clock_v2_20 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */


/***************************************
*        Function Prototypes
***************************************/

void DEBOUNCE_CLK_Start(void) ;
void DEBOUNCE_CLK_Stop(void) ;

#if(CY_PSOC3 || CY_PSOC5LP)
void DEBOUNCE_CLK_StopBlock(void) ;
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

void DEBOUNCE_CLK_StandbyPower(uint8 state) ;
void DEBOUNCE_CLK_SetDividerRegister(uint16 clkDivider, uint8 restart) 
                                ;
uint16 DEBOUNCE_CLK_GetDividerRegister(void) ;
void DEBOUNCE_CLK_SetModeRegister(uint8 modeBitMask) ;
void DEBOUNCE_CLK_ClearModeRegister(uint8 modeBitMask) ;
uint8 DEBOUNCE_CLK_GetModeRegister(void) ;
void DEBOUNCE_CLK_SetSourceRegister(uint8 clkSource) ;
uint8 DEBOUNCE_CLK_GetSourceRegister(void) ;
#if defined(DEBOUNCE_CLK__CFG3)
void DEBOUNCE_CLK_SetPhaseRegister(uint8 clkPhase) ;
uint8 DEBOUNCE_CLK_GetPhaseRegister(void) ;
#endif /* defined(DEBOUNCE_CLK__CFG3) */

#define DEBOUNCE_CLK_Enable()                       DEBOUNCE_CLK_Start()
#define DEBOUNCE_CLK_Disable()                      DEBOUNCE_CLK_Stop()
#define DEBOUNCE_CLK_SetDivider(clkDivider)         DEBOUNCE_CLK_SetDividerRegister(clkDivider, 1u)
#define DEBOUNCE_CLK_SetDividerValue(clkDivider)    DEBOUNCE_CLK_SetDividerRegister((clkDivider) - 1u, 1u)
#define DEBOUNCE_CLK_SetMode(clkMode)               DEBOUNCE_CLK_SetModeRegister(clkMode)
#define DEBOUNCE_CLK_SetSource(clkSource)           DEBOUNCE_CLK_SetSourceRegister(clkSource)
#if defined(DEBOUNCE_CLK__CFG3)
#define DEBOUNCE_CLK_SetPhase(clkPhase)             DEBOUNCE_CLK_SetPhaseRegister(clkPhase)
#define DEBOUNCE_CLK_SetPhaseValue(clkPhase)        DEBOUNCE_CLK_SetPhaseRegister((clkPhase) + 1u)
#endif /* defined(DEBOUNCE_CLK__CFG3) */


/***************************************
*             Registers
***************************************/

/* Register to enable or disable the clock */
#define DEBOUNCE_CLK_CLKEN              (* (reg8 *) DEBOUNCE_CLK__PM_ACT_CFG)
#define DEBOUNCE_CLK_CLKEN_PTR          ((reg8 *) DEBOUNCE_CLK__PM_ACT_CFG)

/* Register to enable or disable the clock */
#define DEBOUNCE_CLK_CLKSTBY            (* (reg8 *) DEBOUNCE_CLK__PM_STBY_CFG)
#define DEBOUNCE_CLK_CLKSTBY_PTR        ((reg8 *) DEBOUNCE_CLK__PM_STBY_CFG)

/* Clock LSB divider configuration register. */
#define DEBOUNCE_CLK_DIV_LSB            (* (reg8 *) DEBOUNCE_CLK__CFG0)
#define DEBOUNCE_CLK_DIV_LSB_PTR        ((reg8 *) DEBOUNCE_CLK__CFG0)
#define DEBOUNCE_CLK_DIV_PTR            ((reg16 *) DEBOUNCE_CLK__CFG0)

/* Clock MSB divider configuration register. */
#define DEBOUNCE_CLK_DIV_MSB            (* (reg8 *) DEBOUNCE_CLK__CFG1)
#define DEBOUNCE_CLK_DIV_MSB_PTR        ((reg8 *) DEBOUNCE_CLK__CFG1)

/* Mode and source configuration register */
#define DEBOUNCE_CLK_MOD_SRC            (* (reg8 *) DEBOUNCE_CLK__CFG2)
#define DEBOUNCE_CLK_MOD_SRC_PTR        ((reg8 *) DEBOUNCE_CLK__CFG2)

#if defined(DEBOUNCE_CLK__CFG3)
/* Analog clock phase configuration register */
#define DEBOUNCE_CLK_PHASE              (* (reg8 *) DEBOUNCE_CLK__CFG3)
#define DEBOUNCE_CLK_PHASE_PTR          ((reg8 *) DEBOUNCE_CLK__CFG3)
#endif /* defined(DEBOUNCE_CLK__CFG3) */


/**************************************
*       Register Constants
**************************************/

/* Power manager register masks */
#define DEBOUNCE_CLK_CLKEN_MASK         DEBOUNCE_CLK__PM_ACT_MSK
#define DEBOUNCE_CLK_CLKSTBY_MASK       DEBOUNCE_CLK__PM_STBY_MSK

/* CFG2 field masks */
#define DEBOUNCE_CLK_SRC_SEL_MSK        DEBOUNCE_CLK__CFG2_SRC_SEL_MASK
#define DEBOUNCE_CLK_MODE_MASK          (~(DEBOUNCE_CLK_SRC_SEL_MSK))

#if defined(DEBOUNCE_CLK__CFG3)
/* CFG3 phase mask */
#define DEBOUNCE_CLK_PHASE_MASK         DEBOUNCE_CLK__CFG3_PHASE_DLY_MASK
#endif /* defined(DEBOUNCE_CLK__CFG3) */

#endif /* CY_CLOCK_DEBOUNCE_CLK_H */


/* [] END OF FILE */
