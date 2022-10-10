/* ========================================
 *
 * ECE405 Pet Environment Monitoring System
 *              Fall 2022
 *
 * ========================================
*/
#include "project.h"
#include "ssd1306.c"
#include "I2C.c"
#include "aht.c"
#include "UART.c"
#include "handlers.c"
#include "font.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>



//#define WIFI_SSID "wifi_ssid"
//#define WIfI_PWD "wifi_pwd"
//
//#define TIMEOUT_ESP 10000

volatile int SetTemp = 70;
volatile int SetHumid = 50;
volatile int TH, HH;
volatile int TL, HL;
volatile int tol = 2;
volatile int tolh = 2;
//volatile float tempF, Humid;


//void initESP();
//void joinWifiESP(char ssid[], char pwd[]);
//char* getStringESP(int Timeout);
//int sendCommandESP(char command[], int Timeout);



int main(void){
    CyGlobalIntEnable; /* Enable global interrupts. */
    
//    ESPUART_Start();
//    ESPUART_ClearRxBuffer();
//    ESPUART_ClearTxBuffer();
    UART_Start();
    Update_Timer_Start();
    UPDATE_ISR_Start();
    I2C_Start();
    SW2_ISR_StartEx(sw2_int_Handler);
    ENC_ISR_StartEx(enc_int_Handler);
    //UPDATE_ISR_Start();
    TOUT_ISR_StartEx(tout_int_Handler);
    RX_ISR_StartEx(uart_int_Handler);
//    esp_isr_StartEx(esp_int_Handler);   
   // I2C_Start();
    
    
    //char s[80], sESP[80];
    uint8 i2cWrBuf[3];
    uint8 i2cRdBuf[7];
     
    float tempF, humidity;
    
//    int lengthESP = 10;
//    
//    // initialize wifi settings and join network
//    initESP();
//    
//    // listen on port 54321
//    ESPUART_PutString("AT+CIPSERVER=1,54321\r\n\n");
//    CyDelay(10000);
//
//    // send 6 bytes of data
//    ESPUART_PutString("AT+CIPSEND=0,6\r\n\n");
//    CyDelay(1000);
//    
//    // send to connected device
//    ESPUART_PutString("hiiiii");
//    CyDelay(1000);
//    
//    //close TCP connection
//    ESPUART_PutString("AT+CIPCLOSE=0\r\n\n");
//    CyDelay(1000);


    CyDelay(100); // wait 40ms after AHT power on
    initializeAHT();
    CyDelay(80); // wait 80ms for measurement to complete
    
    // initialize OLED
    // must stop sending and clear status to work with AHT
    display_init(DISPLAY_ADDRESS);
    I2C_MasterSendStop();
    I2C_MasterClearStatus();
    
    for(;;)
    {       
        // ***************** TEMP AND HUIMIDITY READINGS ***********************//
        takeMeasurementAHT();

        i2cWrBuf[0] = 0b01110001; // slave addr ; b[0] = 1 for read mode
        I2C_MasterWriteBuf(AHT_ADDR, (uint8 *)i2cWrBuf, 1, I2C_MODE_COMPLETE_XFER);
        while(SDA_1_Read());
        while(!(I2C_MasterStatus() & I2C_MSTAT_WR_CMPLT));
        I2C_MasterClearStatus();
        CyDelay(10);
       
        I2C_MasterReadBuf(AHT_ADDR, (uint8 *)i2cRdBuf, 7, I2C_MODE_COMPLETE_XFER); // get status
        while (!I2C_MSTAT_RD_CMPLT);
        I2C_MasterClearStatus();
        CyDelay(10);
        
        while(i2cRdBuf[0] & (1 << 7)); //check that bit 7 (busy) is low
        
        // convert readings to temp and humidity
        humidity = convertHumidity(i2cRdBuf[1], i2cRdBuf[2], i2cRdBuf[3]);
        tempF = convertTempF(i2cRdBuf[5], i2cRdBuf[4], i2cRdBuf[3]);
        
        // print to OLED
        printTempHumid(tempF, humidity);
                
        //clear I2C buffer
        I2C_MasterClearReadBuf();
        memset(i2cRdBuf, 0, sizeof i2cRdBuf);

        CyDelay(1000);
        
        //***********************************************************************//
    }
}


/* ============== FUNCTIONS ============== */



//void initESP(){
//    char s[80];
//    char c;
//    
//    ESPUART_PutString("AT+CWMODE=1\r\n\n");
//    UART_1_PutString(s);
//    CyDelay(1000);
//    
//    // set up UART settings
////    sprintf(s, "AT+UART=57600,8,1,0,0\r\n\n");
////    ESPUART_PutString(s);
////    CyDelay(1000);  
//    
//    // enable dhcp station mode
//    ESPUART_PutString("AT+CWDHCP=1,1\r\n\n");
//    CyDelay(1000);
//    
//    joinWifiESP(WIFI_SSID, WIfI_PWD);
//
////  show device's current IP
//    ESPUART_PutString("AT+CIFSR\r\n\n");
//    CyDelay(5000);
//    
////    enable multiple connections
//    ESPUART_PutString("AT+CIPMUX=1\r\n\n");
//    CyDelay(1000);
//    
//}
//
//void joinWifiESP(char ssid[], char pwd[]){
//    char s[80];
//    
//    sprintf(s, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pwd);
//    ESPUART_PutString(s);
//    CyDelay(5000);
//}


// incomplete functions that may need to be used in order to parse returned info from ESP e.g. current IP, whether a command executely correctly etc.
//char* getStringESP(int Timeout){
//    char s[80];
//    char* sESP = malloc(50);
//    memset(sESP, '\0', 50);
//    
//    int i = 0;
//    int t = 0;
//    
//    do{
//        // wait for next char from ESP
//        while(ESPUART_GetRxBufferSize() == 0){
//            
//            if(t == Timeout){
//                UART_1_PutString("getStringESP() Timed out");
//                sESP = "TIMEOUT";
//                return sESP;
//            }
//            
//            CyDelay(1);
//            t++;
//        }
//                
//        sESP[i] = ESPUART_GetChar();
//        
//        t = 0;
//        i++;
//        
//    }while(sESP[i-1] != '\n');
//    
//    return sESP;
//}

//int sendCommandESP(char command[], int Timeout){
//    volatile char e[6];
//    volatile char ok[3];
//    char* sESP = malloc(150);
//        
//    uint8 flag = 0, i = 0;
//    
//    sprintf((char *)e, "ERROR"); 
//    sprintf((char *)ok, "OK"); 
//    memset(sESP, '\0', 150);
//    
//    ESPUART_PutString(command);
//    
//    while(1){
//        sESP = getStringESP(Timeout);
//        UART_PutString(sESP);
//        
//        if(strstr(sESP, (char *)ok) != NULL){
//            return 0;
//        }
//        else if(strstr(sESP, (char *)e) != NULL){
//            return 1;   
//        }
//    }
//
//}
/* [] END OF FILE */
