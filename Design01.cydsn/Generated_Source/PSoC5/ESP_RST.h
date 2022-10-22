/*******************************************************************************
* File Name: ESP_RST.h  
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

#if !defined(CY_PINS_ESP_RST_H) /* Pins ESP_RST_H */
#define CY_PINS_ESP_RST_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "ESP_RST_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 ESP_RST__PORT == 15 && ((ESP_RST__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    ESP_RST_Write(uint8 value);
void    ESP_RST_SetDriveMode(uint8 mode);
uint8   ESP_RST_ReadDataReg(void);
uint8   ESP_RST_Read(void);
void    ESP_RST_SetInterruptMode(uint16 position, uint16 mode);
uint8   ESP_RST_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the ESP_RST_SetDriveMode() function.
     *  @{
     */
        #define ESP_RST_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define ESP_RST_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define ESP_RST_DM_RES_UP          PIN_DM_RES_UP
        #define ESP_RST_DM_RES_DWN         PIN_DM_RES_DWN
        #define ESP_RST_DM_OD_LO           PIN_DM_OD_LO
        #define ESP_RST_DM_OD_HI           PIN_DM_OD_HI
        #define ESP_RST_DM_STRONG          PIN_DM_STRONG
        #define ESP_RST_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define ESP_RST_MASK               ESP_RST__MASK
#define ESP_RST_SHIFT              ESP_RST__SHIFT
#define ESP_RST_WIDTH              1u

/* Interrupt constants */
#if defined(ESP_RST__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in ESP_RST_SetInterruptMode() function.
     *  @{
     */
        #define ESP_RST_INTR_NONE      (uint16)(0x0000u)
        #define ESP_RST_INTR_RISING    (uint16)(0x0001u)
        #define ESP_RST_INTR_FALLING   (uint16)(0x0002u)
        #define ESP_RST_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define ESP_RST_INTR_MASK      (0x01u) 
#endif /* (ESP_RST__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define ESP_RST_PS                     (* (reg8 *) ESP_RST__PS)
/* Data Register */
#define ESP_RST_DR                     (* (reg8 *) ESP_RST__DR)
/* Port Number */
#define ESP_RST_PRT_NUM                (* (reg8 *) ESP_RST__PRT) 
/* Connect to Analog Globals */                                                  
#define ESP_RST_AG                     (* (reg8 *) ESP_RST__AG)                       
/* Analog MUX bux enable */
#define ESP_RST_AMUX                   (* (reg8 *) ESP_RST__AMUX) 
/* Bidirectional Enable */                                                        
#define ESP_RST_BIE                    (* (reg8 *) ESP_RST__BIE)
/* Bit-mask for Aliased Register Access */
#define ESP_RST_BIT_MASK               (* (reg8 *) ESP_RST__BIT_MASK)
/* Bypass Enable */
#define ESP_RST_BYP                    (* (reg8 *) ESP_RST__BYP)
/* Port wide control signals */                                                   
#define ESP_RST_CTL                    (* (reg8 *) ESP_RST__CTL)
/* Drive Modes */
#define ESP_RST_DM0                    (* (reg8 *) ESP_RST__DM0) 
#define ESP_RST_DM1                    (* (reg8 *) ESP_RST__DM1)
#define ESP_RST_DM2                    (* (reg8 *) ESP_RST__DM2) 
/* Input Buffer Disable Override */
#define ESP_RST_INP_DIS                (* (reg8 *) ESP_RST__INP_DIS)
/* LCD Common or Segment Drive */
#define ESP_RST_LCD_COM_SEG            (* (reg8 *) ESP_RST__LCD_COM_SEG)
/* Enable Segment LCD */
#define ESP_RST_LCD_EN                 (* (reg8 *) ESP_RST__LCD_EN)
/* Slew Rate Control */
#define ESP_RST_SLW                    (* (reg8 *) ESP_RST__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define ESP_RST_PRTDSI__CAPS_SEL       (* (reg8 *) ESP_RST__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define ESP_RST_PRTDSI__DBL_SYNC_IN    (* (reg8 *) ESP_RST__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define ESP_RST_PRTDSI__OE_SEL0        (* (reg8 *) ESP_RST__PRTDSI__OE_SEL0) 
#define ESP_RST_PRTDSI__OE_SEL1        (* (reg8 *) ESP_RST__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define ESP_RST_PRTDSI__OUT_SEL0       (* (reg8 *) ESP_RST__PRTDSI__OUT_SEL0) 
#define ESP_RST_PRTDSI__OUT_SEL1       (* (reg8 *) ESP_RST__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define ESP_RST_PRTDSI__SYNC_OUT       (* (reg8 *) ESP_RST__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(ESP_RST__SIO_CFG)
    #define ESP_RST_SIO_HYST_EN        (* (reg8 *) ESP_RST__SIO_HYST_EN)
    #define ESP_RST_SIO_REG_HIFREQ     (* (reg8 *) ESP_RST__SIO_REG_HIFREQ)
    #define ESP_RST_SIO_CFG            (* (reg8 *) ESP_RST__SIO_CFG)
    #define ESP_RST_SIO_DIFF           (* (reg8 *) ESP_RST__SIO_DIFF)
#endif /* (ESP_RST__SIO_CFG) */

/* Interrupt Registers */
#if defined(ESP_RST__INTSTAT)
    #define ESP_RST_INTSTAT            (* (reg8 *) ESP_RST__INTSTAT)
    #define ESP_RST_SNAP               (* (reg8 *) ESP_RST__SNAP)
    
	#define ESP_RST_0_INTTYPE_REG 		(* (reg8 *) ESP_RST__0__INTTYPE)
#endif /* (ESP_RST__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_ESP_RST_H */


/* [] END OF FILE */
