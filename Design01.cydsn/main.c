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
void S_sequence();
void P_sequence();


int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    UART_Start();
    rx_int_StartEx(uart_int_Handler);
    
    I2C_Start();
    
    char s[80];
    uint8 i2cWrBuf[3];
    uint8 i2cRdBuf[7];
    
    RestartAHT();
    CyDelay(20);
    
    CyDelay(40); // wait 40ms after AHT power on
//    initializeAHT();
    CyDelay(80); // wait 80ms for measurement to complete
       
    sprintf(s, "initialized\r\n");
    UART_PutString(s); 
    
    S_sequence();
    i2cWrBuf[0] = 0b01110001; // slave addr ; b[0] = 1 for read mode
    I2C_MasterWriteBuf(AHT_ADDR, (uint8 *)i2cWrBuf, 1, I2C_MODE_COMPLETE_XFER);
    while(SDA_Read()); // SDA pulled low for ACK

    I2C_MasterReadBuf(AHT_ADDR, (uint8 *)i2cRdBuf, 7, I2C_MODE_COMPLETE_XFER); // get status
    
    while(i2cRdBuf[0] >= 128){
        i2cWrBuf[0] = 0b01110001; // slave addr ; b[0] = 1 for read mode
        I2C_MasterWriteBuf(AHT_ADDR, (uint8 *)i2cWrBuf, 1, I2C_MODE_COMPLETE_XFER);
        while(SDA_Read()); // SDA pulled low for ACK

        I2C_MasterReadBuf(AHT_ADDR, (uint8 *)i2cRdBuf, 7, I2C_MODE_COMPLETE_XFER); // get status
        
        sprintf(s, "%i\r\n", i2cRdBuf[0]);
        UART_PutString(s);
        
        CyDelay(500);
    }
    
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
    
    S_sequence();
    
    i2cWrBuf[0] = 0b01110000; // slave addr ; b[0] = 0 for write mode
    I2C_MasterWriteBuf(AHT_ADDR, (uint8 *)i2cWrBuf, 1, I2C_MODE_COMPLETE_XFER);
    while(SDA_Read()); // SDA pulled low for ACK
    
    i2cWrBuf[0] = 0xBE; // initialization cmd
    I2C_MasterWriteBuf(AHT_ADDR, (uint8 *)i2cWrBuf, 1, I2C_MODE_COMPLETE_XFER);
    while(SDA_Read()); // SDA pulled low for ACK

    i2cWrBuf[0] = 0x08; // initialization cmd
    I2C_MasterWriteBuf(AHT_ADDR, (uint8 *)i2cWrBuf, 1, I2C_MODE_COMPLETE_XFER);
    while(SDA_Read()); // SDA pulled low for ACK
    
    i2cWrBuf[0] = 0x00; // initialization cmd
    I2C_MasterWriteBuf(AHT_ADDR, (uint8 *)i2cWrBuf, 1, I2C_MODE_COMPLETE_XFER);
    while(SDA_Read()); // SDA pulled low for ACK
    
    P_sequence();
    CyDelay(10);
    
    S_sequence();
    i2cWrBuf[0] = 0b01110000; // slave addr ; b[0] = 0 for write mode
    I2C_MasterWriteBuf(AHT_ADDR, (uint8 *)i2cWrBuf, 1, I2C_MODE_COMPLETE_XFER);
    while(SDA_Read()); // SDA pulled low for ACK
    
    i2cWrBuf[0] = 0xAC; // start measurement cmd
    I2C_MasterWriteBuf(AHT_ADDR, (uint8 *)i2cWrBuf, 1, I2C_MODE_COMPLETE_XFER);
    while(SDA_Read()); // SDA pulled low for ACK
    
    i2cWrBuf[0] = 0x33;
    I2C_MasterWriteBuf(AHT_ADDR, (uint8 *)i2cWrBuf, 1, I2C_MODE_COMPLETE_XFER);
    while(SDA_Read()); // SDA pulled low for ACK
    
    i2cWrBuf[0] = 0x00;
    I2C_MasterWriteBuf(AHT_ADDR, (uint8 *)i2cWrBuf, 1, I2C_MODE_COMPLETE_XFER);
    while(SDA_Read()); // SDA pulled low for ACK    
    
    P_sequence();
    
}

void RestartAHT(){
    uint8 i2cWrBuf[1];

    S_sequence();
    i2cWrBuf[0] = 0b01110000; // slave addr ; b[0] = 0 for write mode
    I2C_MasterWriteBuf(AHT_ADDR, (uint8 *)i2cWrBuf, 1, I2C_MODE_COMPLETE_XFER);
    while(SDA_Read()); // SDA pulled low for ACK
    
    i2cWrBuf[0] = 0b10111010;
    I2C_MasterWriteBuf(AHT_ADDR, (uint8 *)i2cWrBuf, 1, I2C_MODE_COMPLETE_XFER);
    while(SDA_Read()); // SDA pulled low for ACK
    P_sequence();
}

// Start Transmit State
void S_sequence(){
    SDA_Write(0);
    CyDelay(10);
    SCL_Write(0);
}

// Stop Transmit State
void P_sequence(){
    SCL_Write(1);
    CyDelay(10);
    SDA_Write(1);
}

/* [] END OF FILE */
