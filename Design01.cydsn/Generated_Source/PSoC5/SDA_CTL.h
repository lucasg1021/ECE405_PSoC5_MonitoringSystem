/*******************************************************************************
* File Name: SDA_CTL.h  
* Version 1.80
*
* Description:
*  This file containts Control Register function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CONTROL_REG_SDA_CTL_H) /* CY_CONTROL_REG_SDA_CTL_H */
#define CY_CONTROL_REG_SDA_CTL_H

#include "cyfitter.h"

#if ((CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC3) || \
     (CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC4) || \
     (CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC5))
    #include "cytypes.h"
#else
    #include "syslib/cy_syslib.h"
#endif

    
/***************************************
*     Data Struct Definitions
***************************************/

/* Sleep Mode API Support */
typedef struct
{
    uint8 controlState;

} SDA_CTL_BACKUP_STRUCT;


/***************************************
*         Function Prototypes 
***************************************/

void    SDA_CTL_Write(uint8 control) ;
uint8   SDA_CTL_Read(void) ;

void SDA_CTL_SaveConfig(void) ;
void SDA_CTL_RestoreConfig(void) ;
void SDA_CTL_Sleep(void) ; 
void SDA_CTL_Wakeup(void) ;


/***************************************
*            Registers        
***************************************/

/* Control Register */
#define SDA_CTL_Control        (* (reg8 *) SDA_CTL_Sync_ctrl_reg__CONTROL_REG )
#define SDA_CTL_Control_PTR    (  (reg8 *) SDA_CTL_Sync_ctrl_reg__CONTROL_REG )

#endif /* End CY_CONTROL_REG_SDA_CTL_H */


/* [] END OF FILE */
