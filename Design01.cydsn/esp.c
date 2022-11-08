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
#include "esp.h"
#include "circbuf.h"
#include "EEPROM_functions.h"
#include "aht.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define ESP_CIRCBUF_LEN 64  //esp.h


extern uint8_t espStringData[ESP_CIRCBUF_LEN];
extern circBufESP espBuf;

void initESP(char* sESP){
    char OK[] = "OK\r\n";
    
    closeConnectionESP(sESP);
    waitForResponseESP(OK, sESP, 1000);
    
    // enable ESP WDT
    ESPUART_PutString("AT+CSYSWDTENABLE\r\n\n");
    waitForResponseESP(OK, sESP, 5000);
    
    // set to station mode
    ESPUART_PutString("AT+CWMODE=1\r\n\n");
    waitForResponseESP(OK, sESP, 5000);

    // set up UART settings - only needs to be ran if first time using ESP module
//    sprintf(s, "AT+UART=57600,8,1,0,0\r\n\n");
//    ESPUART_PutString(s);
//    CyDelay(1000);  
    
    // enable dhcp station mode
    ESPUART_PutString("AT+CWDHCP=1,1\r\n\n");
    waitForResponseESP(OK, sESP, 5000);
    
    joinWifiESP((char *)wifi_ssid, (char *)wifi_pwd, sESP);

////  show device's current IP
    ESPUART_PutString("AT+CIFSR\r\n\n");
    CyDelay(1000);
    waitForResponseESP("OK", sESP, 5000);
    
//    enable multiple connections
    ESPUART_PutString("AT+CIPMUX=1\r\n\n");
    waitForResponseESP(OK, sESP, 5000);
    
}

void joinWifiESP(char *ssid, char *pwd, char* sESP){
    char s[80];
    
    // join wifi network, keep trying until successfully joined
    sprintf(s, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pwd);
    ESPUART_PutString(s);
    while(waitForResponseESP("OK\r\n", sESP, 10000)){
        sprintf(s, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pwd);
        ESPUART_PutString(s);
    }
    CyDelay(1000);
}

// function that will constant pop from circular buffer until desired response from ESP seen or timeout
int waitForResponseESP(char returnStr[], char* sESP, int Timeout){
    uint8_t c;
    uint8_t key8b = KEY & 0xFF;

    char str[80], s[30];
    
    int i = 0;
    int time = 0;
    
    memset(sESP, '\0', 80);
    memset(str, '\0', 80);
    
    // loop until desired returnStr seen
    while(strstr(sESP, returnStr) == NULL){
        // remove char from circbuf if there's a new one
        while(circBufPop(&espBuf, &c) != 0){
            time++;
            CyDelay(1);
            
            // if no new char after timeout period, return -1
            if(time == Timeout){
            //UART_PutString("Timed out waiting for response\r\n");
//            
//            if(timeoutCount == 10){
//                esprx_int_Disable();
//                ESP_RST_Write(0);
//                CyDelay(100);
//                ESP_RST_Write(1);
//                CyDelay(100);
//                CySoftwareReset();
//            }
            return -1;
            }
            CyWdtClear();
        }
        
        // add new char to ESP string variable
        sESP[i] = c;
        
        if(DEBUG_MSGS){
            UART_PutChar(c);
        }
        
        // if a key has been shared, decrypt 
        if(keyFlag){
            str[i] = c ^ key8b;   
        }
                
        i++;
        time = 0;

        // if android requests data, set connection flag to 1 if key shared, else ESP ask for key
        if(strstr(str, "REQUESTDATA") != NULL){
            if(keyFlag){
                connection = 1;
                memset(str, '\0', 80);
            }
            else{
                // ask android to run startup seq
                requestStartup(sESP);
            }
            return -1;   
        }
        // if set points have been updated in app look for DONE response from android
        else if(strstr(str, "SETPOINTS") != NULL){
            waitForResponseESP("DONE", sESP, 1000);
            return -1;   
        }
        // android will send "DONE" after sending set temp, set humid, temp tolerance and humid tolerance 
        else if(strstr(str, "DONE") != NULL){
            // get each set value and replace with space so next value can be extracted
            SetTemp = atoi(str);
            str[0] = ' '; str[1] = ' '; str[2] = ' ';
            SetHumid = atoi(str);
            str[3] = ' '; str[4] = ' '; str[5] = ' ';
            tolT = atoi(str);
            str[6] = ' '; str[7] = ' ';
            tolH = atoi(str);
            
            // write set points to memory
            changeSetPointsEEPROM((uint8_t) SetTemp, (uint8_t) SetHumid, (uint8_t) tolT, (uint8_t) tolH);
            
            // ACK and send new values back to ensure both sides have the same numbers
            sprintf(s, "ACK %d %d %d %d", SetTemp, SetHumid, tolT, tolH);
            encryptESP(s, KEY, 13);
                            
            ESPUART_PutString("AT+CIPSEND=0,13\r\n\n");
            waitForResponseESP(">", sESP, 5000);

            // send to connected device
            ESPUART_PutString(s);
            waitForResponseESP("OK\r\n", sESP, 1000);
            
            setTol();
                
            return 1;
        }
        else if(strstr(sESP, "STARTUP\r\n") != NULL){
            // start key sharing
            getEncryptStartupESP(sESP); 

            return -1;
        }
        else if(strstr(sESP, "ALREADY CONNECTED\r\n") != NULL){
            return 0;   
        }
        // if receive 11 bytes - "REQUESTDATA" - encrypted but no key, request key sharing startup
        else if(strstr(sESP, "IPD,0,11") != NULL && !keyFlag){
            requestStartup(sESP);   
        }
        CyWdtClear();
        
    }
    
    return 0;
}

//start the key exchange process with the app for encryption
void getEncryptStartupESP(char* sESP){
    char s[80];
    const char colon[2] = ":";

    // send ACK and start points
    ESPUART_PutString("AT+CIPSEND=0,13\r\n\n");
    waitForResponseESP(">", sESP, 5000);
    
    // send to connected device
    sprintf(s, "ACK %d %d %d %d", SetTemp, SetHumid, tolT, tolH);
    ESPUART_PutString(s);
    waitForResponseESP("OK\r\n", sESP, 1000);
    
    closeConnectionESP(sESP);
    initUDPConnectionESP(sESP);
    waitForResponseESP("OK\r\n", sESP, 1000);
    
    // generate private int
    PRIV = rand() % (9) + 1; // generate random number 1-9
    
    int A = (int)pow((double)BASE, (double)PRIV) % MOD;
    
    waitForResponseESP("ENDSTARTUP", sESP, 60000);
    char* _ = strtok(sESP, colon); 
    char* str2 = strtok(NULL, "");
    CyWdtClear();
    
    int B = atoi(str2);
    
    // send ACK and A
    sprintf(s, "%i ACK", A);
    int l = strlen(s);
    char str[80];
    sprintf(str, "AT+CIPSEND=0,%i\r\n\n", l);
    ESPUART_PutString(str);
    waitForResponseESP(">", sESP, 5000);
    
    // send to connected device
    ESPUART_PutString(s);
    waitForResponseESP("OK\r\n", sESP, 1000);
    CyWdtClear();
    
    KEY = (unsigned)pow((double)B, (double)PRIV) % MOD;
    sprintf(s, "%i\r\n", B);
    UART_PutString(s);
    sprintf(s, "%i\r\n", KEY);
    UART_PutString(s);
    if(KEY != 0 && KEY != 1){
        keyFlag = 1;
    }
    closeConnectionESP(sESP);

    CyWdtClear();
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
    waitForResponseESP("OK\r\n", sESP, 500);
    ESPUART_PutString("AT+CIPCLOSE=0\r\n\n");
    waitForResponseESP("OK\r\n", sESP, 500);
    CyWdtClear();
}

// encrypt sESP string using XOR
void encryptESP(char* s, unsigned key, int len){
    uint8_t key8b = key & 0xFF; // lower 8 bits of key

    for(int i = 0; i < len; i++){
        s[i] = s[i] ^ key8b;
        CyWdtClear();
    }
}

void requestStartup(char* sESP){
    ESPUART_PutString("AT+CIPSEND=0,10\r\n\n");
    waitForResponseESP(">", sESP, 2000);

    // send to connected device
    ESPUART_PutString("STARTUPSEQ");
    waitForResponseESP("OK\r\n", sESP, 1000);

    closeConnectionESP(sESP);
}

void sendDataESP(char* sESP, float tempF, float humid){
    char s[80];
    
    sprintf(s, "EQUIP %d ALERT %d NOTICE %d %.2f %.2f %d %d %d %d DATA", equipFlag, alertFlag, noticeFlag, tempF, humid, SetTemp, SetHumid, tolT, tolH);
    
    if(keyFlag){
        encryptESP(s, KEY, strlen(s));
    }
    
    CyWdtClear();
    CyDelay(100);
    CyWdtClear();
    
    // send data
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
    alertFlag = 0;
    noticeFlag = 0;
    equipFlag = 0;
    
}
/* [] END OF FILE */
