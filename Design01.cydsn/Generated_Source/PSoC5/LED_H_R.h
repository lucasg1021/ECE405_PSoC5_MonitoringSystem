/*******************************************************************************
* File Name: LED_H_R.h  
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

#if !defined(CY_PINS_LED_H_R_H) /* Pins LED_H_R_H */
#define CY_PINS_LED_H_R_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "LED_H_R_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 LED_H_R__PORT == 15 && ((LED_H_R__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    LED_H_R_Write(uint8 value);
void    LED_H_R_SetDriveMode(uint8 mode);
uint8   LED_H_R_ReadDataReg(void);
uint8   LED_H_R_Read(void);
void    LED_H_R_SetInterruptMode(uint16 position, uint16 mode);
uint8   LED_H_R_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the LED_H_R_SetDriveMode() function.
     *  @{
     */
        #define LED_H_R_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define LED_H_R_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define LED_H_R_DM_RES_UP          PIN_DM_RES_UP
        #define LED_H_R_DM_RES_DWN         PIN_DM_RES_DWN
        #define LED_H_R_DM_OD_LO           PIN_DM_OD_LO
        #define LED_H_R_DM_OD_HI           PIN_DM_OD_HI
        #define LED_H_R_DM_STRONG          PIN_DM_STRONG
        #define LED_H_R_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define LED_H_R_MASK               LED_H_R__MASK
#define LED_H_R_SHIFT              LED_H_R__SHIFT
#define LED_H_R_WIDTH              1u

/* Interrupt constants */
#if defined(LED_H_R__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in LED_H_R_SetInterruptMode() function.
     *  @{
     */
        #define LED_H_R_INTR_NONE      (uint16)(0x0000u)
        #define LED_H_R_INTR_RISING    (uint16)(0x0001u)
        #define LED_H_R_INTR_FALLING   (uint16)(0x0002u)
        #define LED_H_R_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define LED_H_R_INTR_MASK      (0x01u) 
#endif /* (LED_H_R__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define LED_H_R_PS                     (* (reg8 *) LED_H_R__PS)
/* Data Register */
#define LED_H_R_DR                     (* (reg8 *) LED_H_R__DR)
/* Port Number */
#define LED_H_R_PRT_NUM                (* (reg8 *) LED_H_R__PRT) 
/* Connect to Analog Globals */                                                  
#define LED_H_R_AG                     (* (reg8 *) LED_H_R__AG)                       
/* Analog MUX bux enable */
#define LED_H_R_AMUX                   (* (reg8 *) LED_H_R__AMUX) 
/* Bidirectional Enable */                                                        
#define LED_H_R_BIE                    (* (reg8 *) LED_H_R__BIE)
/* Bit-mask for Aliased Register Access */
#define LED_H_R_BIT_MASK               (* (reg8 *) LED_H_R__BIT_MASK)
/* Bypass Enable */
#define LED_H_R_BYP                    (* (reg8 *) LED_H_R__BYP)
/* Port wide control signals */                                                   
#define LED_H_R_CTL                    (* (reg8 *) LED_H_R__CTL)
/* Drive Modes */
#define LED_H_R_DM0                    (* (reg8 *) LED_H_R__DM0) 
#define LED_H_R_DM1                    (* (reg8 *) LED_H_R__DM1)
#define LED_H_R_DM2                    (* (reg8 *) LED_H_R__DM2) 
/* Input Buffer Disable Override */
#define LED_H_R_INP_DIS                (* (reg8 *) LED_H_R__INP_DIS)
/* LCD Common or Segment Drive */
#define LED_H_R_LCD_COM_SEG            (* (reg8 *) LED_H_R__LCD_COM_SEG)
/* Enable Segment LCD */
#define LED_H_R_LCD_EN                 (* (reg8 *) LED_H_R__LCD_EN)
/* Slew Rate Control */
#define LED_H_R_SLW                    (* (reg8 *) LED_H_R__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define LED_H_R_PRTDSI__CAPS_SEL       (* (reg8 *) LED_H_R__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define LED_H_R_PRTDSI__DBL_SYNC_IN    (* (reg8 *) LED_H_R__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define LED_H_R_PRTDSI__OE_SEL0        (* (reg8 *) LED_H_R__PRTDSI__OE_SEL0) 
#define LED_H_R_PRTDSI__OE_SEL1        (* (reg8 *) LED_H_R__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define LED_H_R_PRTDSI__OUT_SEL0       (* (reg8 *) LED_H_R__PRTDSI__OUT_SEL0) 
#define LED_H_R_PRTDSI__OUT_SEL1       (* (reg8 *) LED_H_R__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define LED_H_R_PRTDSI__SYNC_OUT       (* (reg8 *) LED_H_R__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(LED_H_R__SIO_CFG)
    #define LED_H_R_SIO_HYST_EN        (* (reg8 *) LED_H_R__SIO_HYST_EN)
    #define LED_H_R_SIO_REG_HIFREQ     (* (reg8 *) LED_H_R__SIO_REG_HIFREQ)
    #define LED_H_R_SIO_CFG            (* (reg8 *) LED_H_R__SIO_CFG)
    #define LED_H_R_SIO_DIFF           (* (reg8 *) LED_H_R__SIO_DIFF)
#endif /* (LED_H_R__SIO_CFG) */

/* Interrupt Registers */
#if defined(LED_H_R__INTSTAT)
    #define LED_H_R_INTSTAT            (* (reg8 *) LED_H_R__INTSTAT)
    #define LED_H_R_SNAP               (* (reg8 *) LED_H_R__SNAP)
    
	#define LED_H_R_0_INTTYPE_REG 		(* (reg8 *) LED_H_R__0__INTTYPE)
#endif /* (LED_H_R__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_LED_H_R_H */


/* [] END OF FILE */
