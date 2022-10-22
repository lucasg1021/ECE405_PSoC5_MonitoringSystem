
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
#include "menu.h"
#include "EEPROM.h"
#include "circbuf.h"
#include "ssd1306.h"
#include "Tout.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define TIMEOUT_ESP 10000  
#define ESP_CIRCBUF_LEN 64  

volatile int connection = 0; // flag indicating whether a device is currently connected (0 for no connection, 1 for connected)
volatile int keyFlag = 1; // indicates if startup sequence has been executed and key has been set
volatile int PRIV;
volatile int BASE = 7;
volatile long long MOD = 2147483647;
volatile unsigned KEY = 123;

volatile int SetTemp;
volatile int SetHumid;
volatile int TH, HH;
volatile int TL, HL;
volatile int tolT = 2;
volatile int tolH = 2;
volatile int Select = 0;

volatile int ENC_Flag = 0;
volatile int SW1_Flag = 0;
volatile int SW2_Flag = 0;
volatile int alertFlag = 0;
volatile int noticeFlag = 0;
volatile int mistFlag = 2;

volatile char * wifi_ssid;
volatile char * wifi_pwd;

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

    esprx_int_StartEx(esp_int_Handler);
//    TOUT_ISR_Start();

    I2C_Start();
    
    // set which i2c to use
    changeI2CDevice(0);
    
    mistISR_Start();
    mistTimer_WriteCounter(0);
    mistTimer_WritePeriod(6000);     // 6000/(100Hz CLK) = 1 minute period before misting can begin
    mistTimer_Enable();
        
    char s[80], sESP[80], eepromS[30];
    char* str;
    uint8 i2cWrBuf[3], i2cRdBuf[7];
    volatile uint8_t eepromWrBuf[30], eepromRdBuf[30], eepromChar;
    int baseESP, modESP;
    float tempF, humid;

    ESP_RST_Write(1);
    
    LED_T_G_Write(1);
    LED_H_G_Write(1);
    
    //** Manual Write EEPROM ** //     

//    uint8_t string[11] = "";
//    string[10] = '\n';
//    
//    uint8_t string2[4] = "";
//    string2[3] = '\n';
//    
//    uint8_t string3[1];
//    string3[0] = 0x4A;                //Temp
//    
//    uint8_t string4[1];
//    string4[0] = 50;               //Humidity 
    
//    uint8_t string5[1];
//    string5[0] = 2;           // tolT
//    uint8_t string6[1];
//    string6[0] = 2;// tolH
//
//    // write/read wifi ssid
//    writeEEPROM(WIFISSID_STARTADDR, string2, 4);
//    writeEEPROM(WIFIPWD_STARTADDR, string, 11);
//    writeEEPROM(SETTEMP_STARTADDR, string3, 1);
//    writeEEPROM(SETHUMID_STARTADDR, string4, 1);
//    writeEEPROM(TOLT_STARTADDR, string5, 1);
//    writeEEPROM(TOLH_STARTADDR, string6, 1);
    
  //***************************************//  
    
    // get intial values from EEPROM
    readEEPROM(WIFISSID_STARTADDR, eepromS, 4);
    wifi_ssid = strdup(strtok(eepromS, "\n"));
    
    readEEPROM(WIFIPWD_STARTADDR, eepromS, 11);
    wifi_pwd = strdup(strtok(eepromS, "\n"));
    
    readEEPROM(SETTEMP_STARTADDR, eepromS, 1);
    SetTemp = eepromS[0];
    
    readEEPROM(SETHUMID_STARTADDR, eepromS, 1);
    SetHumid = eepromS[0];
    
    readEEPROM(TOLT_STARTADDR, eepromS, 1);
    tolT = eepromS[0];
    
    readEEPROM(TOLH_STARTADDR, eepromS, 1);
    tolH = eepromS[0];

    sprintf(s, "%d", SetTemp);
    UART_PutString((char*)wifi_ssid);
    UART_PutString((char*)wifi_pwd);
    UART_PutString(s);
    sprintf(s, "%d %d %d", SetHumid, tolT, tolH);
    UART_PutString(s);
   
    
    // initialize wifi settings and join network
    initESP(sESP);

    // restart and initialize temp/humid sensor
    restartAHT();
    I2C_MasterSendStop();
    I2C_MasterClearStatus();
    CyDelay(100); // wait 40ms after AHT power on
    initializeAHT();
    CyDelay(80); // wait 80ms for measurement to complete
    changeI2CDevice(0);
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
//    CyWdtStart(CYWDT_1024_TICKS, CYWDT_LPMODE_NOCHANGE);
    CyWdtClear();

    SW1_ISR_Start();
    SW2_ISR_Start();
    ENC_ISR_Start();

    setTol();
    
    for(;;)
    {            

        // check if user hit button
        if((SW1_Flag == 1) || (SW2_Flag == 1)){ menu(); }
        
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
        
        changeI2CDevice(1);
        takeMeasurementAHT(&tempF, &humid);   // measure temp and humid


//        // read AHT measurement
//        i2cWrBuf[0] = 0b01110001; // slave addr ; b[0] = 1 for read mode
//        I2C_MasterWriteBuf(AHT_ADDR, (uint8 *)i2cWrBuf, 1, I2C_MODE_COMPLETE_XFER);
//        while(SDA0_Read());
//        while(!(I2C_MasterStatus() & I2C_MSTAT_WR_CMPLT));
//        I2C_MasterClearStatus();
//        CyDelay(10);
//        
//        I2C_MasterReadBuf(AHT_ADDR, (uint8 *)i2cRdBuf, 7, I2C_MODE_COMPLETE_XFER); // get status
//        while (!I2C_MSTAT_RD_CMPLT);
//        I2C_MasterClearStatus();
//        CyDelay(10);
//        
//        while(i2cRdBuf[0] & (1 << 7)); //check that bit 7 (busy) is low
//        
//        CyWdtClear();
//        
//        // convert readings to temp and humidity
//        humid = convertHumidity(i2cRdBuf[1], i2cRdBuf[2], i2cRdBuf[3]);
//        tempF = convertTempF(i2cRdBuf[5], i2cRdBuf[4], i2cRdBuf[3]);
        checkParam(tempF, humid);
        
        // print to OLED
        changeI2CDevice(0);
        printTempHumid(tempF, humid);
                
        //clear I2C buffer
        I2C_MasterClearReadBuf();
        memset(i2cRdBuf, 0, sizeof i2cRdBuf);

        CyWdtClear();

        // if connected to app, send temp and humidity information
        if(connection){
            
            // check if an alert has been triggered, send corresponding alarm code
            if(alertFlag){
                //check if notice has also been triggered
                if(noticeFlag){
                    sprintf(s,"ALERT %d NOTICE %d %.2f %.2f DATA", alertFlag, noticeFlag, tempF, humid);
                }
                else{
                    sprintf(s,"ALERT %d %.2f %.2f DATA", alertFlag, tempF, humid);
                }
                alertFlag = 0;
                noticeFlag = 0;
            }
            else if(noticeFlag){
                sprintf(s,"NOTICE %d %.2f %.2f DATA", noticeFlag, tempF, humid);
            }
            else{
                sprintf(s,"%.2f %.2f DATA", tempF, humid);
            }
            
            if(keyFlag){
                encryptESP(s, KEY, strlen(s));
            }
            CyWdtClear();
            CyDelay(100);
            CyWdtClear();

            // send 11 bytes of data
            sprintf(sESP, "AT+CIPSEND=0,%i\r\n\n", strlen(s));
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
        
        setTol();
        
        memset(sESP, '\0', 80);

    }
}
/* [] END OF FILE */
