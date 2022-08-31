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
#include <math.h>

#define AHT_ADDR (uint8)(0x38)

CY_ISR(uart_int_Handler){
    char c = UART_GetChar();
    UART_PutChar(c);
    UART_ClearRxBuffer();
}

void initializeAHT();
void takeMeasurementAHT();
void RestartAHT();
int convert8bto16b(uint8 num1, uint8 num2);
float convertTemp(uint8 num1, uint8 num2, uint8 num3);


int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    UART_Start();
    rx_int_StartEx(uart_int_Handler);
    
    I2C_Start();
    
    // start sensor
    SCL_Write(0);
    CyDelay(10);

    RestartAHT();
    
    SCL_Write(1);
    CyDelay(20);
    
    char s[80];
    uint8 i2cWrBuf[3];
    uint8 i2cRdBuf[10];
    uint8 num1, num2, num3;
    
    float tempC;
    
    CyDelay(40); // wait 40ms after AHT power on
    initializeAHT();
    CyDelay(80); // wait 80ms for measurement to complete
       
    sprintf(s, "initialized\r\n");
    UART_PutString(s); 
    
    i2cWrBuf[0] = 0b01110001; // slave addr ; b[0] = 1 for read mode
    I2C_MasterWriteBuf(AHT_ADDR, (uint8 *)i2cWrBuf, 1, I2C_MODE_COMPLETE_XFER);
    while(SDA_Read());
    while(!(I2C_MasterStatus() & I2C_MSTAT_WR_CMPLT));
    I2C_MasterClearStatus();
    CyDelay(10);
   

    I2C_MasterReadBuf(AHT_ADDR, (uint8 *)i2cRdBuf, 7, I2C_MODE_COMPLETE_XFER); // get status
    while (!I2C_MSTAT_RD_CMPLT);
    I2C_MasterClearStatus();
    CyDelay(10);
    
    
    while(i2cRdBuf[0] & (1 << 7)); //check that bit 7 (busy) is low
    
     //print status a measurement values
    for(int i = 0; i < 7; i++){
        sprintf(s, "%i\r\n", i2cRdBuf[i]);
        UART_PutString(s);
    }
    
    num1 = i2cRdBuf[5];
    num2 = i2cRdBuf[4];
    num3 = i2cRdBuf[3];
    tempC = convertTemp(num1, num2, num3);
    sprintf(s, "%.2f\r\n", tempC);
    UART_PutString(s);
    
    I2C_MasterClearReadBuf();
    
    for(;;)
    {
        takeMeasurementAHT();
        
            i2cWrBuf[0] = 0b01110001; // slave addr ; b[0] = 1 for read mode
    I2C_MasterWriteBuf(AHT_ADDR, (uint8 *)i2cWrBuf, 1, I2C_MODE_COMPLETE_XFER);
    while(SDA_Read());
    while(!(I2C_MasterStatus() & I2C_MSTAT_WR_CMPLT));
    I2C_MasterClearStatus();
    CyDelay(10);
   

    I2C_MasterReadBuf(AHT_ADDR, (uint8 *)i2cRdBuf, 7, I2C_MODE_COMPLETE_XFER); // get status
    while (!I2C_MSTAT_RD_CMPLT);
    I2C_MasterClearStatus();
    CyDelay(10);
    
    while(i2cRdBuf[0] & (1 << 7)); //check that bit 7 (busy) is low
    
    num1 = i2cRdBuf[5];
    num2 = i2cRdBuf[4];
    num3 = i2cRdBuf[3];
    tempC = convertTemp(num1, num2, num3);
    sprintf(s, "%.2f\r\n", tempC);
    UART_PutString(s);
    
    I2C_MasterClearReadBuf();

    CyDelay(500);
    }
}

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
    
    takeMeasurementAHT();
   
}

void takeMeasurementAHT(){
    uint8 i2cWrBuf[4];
    
    i2cWrBuf[0] = 0b01110000; // slave addr ; b[0] = 0 for write mode
    i2cWrBuf[1] = 0xAC; // start measurement cmd
    i2cWrBuf[2] = 0b00110011;
    i2cWrBuf[3] = 0b00000000;
    I2C_MasterWriteBuf(AHT_ADDR, (uint8 *)i2cWrBuf, 4, I2C_MODE_COMPLETE_XFER);
    while(!(I2C_MasterStatus() & I2C_MSTAT_WR_CMPLT));
    I2C_MasterClearStatus();
    
    //datasheet recommends 80 second delay for measurement to take place
    CyDelay(80);
}

void RestartAHT(){
    uint8 i2cWrBuf[1];

    i2cWrBuf[0] = 0b01110000; // slave addr ; b[0] = 0 for write mode
    I2C_MasterWriteBuf(AHT_ADDR, i2cWrBuf, 1, I2C_MODE_COMPLETE_XFER);
    while(SDA_Read()); // SDA pulled low for ACK
    I2C_MasterClearStatus();
    
    i2cWrBuf[0] = 0b10111010;
    I2C_MasterWriteBuf(AHT_ADDR, i2cWrBuf, 1, I2C_MODE_COMPLETE_XFER);
    while(SDA_Read()); // SDA pulled low for ACK
    I2C_MasterClearStatus();
}

int convert8bto16b(uint8 num1, uint8 num2){
    int result = (num2 << 8) | num1;
    
    return result;
}

float convertTemp(uint8 num1, uint8 num2, uint8 num3){
    // combine 2 8 bit temperature data bytes and lower 4 bits of humidity/temperature byte
    int num16b = (num2 << 8) | num1;   
    int lowernum3 = num3 & 0x0F;
    int num20b = (lowernum3 << 16) | num16b;
    
    //convert to temp in C using 
    float result = (((double)num20b * 200.0)/pow(2.0, 20.0)) - 50.0;
    
    return result;
}

/* [] END OF FILE */
