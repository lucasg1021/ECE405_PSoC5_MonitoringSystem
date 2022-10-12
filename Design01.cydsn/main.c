/* ========================================
 *
 * ECE405 Pet Environment Monitoring System
 *              Fall 2022
 *
 * ========================================
*/
#include "project.h"
#include "aht.h"
#include "esp.h"
#include "circbuf.h"
#include "ssd1306.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define TIMEOUT_ESP 10000  
#define ESP_CIRCBUF_LEN 64  

volatile int connection = 0; // flag indicating whether a device is currently connected (0 for no connection, 1 for connected)
volatile int keyFlag = 0; // indicates if startup sequence has been executed and key has been set
volatile int PRIV;
volatile int BASE = 7;
volatile int timeoutCount = 0;
volatile long long MOD = 2147483647;
volatile unsigned KEY = 0;

volatile int SetTemp = 70;
volatile int SetHumid = 50;
volatile int TH, HH;
volatile int TL, HL;
volatile int tol = 2;
volatile int tolh = 2;
volatile float tempF, humid;
volatile int Select;

// declare circular buffer data array and variable
uint8_t espStringData[ESP_CIRCBUF_LEN];
circBufESP espBuf = {
    .buffer = espStringData,
    .head = 0,
    .tail = 0,
    .length = ESP_CIRCBUF_LEN
};

CY_ISR(esp_int_Handler){
    char c = ESPUART_GetChar();
    circBufPush(&espBuf, c);
    ESPUART_ClearRxBuffer();
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    ESPUART_Start();
    ESPUART_ClearRxBuffer();
    ESPUART_ClearTxBuffer();
    UART_Start();
//    rx_int_StartEx(uart_int_Handler);
    esprx_int_StartEx(esp_int_Handler);  
    I2C_Start();
        
    char s[80], sESP[80];
    char baseS;
    uint8 i2cWrBuf[3], i2cRdBuf[7], cESP;
    int baseESP, modESP;
    float tempF, humidity;

    CyDelay(1000);
    ESP_RST_Write(1);
    
    // initialize wifi settings and join network
    initESP(sESP);

    // restart and initialize temp/humid sensor
    restartAHT();
    I2C_MasterSendStop();
    I2C_MasterClearStatus();
    CyDelay(100); // wait 40ms after AHT power on
    initializeAHT();
    CyDelay(80); // wait 80ms for measurement to complete
    
    // initialize OLED
    // must stop sending and clear status to work with AHT
    display_init(DISPLAY_ADDRESS);
    I2C_MasterSendStop();
    I2C_MasterClearStatus();
    
    // start WDT
    // CyWdtStart(CYWDT_1024_TICKS, CYWDT_LPMODE_NOCHANGE);
    CyWdtClear();
    
    for(;;)
    {            
        // every loop check for connection, if none listen on port 54321
        if(connection == 0){
            initUDPConnectionESP(sESP);
            if(!waitForResponseESP("OK", sESP, 1000)){
                CyWdtClear();
                waitForResponseESP("OK", sESP, 1000);
                CyWdtClear();
              
            }
            else if(!waitForResponseESP("ALREADY CONNECTED\r\n\n", sESP, 1000)){
                CyWdtClear();
                if(!waitForResponseESP("ERROR\r\n", sESP, 1000)){
                    CyWdtClear();
                    waitForResponseESP("OK", sESP, 1000);
                    CyWdtClear();
                }
            }
            CyWdtClear();
        }
        
        takeMeasurementAHT();   // measure temp and humid

        // read AHT measurement
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
        
        CyWdtClear();
        
        // convert readings to temp and humidity
        humidity = convertHumidity(i2cRdBuf[1], i2cRdBuf[2], i2cRdBuf[3]);
        tempF = convertTempF(i2cRdBuf[5], i2cRdBuf[4], i2cRdBuf[3]);
        
        // print to OLED
        printTempHumid(tempF, humidity);
                
        //clear I2C buffer
        I2C_MasterClearReadBuf();
        memset(i2cRdBuf, 0, sizeof i2cRdBuf);

        CyWdtClear();
        // if connected to app, send temp and humidity information
        if(connection){
            
            sprintf(s,"%.2f %.2f DATA", tempF, humidity);

            if(keyFlag){
                encryptESP(s, KEY, 16);
            }
            CyWdtClear();

            // send 11 bytes of data
            sprintf(sESP, "AT+CIPSEND=0,%i\r\n\n", 16);
            ESPUART_PutString(sESP);
            waitForResponseESP(">", sESP, 2000);
            CyWdtClear();
            
            // send to connected device
            ESPUART_PutString(s);
            waitForResponseESP("OK\r\n", sESP, 1000);
           
            closeConnectionESP(sESP);
            CyWdtClear();
                                    
            connection = 0;           
        }

        CyWdtClear();
        CyDelay(1000);
        CyWdtClear();
        timeoutCount = 0;
    }
}
/* [] END OF FILE */
