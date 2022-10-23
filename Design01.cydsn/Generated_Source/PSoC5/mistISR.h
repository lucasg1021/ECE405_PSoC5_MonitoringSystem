/*******************************************************************************
* File Name: mistISR.h
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
#if !defined(CY_ISR_mistISR_H)
#define CY_ISR_mistISR_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void mistISR_Start(void);
void mistISR_StartEx(cyisraddress address);
void mistISR_Stop(void);

CY_ISR_PROTO(mistISR_Interrupt);

void mistISR_SetVector(cyisraddress address);
cyisraddress mistISR_GetVector(void);

void mistISR_SetPriority(uint8 priority);
uint8 mistISR_GetPriority(void);

void mistISR_Enable(void);
uint8 mistISR_GetState(void);
void mistISR_Disable(void);

void mistISR_SetPending(void);
void mistISR_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the mistISR ISR. */
#define mistISR_INTC_VECTOR            ((reg32 *) mistISR__INTC_VECT)

/* Address of the mistISR ISR priority. */
#define mistISR_INTC_PRIOR             ((reg8 *) mistISR__INTC_PRIOR_REG)

/* Priority of the mistISR interrupt. */
#define mistISR_INTC_PRIOR_NUMBER      mistISR__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable mistISR interrupt. */
#define mistISR_INTC_SET_EN            ((reg32 *) mistISR__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the mistISR interrupt. */
#define mistISR_INTC_CLR_EN            ((reg32 *) mistISR__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the mistISR interrupt state to pending. */
#define mistISR_INTC_SET_PD            ((reg32 *) mistISR__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the mistISR interrupt. */
#define mistISR_INTC_CLR_PD            ((reg32 *) mistISR__INTC_CLR_PD_REG)


#endif /* CY_ISR_mistISR_H */


/* [] END OF FILE */
