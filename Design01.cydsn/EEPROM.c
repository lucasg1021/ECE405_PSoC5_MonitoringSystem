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

#include "EEPROM.h"
#include "I2C.h"

void writeEEPROM(uint8_t eepromAddr, uint8_t writeS[], int len){
    uint8_t i2cWrBuf[2];
    
    for(int i = 0; i < len; i++){
        i2cWrBuf[0] = eepromAddr;
        i2cWrBuf[1] = writeS[i];
        I2C_MasterWriteBuf(EEPROM_ADDR, (uint8 *)i2cWrBuf, 2, I2C_MODE_COMPLETE_XFER);
        while(!(I2C_MasterStatus() & I2C_MSTAT_WR_CMPLT));
        I2C_MasterClearStatus(); 
        CyDelay(10);
        
        eepromAddr++;
    }
}

void readEEPROM(uint8_t eepromAddr, char *eepromS, int len){
    uint8_t i2cWrBuf[1];
    uint8_t i2cRdBuf[1];
    
    for(int i = 0; i < len; i++){
        i2cWrBuf[0] = eepromAddr;
        I2C_MasterWriteBuf(EEPROM_ADDR, (uint8 *)i2cWrBuf, 1, I2C_MODE_COMPLETE_XFER);
        while(!(I2C_MasterStatus() & I2C_MSTAT_WR_CMPLT));
        I2C_MasterClearStatus(); 
        CyDelay(10);
        
        I2C_MasterReadBuf(EEPROM_ADDR, i2cRdBuf, 1, I2C_MODE_COMPLETE_XFER);
        while (!I2C_MSTAT_RD_CMPLT);
        I2C_MasterClearStatus();
        CyDelay(10);
        
        eepromS[i] = (char)i2cRdBuf[0];
        eepromAddr++;
    }
}

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
