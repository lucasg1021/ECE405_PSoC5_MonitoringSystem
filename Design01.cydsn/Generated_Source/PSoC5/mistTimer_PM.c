/*******************************************************************************
* File Name: mistTimer_PM.c
* Version 2.80
*
*  Description:
*     This file provides the power management source code to API for the
*     Timer.
*
*   Note:
*     None
*
*******************************************************************************
* Copyright 2008-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "mistTimer.h"

static mistTimer_backupStruct mistTimer_backup;


/*******************************************************************************
* Function Name: mistTimer_SaveConfig
********************************************************************************
*
* Summary:
*     Save the current user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  mistTimer_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void mistTimer_SaveConfig(void) 
{
    #if (!mistTimer_UsingFixedFunction)
        mistTimer_backup.TimerUdb = mistTimer_ReadCounter();
        mistTimer_backup.InterruptMaskValue = mistTimer_STATUS_MASK;
        #if (mistTimer_UsingHWCaptureCounter)
            mistTimer_backup.TimerCaptureCounter = mistTimer_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!mistTimer_UDB_CONTROL_REG_REMOVED)
            mistTimer_backup.TimerControlRegister = mistTimer_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: mistTimer_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  mistTimer_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void mistTimer_RestoreConfig(void) 
{   
    #if (!mistTimer_UsingFixedFunction)

        mistTimer_WriteCounter(mistTimer_backup.TimerUdb);
        mistTimer_STATUS_MASK =mistTimer_backup.InterruptMaskValue;
        #if (mistTimer_UsingHWCaptureCounter)
            mistTimer_SetCaptureCount(mistTimer_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!mistTimer_UDB_CONTROL_REG_REMOVED)
            mistTimer_WriteControlRegister(mistTimer_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: mistTimer_Sleep
********************************************************************************
*
* Summary:
*     Stop and Save the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  mistTimer_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void mistTimer_Sleep(void) 
{
    #if(!mistTimer_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(mistTimer_CTRL_ENABLE == (mistTimer_CONTROL & mistTimer_CTRL_ENABLE))
        {
            /* Timer is enabled */
            mistTimer_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            mistTimer_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    mistTimer_Stop();
    mistTimer_SaveConfig();
}


/*******************************************************************************
* Function Name: mistTimer_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  mistTimer_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void mistTimer_Wakeup(void) 
{
    mistTimer_RestoreConfig();
    #if(!mistTimer_UDB_CONTROL_REG_REMOVED)
        if(mistTimer_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                mistTimer_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
