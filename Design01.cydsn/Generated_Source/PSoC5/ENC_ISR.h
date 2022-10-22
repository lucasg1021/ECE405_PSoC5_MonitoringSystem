/*******************************************************************************
* File Name: ENC_ISR.h
* Version 1.71
*
*  Description:
*   Provides the function definitions for the Interrupt Controller.
*
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/
#if !defined(CY_ISR_ENC_ISR_H)
#define CY_ISR_ENC_ISR_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void ENC_ISR_Start(void);
void ENC_ISR_StartEx(cyisraddress address);
void ENC_ISR_Stop(void);

CY_ISR_PROTO(ENC_ISR_Interrupt);

void ENC_ISR_SetVector(cyisraddress address);
cyisraddress ENC_ISR_GetVector(void);

void ENC_ISR_SetPriority(uint8 priority);
uint8 ENC_ISR_GetPriority(void);

void ENC_ISR_Enable(void);
uint8 ENC_ISR_GetState(void);
void ENC_ISR_Disable(void);

void ENC_ISR_SetPending(void);
void ENC_ISR_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the ENC_ISR ISR. */
#define ENC_ISR_INTC_VECTOR            ((reg32 *) ENC_ISR__INTC_VECT)

/* Address of the ENC_ISR ISR priority. */
#define ENC_ISR_INTC_PRIOR             ((reg8 *) ENC_ISR__INTC_PRIOR_REG)

/* Priority of the ENC_ISR interrupt. */
#define ENC_ISR_INTC_PRIOR_NUMBER      ENC_ISR__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable ENC_ISR interrupt. */
#define ENC_ISR_INTC_SET_EN            ((reg32 *) ENC_ISR__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the ENC_ISR interrupt. */
#define ENC_ISR_INTC_CLR_EN            ((reg32 *) ENC_ISR__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the ENC_ISR interrupt state to pending. */
#define ENC_ISR_INTC_SET_PD            ((reg32 *) ENC_ISR__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the ENC_ISR interrupt. */
#define ENC_ISR_INTC_CLR_PD            ((reg32 *) ENC_ISR__INTC_CLR_PD_REG)


#endif /* CY_ISR_ENC_ISR_H */


/* [] END OF FILE */
