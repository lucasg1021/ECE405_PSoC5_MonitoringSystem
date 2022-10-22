/*******************************************************************************
* File Name: ESPUART_PM.c
* Version 2.50
*
* Description:
*  This file provides Sleep/WakeUp APIs functionality.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "ESPUART.h"


/***************************************
* Local data allocation
***************************************/

static ESPUART_BACKUP_STRUCT  ESPUART_backup =
{
    /* enableState - disabled */
    0u,
};



/*******************************************************************************
* Function Name: ESPUART_SaveConfig
********************************************************************************
*
* Summary:
*  This function saves the component nonretention control register.
*  Does not save the FIFO which is a set of nonretention registers.
*  This function is called by the ESPUART_Sleep() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  ESPUART_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void ESPUART_SaveConfig(void)
{
    #if(ESPUART_CONTROL_REG_REMOVED == 0u)
        ESPUART_backup.cr = ESPUART_CONTROL_REG;
    #endif /* End ESPUART_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: ESPUART_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the nonretention control register except FIFO.
*  Does not restore the FIFO which is a set of nonretention registers.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  ESPUART_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
* Notes:
*  If this function is called without calling ESPUART_SaveConfig() 
*  first, the data loaded may be incorrect.
*
*******************************************************************************/
void ESPUART_RestoreConfig(void)
{
    #if(ESPUART_CONTROL_REG_REMOVED == 0u)
        ESPUART_CONTROL_REG = ESPUART_backup.cr;
    #endif /* End ESPUART_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: ESPUART_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred API to prepare the component for sleep. 
*  The ESPUART_Sleep() API saves the current component state. Then it
*  calls the ESPUART_Stop() function and calls 
*  ESPUART_SaveConfig() to save the hardware configuration.
*  Call the ESPUART_Sleep() function before calling the CyPmSleep() 
*  or the CyPmHibernate() function. 
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  ESPUART_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void ESPUART_Sleep(void)
{
    #if(ESPUART_RX_ENABLED || ESPUART_HD_ENABLED)
        if((ESPUART_RXSTATUS_ACTL_REG  & ESPUART_INT_ENABLE) != 0u)
        {
            ESPUART_backup.enableState = 1u;
        }
        else
        {
            ESPUART_backup.enableState = 0u;
        }
    #else
        if((ESPUART_TXSTATUS_ACTL_REG  & ESPUART_INT_ENABLE) !=0u)
        {
            ESPUART_backup.enableState = 1u;
        }
        else
        {
            ESPUART_backup.enableState = 0u;
        }
    #endif /* End ESPUART_RX_ENABLED || ESPUART_HD_ENABLED*/

    ESPUART_Stop();
    ESPUART_SaveConfig();
}


/*******************************************************************************
* Function Name: ESPUART_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred API to restore the component to the state when 
*  ESPUART_Sleep() was called. The ESPUART_Wakeup() function
*  calls the ESPUART_RestoreConfig() function to restore the 
*  configuration. If the component was enabled before the 
*  ESPUART_Sleep() function was called, the ESPUART_Wakeup()
*  function will also re-enable the component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  ESPUART_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void ESPUART_Wakeup(void)
{
    ESPUART_RestoreConfig();
    #if( (ESPUART_RX_ENABLED) || (ESPUART_HD_ENABLED) )
        ESPUART_ClearRxBuffer();
    #endif /* End (ESPUART_RX_ENABLED) || (ESPUART_HD_ENABLED) */
    #if(ESPUART_TX_ENABLED || ESPUART_HD_ENABLED)
        ESPUART_ClearTxBuffer();
    #endif /* End ESPUART_TX_ENABLED || ESPUART_HD_ENABLED */

    if(ESPUART_backup.enableState != 0u)
    {
        ESPUART_Enable();
    }
}


/* [] END OF FILE */
