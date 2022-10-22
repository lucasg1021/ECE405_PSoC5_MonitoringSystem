/*******************************************************************************
* File Name: LED_T_Y.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_LED_T_Y_H) /* Pins LED_T_Y_H */
#define CY_PINS_LED_T_Y_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "LED_T_Y_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 LED_T_Y__PORT == 15 && ((LED_T_Y__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    LED_T_Y_Write(uint8 value);
void    LED_T_Y_SetDriveMode(uint8 mode);
uint8   LED_T_Y_ReadDataReg(void);
uint8   LED_T_Y_Read(void);
void    LED_T_Y_SetInterruptMode(uint16 position, uint16 mode);
uint8   LED_T_Y_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the LED_T_Y_SetDriveMode() function.
     *  @{
     */
        #define LED_T_Y_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define LED_T_Y_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define LED_T_Y_DM_RES_UP          PIN_DM_RES_UP
        #define LED_T_Y_DM_RES_DWN         PIN_DM_RES_DWN
        #define LED_T_Y_DM_OD_LO           PIN_DM_OD_LO
        #define LED_T_Y_DM_OD_HI           PIN_DM_OD_HI
        #define LED_T_Y_DM_STRONG          PIN_DM_STRONG
        #define LED_T_Y_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define LED_T_Y_MASK               LED_T_Y__MASK
#define LED_T_Y_SHIFT              LED_T_Y__SHIFT
#define LED_T_Y_WIDTH              1u

/* Interrupt constants */
#if defined(LED_T_Y__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in LED_T_Y_SetInterruptMode() function.
     *  @{
     */
        #define LED_T_Y_INTR_NONE      (uint16)(0x0000u)
        #define LED_T_Y_INTR_RISING    (uint16)(0x0001u)
        #define LED_T_Y_INTR_FALLING   (uint16)(0x0002u)
        #define LED_T_Y_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define LED_T_Y_INTR_MASK      (0x01u) 
#endif /* (LED_T_Y__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define LED_T_Y_PS                     (* (reg8 *) LED_T_Y__PS)
/* Data Register */
#define LED_T_Y_DR                     (* (reg8 *) LED_T_Y__DR)
/* Port Number */
#define LED_T_Y_PRT_NUM                (* (reg8 *) LED_T_Y__PRT) 
/* Connect to Analog Globals */                                                  
#define LED_T_Y_AG                     (* (reg8 *) LED_T_Y__AG)                       
/* Analog MUX bux enable */
#define LED_T_Y_AMUX                   (* (reg8 *) LED_T_Y__AMUX) 
/* Bidirectional Enable */                                                        
#define LED_T_Y_BIE                    (* (reg8 *) LED_T_Y__BIE)
/* Bit-mask for Aliased Register Access */
#define LED_T_Y_BIT_MASK               (* (reg8 *) LED_T_Y__BIT_MASK)
/* Bypass Enable */
#define LED_T_Y_BYP                    (* (reg8 *) LED_T_Y__BYP)
/* Port wide control signals */                                                   
#define LED_T_Y_CTL                    (* (reg8 *) LED_T_Y__CTL)
/* Drive Modes */
#define LED_T_Y_DM0                    (* (reg8 *) LED_T_Y__DM0) 
#define LED_T_Y_DM1                    (* (reg8 *) LED_T_Y__DM1)
#define LED_T_Y_DM2                    (* (reg8 *) LED_T_Y__DM2) 
/* Input Buffer Disable Override */
#define LED_T_Y_INP_DIS                (* (reg8 *) LED_T_Y__INP_DIS)
/* LCD Common or Segment Drive */
#define LED_T_Y_LCD_COM_SEG            (* (reg8 *) LED_T_Y__LCD_COM_SEG)
/* Enable Segment LCD */
#define LED_T_Y_LCD_EN                 (* (reg8 *) LED_T_Y__LCD_EN)
/* Slew Rate Control */
#define LED_T_Y_SLW                    (* (reg8 *) LED_T_Y__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define LED_T_Y_PRTDSI__CAPS_SEL       (* (reg8 *) LED_T_Y__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define LED_T_Y_PRTDSI__DBL_SYNC_IN    (* (reg8 *) LED_T_Y__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define LED_T_Y_PRTDSI__OE_SEL0        (* (reg8 *) LED_T_Y__PRTDSI__OE_SEL0) 
#define LED_T_Y_PRTDSI__OE_SEL1        (* (reg8 *) LED_T_Y__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define LED_T_Y_PRTDSI__OUT_SEL0       (* (reg8 *) LED_T_Y__PRTDSI__OUT_SEL0) 
#define LED_T_Y_PRTDSI__OUT_SEL1       (* (reg8 *) LED_T_Y__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define LED_T_Y_PRTDSI__SYNC_OUT       (* (reg8 *) LED_T_Y__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(LED_T_Y__SIO_CFG)
    #define LED_T_Y_SIO_HYST_EN        (* (reg8 *) LED_T_Y__SIO_HYST_EN)
    #define LED_T_Y_SIO_REG_HIFREQ     (* (reg8 *) LED_T_Y__SIO_REG_HIFREQ)
    #define LED_T_Y_SIO_CFG            (* (reg8 *) LED_T_Y__SIO_CFG)
    #define LED_T_Y_SIO_DIFF           (* (reg8 *) LED_T_Y__SIO_DIFF)
#endif /* (LED_T_Y__SIO_CFG) */

/* Interrupt Registers */
#if defined(LED_T_Y__INTSTAT)
    #define LED_T_Y_INTSTAT            (* (reg8 *) LED_T_Y__INTSTAT)
    #define LED_T_Y_SNAP               (* (reg8 *) LED_T_Y__SNAP)
    
	#define LED_T_Y_0_INTTYPE_REG 		(* (reg8 *) LED_T_Y__0__INTTYPE)
#endif /* (LED_T_Y__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_LED_T_Y_H */


/* [] END OF FILE */
