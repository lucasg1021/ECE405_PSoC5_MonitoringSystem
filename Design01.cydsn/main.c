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
#include <stdio.h>

#define AHT_ADDR 0x37

CY_ISR(uart_int_Handler){
    char c = UART_GetChar();
    UART_PutChar(c);
    UART_ClearRxBuffer();
}

void initializeAHT();
void RestartAHT();


int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    UART_Start();
    rx_int_StartEx(uart_int_Handler);
    
    I2C_Start();
    
    char s[80];
    uint8 i2cWrBuf[3];
    uint8 i2cRdBuf[7];
    
    CyDelay(40); // wait 40ms after AHT power on
    initializeAHT();
    CyDelay(80); // wait 80ms for measurement to complete
       
    sprintf(s, "initialized\r\n");
    UART_PutString(s); 
    
    i2cWrBuf[0] = 0b01110001; // slave addr ; b[0] = 1 for read mode
    I2C_MasterWriteBuf(AHT_ADDR, (uint8 *)i2cWrBuf, 1, I2C_MODE_COMPLETE_XFER);
    while(SDA_Read()); // SDA pulled low for ACK

    I2C_MasterReadBuf(AHT_ADDR, (uint8 *)i2cRdBuf, 7, I2C_MODE_COMPLETE_XFER); // get status
    
    while((int)i2cRdBuf & (1 << 7)); //check that bit 7 (busy) is low
    
    // print status a measurement values
    for(int i = 0; i < 7; i++){
        sprintf(s, "%i\r\n", i2cRdBuf[i]);
        UART_PutString(s);
    }
    
    for(;;)
    {
    }
}

void initializeAHT(){
    uint8 i2cWrBuf[4];
    
    i2cWrBuf[0] = 0b01110000; // slave addr ; b[0] = 0 for write mode
    I2C_MasterWriteBuf(AHT_ADDR, (uint8 *)i2cWrBuf, 1, I2C_MODE_COMPLETE_XFER);
    while(SDA_Read()); // SDA pulled low for ACK
    
    i2cWrBuf[0] = 0xBE; // initialization cmd
    I2C_MasterWriteBuf(AHT_ADDR, (uint8 *)i2cWrBuf, 1, I2C_MODE_COMPLETE_XFER);
    while(SDA_Read()); // SDA pulled low for ACK
    
    CyDelay(10);
    
    i2cWrBuf[0] = 0xAC; // start measurement cmd
    I2C_MasterWriteBuf(AHT_ADDR, (uint8 *)i2cWrBuf, 1, I2C_MODE_COMPLETE_XFER);
    while(SDA_Read()); // SDA pulled low for ACK
    
}

void RestartAHT(){
    uint8 i2cWrBuf[1];

    i2cWrBuf[0] = 0b01110000; // slave addr ; b[0] = 0 for write mode
    I2C_MasterWriteBuf(AHT_ADDR, (uint8 *)i2cWrBuf, 1, I2C_MODE_COMPLETE_XFER);
    while(SDA_Read()); // SDA pulled low for ACK
    
    i2cWrBuf[0] = 0b10111010;
    I2C_MasterWriteBuf(AHT_ADDR, (uint8 *)i2cWrBuf, 1, I2C_MODE_COMPLETE_XFER);
    while(SDA_Read()); // SDA pulled low for ACK
}

/* [] END OF FILE */
