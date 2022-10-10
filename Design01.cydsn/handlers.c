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

extern volatile int TH, HH;
extern volatile int TL, HL;
extern volatile int tol, tolh;
extern volatile int SetTemp, SetHumid;
extern volatile float tempF, humid;
extern uint8 i2cBuffer[17];


CY_ISR(uart_int_Handler){
    char c = UART_GetChar();
    UART_PutChar(c);
    UART_ClearRxBuffer();
}

CY_ISR(enc_int_Handler){

    CyDelay(1);
 
    if(ENC_Read() == ENC_INTR_RISING){
        SetTemp = SetTemp + 1;
    }
    else if(ENC_Read() == ENC_INTR_FALLING){
        SetTemp = SetTemp - 1; 
    }
 
    TL = (SetTemp - tol); //Change value of TL 
    TH = (SetTemp + tol); //Change value of TH
    HL = (SetHumid - tolh); //Change value of HL
    HH = (SetHumid + tolh); //Change value of HH
 
    i2cBuffer[0] = 0xA1; //Reconfigure TH
    i2cBuffer[1] = TH;
    i2cBuffer[2] = 0x00;
    I2C_MasterWriteBuf((uint8)0x48,i2cBuffer,4,I2C_MODE_COMPLETE_XFER);
    while (!(I2C_MasterStatus() & I2C_MSTAT_WR_CMPLT));
    I2C_MasterClearStatus();
 
    i2cBuffer[0] = 0xA2; //Reconfigure TL
    i2cBuffer[1] = TL;
    i2cBuffer[2] = 0x00;
    I2C_MasterWriteBuf((uint8)0x48,i2cBuffer,3,I2C_MODE_COMPLETE_XFER);
    while (!(I2C_MasterStatus() & I2C_MSTAT_WR_CMPLT));
    I2C_MasterClearStatus();
 
 //ENC_ClearInterrupt();
    
}

CY_ISR(rx_int_Handler){
    
    
}

CY_ISR(sw2_int_Handler){
    
    
    
}


CY_ISR(tout_int_Handler){
    if(tempF > TH){
         //Turn off lamp
          // Alarm high
        }
        else if(tempF < TL){
            //Turn on lamp  
            // Alarm low
        }
        if(humid > HH){
            // Alarm High
        }
        else if(humid < HL){
            //Turn on mister for X min
            // Alarm low
        }
        else{
         //Alarms OK   
        }
        Tout_ClearInterrupt();
    
}

//CY_ISR(esp_int_Handler){
//    char c;
//    char s[80];
//    int i = 0;
//    
//    while((c = ESPUART_GetChar()) != '\0'){
//        s[i] = c;
//        i++;
//        UART_PutChar(c);
//    }
//
//    ESPUART_ClearRxBuffer();
/* [] END OF FILE */
