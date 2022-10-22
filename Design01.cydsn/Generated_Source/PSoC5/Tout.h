/*******************************************************************************
* File Name: Tout.h  
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

#if !defined(CY_PINS_Tout_H) /* Pins Tout_H */
#define CY_PINS_Tout_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Tout_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Tout__PORT == 15 && ((Tout__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Tout_Write(uint8 value);
void    Tout_SetDriveMode(uint8 mode);
uint8   Tout_ReadDataReg(void);
uint8   Tout_Read(void);
void    Tout_SetInterruptMode(uint16 position, uint16 mode);
uint8   Tout_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Tout_SetDriveMode() function.
     *  @{
     */
        #define Tout_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Tout_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Tout_DM_RES_UP          PIN_DM_RES_UP
        #define Tout_DM_RES_DWN         PIN_DM_RES_DWN
        #define Tout_DM_OD_LO           PIN_DM_OD_LO
        #define Tout_DM_OD_HI           PIN_DM_OD_HI
        #define Tout_DM_STRONG          PIN_DM_STRONG
        #define Tout_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Tout_MASK               Tout__MASK
#define Tout_SHIFT              Tout__SHIFT
#define Tout_WIDTH              1u

/* Interrupt constants */
#if defined(Tout__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Tout_SetInterruptMode() function.
     *  @{
     */
        #define Tout_INTR_NONE      (uint16)(0x0000u)
        #define Tout_INTR_RISING    (uint16)(0x0001u)
        #define Tout_INTR_FALLING   (uint16)(0x0002u)
        #define Tout_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Tout_INTR_MASK      (0x01u) 
#endif /* (Tout__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Tout_PS                     (* (reg8 *) Tout__PS)
/* Data Register */
#define Tout_DR                     (* (reg8 *) Tout__DR)
/* Port Number */
#define Tout_PRT_NUM                (* (reg8 *) Tout__PRT) 
/* Connect to Analog Globals */                                                  
#define Tout_AG                     (* (reg8 *) Tout__AG)                       
/* Analog MUX bux enable */
#define Tout_AMUX                   (* (reg8 *) Tout__AMUX) 
/* Bidirectional Enable */                                                        
#define Tout_BIE                    (* (reg8 *) Tout__BIE)
/* Bit-mask for Aliased Register Access */
#define Tout_BIT_MASK               (* (reg8 *) Tout__BIT_MASK)
/* Bypass Enable */
#define Tout_BYP                    (* (reg8 *) Tout__BYP)
/* Port wide control signals */                                                   
#define Tout_CTL                    (* (reg8 *) Tout__CTL)
/* Drive Modes */
#define Tout_DM0                    (* (reg8 *) Tout__DM0) 
#define Tout_DM1                    (* (reg8 *) Tout__DM1)
#define Tout_DM2                    (* (reg8 *) Tout__DM2) 
/* Input Buffer Disable Override */
#define Tout_INP_DIS                (* (reg8 *) Tout__INP_DIS)
/* LCD Common or Segment Drive */
#define Tout_LCD_COM_SEG            (* (reg8 *) Tout__LCD_COM_SEG)
/* Enable Segment LCD */
#define Tout_LCD_EN                 (* (reg8 *) Tout__LCD_EN)
/* Slew Rate Control */
#define Tout_SLW                    (* (reg8 *) Tout__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Tout_PRTDSI__CAPS_SEL       (* (reg8 *) Tout__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Tout_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Tout__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Tout_PRTDSI__OE_SEL0        (* (reg8 *) Tout__PRTDSI__OE_SEL0) 
#define Tout_PRTDSI__OE_SEL1        (* (reg8 *) Tout__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Tout_PRTDSI__OUT_SEL0       (* (reg8 *) Tout__PRTDSI__OUT_SEL0) 
#define Tout_PRTDSI__OUT_SEL1       (* (reg8 *) Tout__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Tout_PRTDSI__SYNC_OUT       (* (reg8 *) Tout__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Tout__SIO_CFG)
    #define Tout_SIO_HYST_EN        (* (reg8 *) Tout__SIO_HYST_EN)
    #define Tout_SIO_REG_HIFREQ     (* (reg8 *) Tout__SIO_REG_HIFREQ)
    #define Tout_SIO_CFG            (* (reg8 *) Tout__SIO_CFG)
    #define Tout_SIO_DIFF           (* (reg8 *) Tout__SIO_DIFF)
#endif /* (Tout__SIO_CFG) */

/* Interrupt Registers */
#if defined(Tout__INTSTAT)
    #define Tout_INTSTAT            (* (reg8 *) Tout__INTSTAT)
    #define Tout_SNAP               (* (reg8 *) Tout__SNAP)
    
	#define Tout_0_INTTYPE_REG 		(* (reg8 *) Tout__0__INTTYPE)
#endif /* (Tout__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Tout_H */


/* [] END OF FILE */
