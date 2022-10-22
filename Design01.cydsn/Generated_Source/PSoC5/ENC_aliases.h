/*******************************************************************************
* File Name: ENC.h  
* Version 2.20
*
* Description:
*  This file contains the Alias definitions for Per-Pin APIs in cypins.h. 
*  Information on using these APIs can be found in the System Reference Guide.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_ENC_ALIASES_H) /* Pins ENC_ALIASES_H */
#define CY_PINS_ENC_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define ENC_0			(ENC__0__PC)
#define ENC_0_INTR	((uint16)((uint16)0x0001u << ENC__0__SHIFT))

#define ENC_1			(ENC__1__PC)
#define ENC_1_INTR	((uint16)((uint16)0x0001u << ENC__1__SHIFT))

#define ENC_INTR_ALL	 ((uint16)(ENC_0_INTR| ENC_1_INTR))
#define ENC_A			(ENC__A__PC)
#define ENC_A_INTR	((uint16)((uint16)0x0001u << ENC__0__SHIFT))

#define ENC_B			(ENC__B__PC)
#define ENC_B_INTR	((uint16)((uint16)0x0001u << ENC__1__SHIFT))

#endif /* End Pins ENC_ALIASES_H */


/* [] END OF FILE */
