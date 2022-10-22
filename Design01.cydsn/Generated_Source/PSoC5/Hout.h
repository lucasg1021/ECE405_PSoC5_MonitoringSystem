/*******************************************************************************
* File Name: Hout.h  
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

#if !defined(CY_PINS_Hout_H) /* Pins Hout_H */
#define CY_PINS_Hout_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Hout_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Hout__PORT == 15 && ((Hout__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Hout_Write(uint8 value);
void    Hout_SetDriveMode(uint8 mode);
uint8   Hout_ReadDataReg(void);
uint8   Hout_Read(void);
void    Hout_SetInterruptMode(uint16 position, uint16 mode);
uint8   Hout_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Hout_SetDriveMode() function.
     *  @{
     */
        #define Hout_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Hout_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Hout_DM_RES_UP          PIN_DM_RES_UP
        #define Hout_DM_RES_DWN         PIN_DM_RES_DWN
        #define Hout_DM_OD_LO           PIN_DM_OD_LO
        #define Hout_DM_OD_HI           PIN_DM_OD_HI
        #define Hout_DM_STRONG          PIN_DM_STRONG
        #define Hout_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Hout_MASK               Hout__MASK
#define Hout_SHIFT              Hout__SHIFT
#define Hout_WIDTH              1u

/* Interrupt constants */
#if defined(Hout__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Hout_SetInterruptMode() function.
     *  @{
     */
        #define Hout_INTR_NONE      (uint16)(0x0000u)
        #define Hout_INTR_RISING    (uint16)(0x0001u)
        #define Hout_INTR_FALLING   (uint16)(0x0002u)
        #define Hout_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Hout_INTR_MASK      (0x01u) 
#endif /* (Hout__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Hout_PS                     (* (reg8 *) Hout__PS)
/* Data Register */
#define Hout_DR                     (* (reg8 *) Hout__DR)
/* Port Number */
#define Hout_PRT_NUM                (* (reg8 *) Hout__PRT) 
/* Connect to Analog Globals */                                                  
#define Hout_AG                     (* (reg8 *) Hout__AG)                       
/* Analog MUX bux enable */
#define Hout_AMUX                   (* (reg8 *) Hout__AMUX) 
/* Bidirectional Enable */                                                        
#define Hout_BIE                    (* (reg8 *) Hout__BIE)
/* Bit-mask for Aliased Register Access */
#define Hout_BIT_MASK               (* (reg8 *) Hout__BIT_MASK)
/* Bypass Enable */
#define Hout_BYP                    (* (reg8 *) Hout__BYP)
/* Port wide control signals */                                                   
#define Hout_CTL                    (* (reg8 *) Hout__CTL)
/* Drive Modes */
#define Hout_DM0                    (* (reg8 *) Hout__DM0) 
#define Hout_DM1                    (* (reg8 *) Hout__DM1)
#define Hout_DM2                    (* (reg8 *) Hout__DM2) 
/* Input Buffer Disable Override */
#define Hout_INP_DIS                (* (reg8 *) Hout__INP_DIS)
/* LCD Common or Segment Drive */
#define Hout_LCD_COM_SEG            (* (reg8 *) Hout__LCD_COM_SEG)
/* Enable Segment LCD */
#define Hout_LCD_EN                 (* (reg8 *) Hout__LCD_EN)
/* Slew Rate Control */
#define Hout_SLW                    (* (reg8 *) Hout__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Hout_PRTDSI__CAPS_SEL       (* (reg8 *) Hout__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Hout_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Hout__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Hout_PRTDSI__OE_SEL0        (* (reg8 *) Hout__PRTDSI__OE_SEL0) 
#define Hout_PRTDSI__OE_SEL1        (* (reg8 *) Hout__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Hout_PRTDSI__OUT_SEL0       (* (reg8 *) Hout__PRTDSI__OUT_SEL0) 
#define Hout_PRTDSI__OUT_SEL1       (* (reg8 *) Hout__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Hout_PRTDSI__SYNC_OUT       (* (reg8 *) Hout__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Hout__SIO_CFG)
    #define Hout_SIO_HYST_EN        (* (reg8 *) Hout__SIO_HYST_EN)
    #define Hout_SIO_REG_HIFREQ     (* (reg8 *) Hout__SIO_REG_HIFREQ)
    #define Hout_SIO_CFG            (* (reg8 *) Hout__SIO_CFG)
    #define Hout_SIO_DIFF           (* (reg8 *) Hout__SIO_DIFF)
#endif /* (Hout__SIO_CFG) */

/* Interrupt Registers */
#if defined(Hout__INTSTAT)
    #define Hout_INTSTAT            (* (reg8 *) Hout__INTSTAT)
    #define Hout_SNAP               (* (reg8 *) Hout__SNAP)
    
	#define Hout_0_INTTYPE_REG 		(* (reg8 *) Hout__0__INTTYPE)
#endif /* (Hout__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Hout_H */


/* [] END OF FILE */
