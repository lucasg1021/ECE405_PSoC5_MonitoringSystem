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
#include "esp.h"
#include "ESPUART.h"
#include "UART.h"
#include "ESP_RST.h"
#include "circbuf.h"
#include "esprx_int.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define ESP_CIRCBUF_LEN 64  //esp.h

extern volatile int connection;
extern volatile int BASE, MOD, PRIV;
extern volatile unsigned KEY;
extern uint8_t espStringData[ESP_CIRCBUF_LEN];
extern circBufESP espBuf;

void initESP(char* sESP){
    char s[80];  
    char OK[] = "OK\r\n";
    
    ESPUART_PutString("AT+CSYSWDTDISABLE\r\n\n");
    waitForResponseESP(OK, sESP, 5000);
    
    ESPUART_PutString("AT+CWMODE=1\r\n\n");
    waitForResponseESP(OK, sESP, 5000);

    // set up UART settings
//    sprintf(s, "AT+UART=57600,8,1,0,0\r\n\n");
//    ESPUART_PutString(s);
//    CyDelay(1000);  
    
    // enable dhcp station mode
    ESPUART_PutString("AT+CWDHCP=1,1\r\n\n");
    waitForResponseESP(OK, sESP, 5000);
    
    joinWifiESP(WIFI_SSID, WIfI_PWD, sESP);

//  show device's current IP
//    ESPUART_PutString("AT+CIFSR\r\n\n");
//    CyDelay(1000);
//    waitForResponseESP("OK", 5000);
    
//    enable multiple connections
    ESPUART_PutString("AT+CIPMUX=1\r\n\n");
    waitForResponseESP(OK, sESP, 5000);
    
}

void joinWifiESP(char ssid[], char pwd[], char* sESP){
    char s[80];
    
    sprintf(s, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pwd);
    ESPUART_PutString(s);
    waitForResponseESP("OK\r\n", sESP, 10000);
    CyDelay(1000);
}

int waitForResponseESP(char returnStr[], char* sESP, int Timeout){
    uint8_t c;
    char ERROR[] = "ERROR\r\n";
    int i = 0;
    int time = 0;
    int time2 = 0;
    
    memset(sESP, '\0', 80);
    while(strstr(sESP, returnStr) == NULL){
        while(circBufPop(&espBuf, &c) != 0){
            time++;
            CyDelay(1);
            
            if(time == Timeout){
            UART_PutString("Timed out waiting for response\r\n");
            return -1;
            }
            CyWdtClear();
        }
        sESP[i] = c;
        UART_PutChar(c);
        i++;
        time = 0;
        
//        if(strstr(s, "ERROR\r\n") != NULL){
//            return -1;   
//        }

       if(strstr(sESP, "CONNECT FAIL\r\n") != NULL){
            esprx_int_Disable();
            ESP_RST_Write(0);
            CyDelay(100);
            ESP_RST_Write(1);
            CyDelay(100);
            CySoftwareReset();
        }
        else if(strstr(sESP, "Android\r\n") != NULL){
            connection = 1;
            return -1;   
        }
        else if(strstr(sESP, "STARTUP\r\n") != NULL){
            getEncryptStartupESP(sESP); 

            return -1;
        }
        if(strstr(sESP, "ALREADY CONNECTED\r\n") != NULL){
            return 0;   
        }
        CyWdtClear();
        
    }
    
    return 0;
}

//start the key exchange process with the app for encryption
void getEncryptStartupESP(char* sESP){
    char s[80], modS[2];
    char baseS;
    const char colon[2] = ":";
    char* token;
    int baseESP, modESP;
    
    token = strtok(sESP, colon); 
    char *str = strtok(NULL, "");
    CyWdtClear();
    modS[0] = str[0];
    modS[1] = str[1];
    baseS = str[3];
    CyWdtClear();
    
    modESP = atoi(modS);
    baseESP = baseS - '0';
    
    BASE = baseESP;
    MOD = modESP;
        
    // send ACK
    ESPUART_PutString("AT+CIPSEND=0,3\r\n\n");
    waitForResponseESP(">", sESP, 5000);
    
    // send to connected device
    ESPUART_PutString("ACK");
    waitForResponseESP("OK\r\n", sESP, 1000);
    
    closeConnectionESP(sESP);
    initUDPConnectionESP(sESP);
    waitForResponseESP("OK\r\n", sESP, 1000);
    
    // generate private int
    PRIV = rand() % (9) + 1; // generate random number 1-9
    
    int A = (int)pow((double)BASE, (double)PRIV) % (int)MOD;
    
    waitForResponseESP("ENDSTARTUP", sESP, 60000);
    token = strtok(sESP, colon); 
    char *str2 = strtok(NULL, "");
    CyWdtClear();
    
    int B = atoi(str2);
    
    // send ACK and A
    ESPUART_PutString("AT+CIPSEND=0,6\r\n\n");
    waitForResponseESP(">", sESP, 5000);
    
    // send to connected device
    sprintf(s, "%i ACK", A);
    ESPUART_PutString(s);
    waitForResponseESP("OK\r\n", sESP, 1000);
    
    KEY = (int)pow(B, PRIV) % (int)MOD;
    sprintf(s, "\r\n%i\r\n", KEY);
    UART_PutString(s);
    
    closeConnectionESP(sESP);
    initUDPConnectionESP(sESP);
    waitForResponseESP("OK\r\n", sESP, 1000);
}

void initUDPConnectionESP(char* sESP){
    ESPUART_PutString("AT+CIPSERVER=1,54321\r\n\n");
    waitForResponseESP("OK\r\n\n", sESP, 1000);
    CyWdtClear();
    ESPUART_PutString("AT+CIPSTART=0,\"UDP\",\"0.0.0.0\",54321,54321,2\r\n\n");
}


void closeConnectionESP(char* sESP){
    //close connection
    ESPUART_PutString("AT+CIPSERVER=0\r\n\n");
    waitForResponseESP("OK\r\n", sESP, 5000);
    ESPUART_PutString("AT+CIPCLOSE=0\r\n\n");
    waitForResponseESP("OK\r\n", sESP, 5000);
}
/* [] END OF FILE */
