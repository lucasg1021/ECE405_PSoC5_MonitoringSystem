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

#include "EEPROM_functions.h"
#include "EEPROM_1.h"
#include "I2C.h"

// write number of bytes to built in eeprom
void writeEEPROM(uint8_t eepromAddr, uint8_t writeS[], int len){
    uint8_t i2cWrBuf[2];
    
    for(int i = 0; i < len; i++){
        while(EEPROM_1_WriteByte(writeS[i], eepromAddr));
        CyDelay(10);
        
        eepromAddr++;
    }
} 

// read number of bytes from eeprom to eepromS string
void readEEPROM(uint8_t eepromAddr, char *eepromS, int len){
    uint8_t i2cWrBuf[1];
    uint8_t i2cRdBuf[1];
    
    for(int i = 0; i < len; i++){        
        eepromS[i] = EEPROM_1_ReadByte(eepromAddr);
        eepromAddr++;
    }
}

// write new set points to eeprom
void changeSetPointsEEPROM(uint8_t setT, uint8_t setH, uint8_t tolTemp, uint8_t tolHumid){
    uint8_t wr[1];
    
    wr[0] = setT;
    writeEEPROM(SETTEMP_STARTADDR, wr, 1);
    
    wr[0] = setH;
    writeEEPROM(SETHUMID_STARTADDR, wr, 1);
    
    wr[0] = tolTemp;
    writeEEPROM(TOLT_STARTADDR, wr, 1);
    
    wr[0] = tolHumid;
    writeEEPROM(TOLH_STARTADDR, wr, 1);
}
/* [] END OF FILE */
