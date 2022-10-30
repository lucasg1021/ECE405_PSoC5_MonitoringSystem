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
#include "project.h"
#include "aht.h"
#include "I2C.h"
#include "ssd1306.h"
#include "Tout.h"
#include "Hout.h"
#include "LED_T_G.h"
#include "LED_T_Y.h"
#include "LED_T_R.h"
#include "LED_H_G.h"
#include "LED_H_Y.h"
#include "LED_H_R.h"
#include "esp.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
    

void initializeAHT(){
    uint8 i2cWrBuf[4];
    uint8 i2cRdBuf[1];
    
    i2cWrBuf[0] = 0b01110001; // slave addr ; b[0] = 1 for read mode
    I2C_MasterWriteBuf(AHT_ADDR, (uint8 *)i2cWrBuf, 1, I2C_MODE_COMPLETE_XFER);
    while(!(I2C_MasterStatus() & I2C_MSTAT_WR_CMPLT));
    I2C_MasterClearStatus();
    CyDelay(10);
        
    I2C_MasterReadBuf(AHT_ADDR, i2cRdBuf, 1, I2C_MODE_COMPLETE_XFER);
    while(!I2C_MSTAT_RD_CMPLT);
    I2C_MasterClearStatus();
    CyDelay(10);
    
    i2cWrBuf[0] = 0b01110000; // slave addr ; b[0] = 0 for write mode
    
    // check if calibration command needed
    if((int)i2cRdBuf & (0 << 3)){
        i2cWrBuf[1] = 0xBE; // initialization cmd
        i2cWrBuf[2] = 0x08;
        i2cWrBuf[3] = 0x00;
        I2C_MasterWriteBuf(AHT_ADDR, (uint8 *)i2cWrBuf, 4, I2C_MODE_COMPLETE_XFER);
        while(!(I2C_MasterStatus() & I2C_MSTAT_WR_CMPLT));
        I2C_MasterClearStatus();
        
        CyDelay(10);
    }
}

void takeMeasurementAHT(float* tempF, float* humid){
    uint8 i2cWrBuf[7], i2cRdBuf[7];
    
    i2cWrBuf[0] = 0b01110000; // slave addr ; b[0] = 0 for write mode
    i2cWrBuf[1] = 0xAC; // start measurement cmd
    i2cWrBuf[2] = 0b00110011;
    i2cWrBuf[3] = 0b00000000;
    I2C_MasterWriteBuf(AHT_ADDR, (uint8 *)i2cWrBuf, 4, I2C_MODE_COMPLETE_XFER);
    while(!(I2C_MasterStatus() & I2C_MSTAT_WR_CMPLT));
    I2C_MasterSendStop();
    I2C_MasterClearStatus();
    
    //datasheet recommends 80 second delay for measurement to take place
    CyDelay(80);
    
    // read AHT measurement
    i2cWrBuf[0] = 0b01110001; // slave addr ; b[0] = 1 for read mode
    I2C_MasterWriteBuf(AHT_ADDR, (uint8 *)i2cWrBuf, 1, I2C_MODE_COMPLETE_XFER);
    while(!(I2C_MasterStatus() & I2C_MSTAT_WR_CMPLT));
    I2C_MasterSendStop();
    I2C_MasterClearStatus();
    CyDelay(10);

    I2C_MasterReadBuf(AHT_ADDR, (uint8 *)i2cRdBuf, 7, I2C_MODE_COMPLETE_XFER); // get status
    while (!I2C_MSTAT_RD_CMPLT);
    I2C_MasterClearStatus();
    CyDelay(10);

    while(i2cRdBuf[0] & (1 << 7)); //check that bit 7 (busy) is low

    CyWdtClear();

    // convert readings to temp and humidity
    *humid = convertHumidity(i2cRdBuf[1], i2cRdBuf[2], i2cRdBuf[3]);
    *tempF = convertTempF(i2cRdBuf[5], i2cRdBuf[4], i2cRdBuf[3]);
}

void restartAHT(){
    uint8 i2cWrBuf[2];
    
    i2cWrBuf[0] = 0b01110000; // slave addr ; b[0] = 0 for write mode
    i2cWrBuf[1] = 0b10111010;
    I2C_MasterWriteBuf(AHT_ADDR, (uint8 *)i2cWrBuf, 2, I2C_MODE_COMPLETE_XFER);
    while(!(I2C_MasterStatus() & I2C_MSTAT_WR_CMPLT));
    I2C_MasterClearStatus();
    CyDelay(10);
}

void printTempHumid(float temp, float humid){
    
    char s[50];
    
    //print temp to uart
    sprintf(s, "Temperature: %.2f F\r\n", temp);
    
    //clear and set up OLED settings, print temp
    display_clear();    
    display_update();    
    gfx_setTextSize(1);
    gfx_setTextColor(WHITE);
    gfx_setCursor(2, 2);
    gfx_println(s);
    
    //print humidity to uart and OLED
    sprintf(s, "Humidity: %.2f \%% \r\n", humid);
    gfx_setCursor(2, 20);
    gfx_println(s);
    display_update(); 
//    I2C_MasterSendStop();
//    I2C_MasterClearStatus();
   
    //print menu to uart
    sprintf(s, "Menu");   
    gfx_setCursor(50,50);
    gfx_println(s);
    display_update(); 
    I2C_MasterSendStop();
    I2C_MasterClearStatus();
}

float convertTempF(uint8 num1, uint8 num2, uint8 num3){
    // combine 2 8 bit temperature data bytes and lower 4 bits of humidity/temperature byte
    int num16b = (num2 << 8) | num1;   
    int lowernum3 = num3 & 0x0F;
    int num20b = (lowernum3 << 16) | num16b;
    
    //convert to temp in C using formula in datasheet
    float tempC = (((double)num20b * 200.0)/pow(2.0, 20.0)) - 50.0;
    
    //convert to F
    float tempF = (tempC * 9/5) + 32.0;
    
    return tempF;
}


float convertHumidity(uint8 num1, uint8 num2, uint8 num3){
    // combine 2 8 bit humidity data bytes and upper 4 bits of humidity/temperature byte
    int num16b = (num1 << 8) | num2;
    int uppernum3 = num3 >> 4;
    int num20b = (num16b << 4) | uppernum3;
    
    //convert to humidity percentage using formula in datasheet
    float result = ((double)num20b/pow(2.0, 20.0)) * 100.0;
    
    return result;
}

void checkParam(float tempF, float humid){

    // check if any params are in alert range
    if((tempF > TH) || (tempF < TL) || (humid > HH) || (humid < HL)){ 
    
        // temp high alert
        if(tempF > TH){
            LED_T_G_Write(0);
            LED_T_Y_Write(0);
            LED_T_R_Write(1);
            
            Tout_Write(0);
            alertFlag = 1;  // flag = 1 for high temp alert
           
        }
        // temp low alert
        else if(tempF < TL){
            LED_T_G_Write(0);
            LED_T_Y_Write(0);
            LED_T_R_Write(1);
            
            Tout_Write(1);
            alertFlag = 2;  // flag = 2 for low temp alert
        }
        else{
            LED_T_R_Write(0);
            LED_T_Y_Write(0);
            LED_T_G_Write(1);
            
            Tout_Write(0);
        }
        // humid high alert
        if(humid > HH){
            if(alertFlag == 1){
                LED_H_G_Write(0);
                LED_H_Y_Write(0);
                LED_H_R_Write(1);
                
                alertFlag = 5;   //flag = 5 for T high and H high   
            }
            else if(alertFlag == 2){
                LED_H_G_Write(0);
                LED_H_Y_Write(0);
                LED_H_R_Write(1);
               
                alertFlag = 6;  //flag = 6 for T low and H high   
            }
            else{
                LED_H_G_Write(0);
                LED_H_Y_Write(0);
                LED_H_R_Write(1);

                alertFlag = 3;  // flag = 3 for high humidity alert
            }

            Hout_Write(0);
           
        }
        // humid low alert
        else if(humid < HL){
            if(alertFlag == 1){
                LED_H_G_Write(0);
                LED_H_Y_Write(0);
                LED_H_R_Write(1);
                
                alertFlag = 7;   //flag = 7 for T high and H low 
            }
            else if(alertFlag == 2){
                LED_H_G_Write(0);
                LED_H_Y_Write(0);
                LED_H_R_Write(1);
                
                alertFlag = 8;  //flag = 6 for T low and H low  
            }
            else{
                LED_H_G_Write(0);
                LED_H_Y_Write(0);
                LED_H_R_Write(1);

                alertFlag = 4; // flag = 4 for H low
            }

            if(!mistFlag){
                // turn mister on for 10 seconds
                UART_PutString("MISTING...");
                Hout_Write(1);
                mistTimer_Stop();
                mistTimer_WriteCounter(0);
                mistTimer_WritePeriod(1000);
                mistFlag = 1;       //mistFlag = 1 for currently misting
                
                mistTimer_Enable();
            }

           
        }
        else{
            LED_H_R_Write(0);
            LED_H_Y_Write(0);
            LED_H_G_Write(1);
        }

        CyWdtClear();

    }
    // check if any params within notice range (+- tol/2)
    if((tempF > (TH - tolT/2) && tempF < TH) || (tempF < (TL + tolT/2) && tempF > TL)|| (humid > (HH - tolH/2) && humid < HH) || (humid < (HL + tolH/2) && humid > HL)){    
        
        // temp high notice
        if(tempF > (TH - tolT/2) && tempF < TH){
            noticeFlag = 1;
            
            LED_T_G_Write(0);
            LED_T_R_Write(0);
            LED_T_Y_Write(1);
        }
        // temp low notice
        else if(tempF < (TL + tolT/2) && tempF > TL){
            noticeFlag = 2;
            
            LED_T_G_Write(0);
            LED_T_R_Write(0);
            LED_T_Y_Write(1);
        }
        // humid high notice
        if(humid > (HH - tolH/2) && humid < HH){
            // temp high and humid high notice
            if(noticeFlag == 1){
                LED_H_G_Write(0);
                LED_H_R_Write(0);
                LED_H_Y_Write(1);
                
                noticeFlag = 5;
            }
            else if(noticeFlag == 2){
                LED_H_G_Write(0);
                LED_H_R_Write(0);
                LED_H_Y_Write(1);
                
                noticeFlag = 6;
            }
            else{
                LED_H_G_Write(0);
                LED_H_R_Write(0);
                LED_H_Y_Write(1);
                
                noticeFlag = 3;
            }
          
        }
        // humid low notice
        else if(humid < (HL + tolH/2) && humid > HL){
            // temp high and humid high notice
            if(noticeFlag == 1){
            LED_H_G_Write(0);
            LED_H_R_Write(0);
            LED_H_Y_Write(1);

            noticeFlag = 7;
            }
            else if(noticeFlag == 2){
            LED_H_G_Write(0);
            LED_H_R_Write(0);
            LED_H_Y_Write(1);

            noticeFlag = 8;
            }
            else{
            LED_H_G_Write(0);
            LED_H_R_Write(0);
            LED_H_Y_Write(1);

            noticeFlag = 4;
            }
        }
    }
    if((tempF < (TH - tolT/2) && tempF > (TL + tolT/2)) ||  (humid < (HH - tolH/2) && humid > (HL + tolH/2))){
        if((tempF < (TH - tolT/2) && tempF > (TL + tolT/2))){
            LED_T_Y_Write(0);
            LED_T_R_Write(0);
            LED_T_G_Write(1);
        }
        if(humid < (HH - tolH/2) && humid > (HL + tolH/2)){
            LED_H_R_Write(0);       
            LED_H_Y_Write(0);            
            LED_H_G_Write(1);
        }       

    }
    CyWdtClear();
}

// update max and min temp and humidities
void setTol(){
  TH = SetTemp + tolT;
  TL = SetTemp - tolT;
  HH = SetHumid + tolH;
  HL = SetHumid - tolH;

  CyWdtClear();
}

// AHT modules have hard coded i2c address, must multiplex to interface with all three
void changeI2CDevice(int dev){
    switch(dev){
        // i2c device 0 - AHT 0, OLED display
        case 0:
            // set SDA1, SD2, and SCL to high impedance mode
            SDA1_SetDriveMode(PIN_DM_DIG_HIZ);
            SDA2_SetDriveMode(PIN_DM_DIG_HIZ);
            SCL_SetDriveMode(PIN_DM_DIG_HIZ);
            
            // write 0 to control reg to select SDA0
            SDA_CTL_Write(0u);
            
            // set SDA0 and shared SCL to open drain
            SDA0_SetDriveMode(PIN_DM_OD_LO);
            SCL_SetDriveMode(PIN_DM_OD_LO);
            break;
            
        // i2c device 1 - AHT 1
        case 1:
            SDA0_SetDriveMode(PIN_DM_DIG_HIZ);
            SDA2_SetDriveMode(PIN_DM_DIG_HIZ);
            SCL_SetDriveMode(PIN_DM_DIG_HIZ);
            SDA_CTL_Write(1u);
            SDA1_SetDriveMode(PIN_DM_OD_LO);
            SCL_SetDriveMode(PIN_DM_OD_LO);
            break;
        
        // i2c device 2 - AHT 2
        case 2:
            SDA0_SetDriveMode(PIN_DM_DIG_HIZ);
            SDA1_SetDriveMode(PIN_DM_DIG_HIZ);
            SCL_SetDriveMode(PIN_DM_DIG_HIZ);
            SDA_CTL_Write(2u);
            SDA2_SetDriveMode(PIN_DM_OD_LO);
            SCL_SetDriveMode(PIN_DM_OD_LO);
            break;

    }
}

void checkISwitches(){
    // check that heat lamp and mister are in the correct state
    // Tout & Hout will be 1 if device should be on
    int tOut = Tout_Read();
    int hOut = Hout_Read();
    
    // current switches normally open, will be 1 if device is actually on
    int tSwitch = I_SW_T_Read();
    int hSwitch = I_SW_H_Read();
    
    // if lamp should be on but isn't
    if(tOut && !tSwitch){
        equipFlag = 1;
    }
    // lamp should not be on but is
    else if(!tOut && tSwitch){
        equipFlag = 2;  
    }
    
    // if mister should be on but isn't
    if(hOut && !hSwitch){
        if(equipFlag == 1){
            // lamp and mister off
            equipFlag = 5;
        }
        else if(equipFlag == 2){
            // lamp on, mister off
            equipFlag = 6;    
        }
        else{
            equipFlag = 3;   
        }
    }
    // mister should be off but is on
    else if(!hOut && hSwitch){
        if(equipFlag == 1){
            // lamp off, mister on
            equipFlag = 7;
        }
        else if(equipFlag == 2){
            // lamp and mister on
            equipFlag = 8;    
        }
        else{
            equipFlag = 4;   
        }
    }
    
}
/* [] END OF FILE */
