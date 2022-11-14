
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
#include "EEPROM_functions.h"
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
volatile long long KEY = 123;

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
volatile int equipFlag = 0;
volatile int mistFlag = 2;
volatile int alertClkFlag = 0;

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
    EEPROM_1_Start();
    esprx_int_StartEx(esp_int_Handler);
    I2C_Start();
    
    // set which i2c device to use
    changeI2CDevice(0);
    
    mistISR_Start();
    mistTimer_WriteCounter(0);
    mistTimer_WritePeriod(6000);     // 6000/(100Hz CLK) = 1 minute period before misting can begin
    mistTimer_Enable();
    
    alertTimeISR_Start();
    alertTimer_WriteCounter(0);
        
    char s[80], sESP[160], eepromS[30];
    float tempF, humid, tempF0, humid0, tempF1, humid1, tempF2, humid2;
    int lampOn, misterOn;

    // enable ESP
    ESP_RST_Write(1);
    
    // turn on green temp & humid status LEDs
    LED_T_G_Write(1);
    LED_H_G_Write(1);
    
    
    //** Manual Write EEPROM ** //     

//    uint8_t string2[11] = "belkin.640";
//    string2[10] = '\n';
//    
//    uint8_t string[9] = "f24f2829";
//    string[8] = '\n';
//    
//    uint8_t string3[1];
//    string3[0] = 78;                //Temp
//    
//    uint8_t string4[1];
//    string4[0] = 80;               //Humidity 
//    
//    uint8_t string5[1];
//    string5[0] = 2;           // tolT
//    uint8_t string6[1];
//    string6[0] = 2;// tolH
//
////    // write/read wifi ssid
//    writeEEPROM(WIFISSID_STARTADDR, string2, 12);
//    writeEEPROM(WIFIPWD_STARTADDR, string, 10);
//    writeEEPROM(SETTEMP_STARTADDR, string3, 1);
//    writeEEPROM(SETHUMID_STARTADDR, string4, 1);
//    writeEEPROM(TOLT_STARTADDR, string5, 1);
//    writeEEPROM(TOLH_STARTADDR, string6, 1);
    
//  //***************************************//  
//    
    
//  get intial values from EEPROM
    readEEPROM(WIFISSID_STARTADDR, eepromS, 30);
    wifi_ssid = strdup(strtok(eepromS, "\n"));
    
    readEEPROM(WIFIPWD_STARTADDR, eepromS, 30);
    wifi_pwd = strdup(strtok(eepromS, "\n"));
    
    readEEPROM(SETTEMP_STARTADDR, eepromS, 1);
    SetTemp = eepromS[0];
    
    readEEPROM(SETHUMID_STARTADDR, eepromS, 1);
    SetHumid = eepromS[0];
    
    readEEPROM(TOLT_STARTADDR, eepromS, 1);
    tolT = eepromS[0];
    
    readEEPROM(TOLH_STARTADDR, eepromS, 1);
    tolH = eepromS[0];

//    sprintf(s, "%d", SetTemp);
//    UART_PutString((char*)wifi_ssid);
//    UART_PutString((char*)wifi_pwd);
//    UART_PutString(s);
//    sprintf(s, "%d %d %d", SetHumid, tolT, tolH);
//    UART_PutString(s);
    
    wifi_ssid = "Welcome to SIUE";
    wifi_pwd = "";
    
    // turn off ESP echo
    if(DEBUG_MSGS){
        ESPUART_PutString("ATE1\r\n\n");
    }
    else{
        ESPUART_PutString("ATE0\r\n\n");
    }
    
    // initialize wifi settings and join network
    initESP(sESP);

    // restart and initialize all 3 temp/humid sensors
    for(int ii = 0; ii <= 2; ii++){
        changeI2CDevice(ii);
        restartAHT();
        initializeAHT();
        I2C_MasterSendStop();
        I2C_MasterClearStatus();
        CyDelay(100); // wait 40ms after AHT power on
    }
    changeI2CDevice(0);
    
    // initialize OLED
    // must stop sending and clear status to work with AHT
    display_init(DISPLAY_ADDRESS);
    I2C_MasterSendStop();
    I2C_MasterClearStatus();
    changeI2CDevice(0);
    
    // start WDT
    //CyWdtStart(CYWDT_1024_TICKS, CYWDT_LPMODE_NOCHANGE);
    CyWdtClear();

    SW1_ISR_Start();
    SW2_ISR_Start();
    ENC_ISR_Start();

    // update temp and humid tolerances
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
        
        // take temp and humid measurements from all three devices, average them
//        changeI2CDevice(0);
//        takeMeasurementAHT(&tempF0, &humid0);   // measure temp and humid
//        changeI2CDevice(1);
//        takeMeasurementAHT(&tempF1, &humid1);   // measure temp and humid
//        changeI2CDevice(2);
//        takeMeasurementAHT(&tempF2, &humid2);   // measure temp and humid
        
        tempF = (tempF0 + tempF1 + tempF2) / 3;
        humid = (humid0 + humid1 + humid2) / 3;
        
        if(0){
            sprintf(s, "%.2f %.2f\r\n%.2f %.2f\r\n%.2f %.2f\r\n", tempF0, humid0, tempF1, humid1, tempF2, humid2);
            UART_PutString(s);
        }
        
        // check that temp and humidity are within tolerance
        if(alertClkFlag != -1){
            checkParam(tempF, humid, 0);
            
            // check that the current sensing switches are showing the correct state
            checkISwitches(1);
        }
        else{
            checkParam(tempF, humid, 1);
            // check that the current sensing switches are showing the correct state
            checkISwitches(1);
            alertClkFlag = -2;
            
            alertTimer_Stop();
            alertTimer_WriteCounter(0);
            alertTimer_WritePeriod(6000);
            alertTimer_Enable();
        }
        
        // print to OLED
        changeI2CDevice(0);
        printTempHumid(tempF, humid);
                
        //clear I2C buffer
        I2C_MasterClearReadBuf();

        CyWdtClear();
       

        // if connected to app, send temp and humidity information as well as alert/notice flags
        if(connection){
            lampOn = I_SW_T_Read();
            misterOn = I_SW_H_Read();
            sendDataESP(sESP, tempF, humid, lampOn, misterOn);
            
            alertClkFlag = 0;
        }
        CyWdtClear();
        CyDelay(1000);
        CyWdtClear();
        
        setTol();
        
        // clear ESP UART string variable
        memset(sESP, '\0', 160);

    }
}
/* [] END OF FILE */
