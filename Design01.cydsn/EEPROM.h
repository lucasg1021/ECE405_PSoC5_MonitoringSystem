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
#define SETTEMP_MAXLEN 1    //memory locations 0x3D
#define SETHUMID_MAXLEN 1   //memory locations 0x3E
#define TOLT_MAXLEN 1       //memory location 0x3F
#define TOLH_MAXLEN 1       //memory location 0x40
    
#define WIFISSID_STARTADDR (uint8) 0x00
#define WIFIPWD_STARTADDR (uint8) 0x1E
#define SETTEMP_STARTADDR (uint8) 0x3D
#define SETHUMID_STARTADDR (uint8) 0x3E
#define TOLT_STARTADDR (uint8) 0x3F
#define TOLH_STARTADDR (uint8) 0x40
    
void writeEEPROM(uint8_t eepromAddr, uint8_t writeEEPROM[], int len);
void readEEPROM(uint8_t eepromAddr, char *eepromS, int len);
void changeSetPointsEEPROM(uint8_t setT, uint8_t setH, uint8_t tolTemp, uint8_t tolHumid);
    
#endif
/* [] END OF FILE */
