/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#ifndef _EEPROM_H
#define _EEPROM_H

#include "cyapicallbacks.h"
#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
    
#define EEPROM_ADDR (uint8) 0x50
#define WIFISSID_MAXLEN 30  //memory locations 0x00-0x1D
#define WIFIPWD_MAXLEN 30   //memory locations 0x1E-0x3C
#define SETTEMP_MAXLEN 5    //memory locations 0x3D-0x41
#define SETHUMID_MAXLEN 5   //memory locations 0x42-0x47
    
void writeEEPROM(uint8_t eepromAddr, uint8_t writeEEPROM[], int len);
//uint8_t readEEPROM(uint8_t eepromAddr);
void readEEPROM(uint8_t eepromAddr, char *eepromS, int len);
    
#endif
/* [] END OF FILE */
