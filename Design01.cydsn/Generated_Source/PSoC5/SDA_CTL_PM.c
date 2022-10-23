/*******************************************************************************
* File Name: SDA_CTL_PM.c
* Version 1.80
*
* Description:
*  This file contains the setup, control, and status commands to support 
*  the component operation in the low power mode. 
*
* Note:
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "SDA_CTL.h"

/* Check for removal by optimization */
#if !defined(SDA_CTL_Sync_ctrl_reg__REMOVED)

static SDA_CTL_BACKUP_STRUCT  SDA_CTL_backup = {0u};

    
/*******************************************************************************
* Function Name: SDA_CTL_SaveConfig
********************************************************************************
*
* Summary:
*  Saves the control register value.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void SDA_CTL_SaveConfig(void) 
{
    SDA_CTL_backup.controlState = SDA_CTL_Control;
}


/*******************************************************************************
* Function Name: SDA_CTL_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the control register value.
*
* Parameters:
*  None
*
* Return:
*  None
*
*
*******************************************************************************/
void SDA_CTL_RestoreConfig(void) 
{
     SDA_CTL_Control = SDA_CTL_backup.controlState;
}


/*******************************************************************************
* Function Name: SDA_CTL_Sleep
********************************************************************************
*
* Summary:
*  Prepares the component for entering the low power mode.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void SDA_CTL_Sleep(void) 
{
    SDA_CTL_SaveConfig();
}


/*******************************************************************************
* Function Name: SDA_CTL_Wakeup
********************************************************************************
*
* Summary:
*  Restores the component after waking up from the low power mode.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void SDA_CTL_Wakeup(void)  
{
    SDA_CTL_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
