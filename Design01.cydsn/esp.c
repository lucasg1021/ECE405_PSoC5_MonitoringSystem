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
#include "EEPROM_functions.h"
#include "aht.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define ESP_CIRCBUF_LEN 64  //esp.h

extern uint8_t espStringData[ESP_CIRCBUF_LEN];
extern circBufESP espBuf;

void initESP(char* sESP){
    char s[80];  
    char OK[] = "OK\r\n";
    
    closeConnectionESP(sESP);
    waitForResponseESP(OK, sESP, 1000);
    
    ESPUART_PutString("AT+CSYSWDTENABLE\r\n\n");
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
    
    joinWifiESP((char *)wifi_ssid, (char *)wifi_pwd, sESP);

////  show device's current IP
//    ESPUART_PutString("AT+CIFSR\r\n\n");
//    CyDelay(1000);
//    waitForResponseESP("OK", sESP, 5000);
    
//    enable multiple connections
    ESPUART_PutString("AT+CIPMUX=1\r\n\n");
    waitForResponseESP(OK, sESP, 5000);
    
}

void joinWifiESP(char *ssid, char *pwd, char* sESP){
    char s[80];
    
    sprintf(s, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pwd);
    ESPUART_PutString(s);
    while(waitForResponseESP("OK\r\n", sESP, 10000)){
        sprintf(s, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pwd);
        ESPUART_PutString(s);
    }
    CyDelay(1000);
}

int waitForResponseESP(char returnStr[], char* sESP, int Timeout){
    uint8_t c;
    uint8_t key8b = KEY & 0xFF;

    char str[80], s[30];
    
    int i = 0;
    int time = 0;
    int time2 = 0;
    
    memset(sESP, '\0', 80);
    memset(str, '\0', 80);
    while(strstr(sESP, returnStr) == NULL){
        while(circBufPop(&espBuf, &c) != 0){
            time++;
            CyDelay(1);
            
            if(time == Timeout){
            UART_PutString("Timed out waiting for response\r\n");
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
        
        sESP[i] = c;
        UART_PutChar(c);
        
        if(keyFlag){
            str[i] = c ^ key8b;   
        }
                
        i++;
        time = 0;

       if(strstr(sESP, "CONNECT FAIL\r\n") != NULL){
//            esprx_int_Disable();
//            ESP_RST_Write(0);
//            CyDelay(100);
//            ESP_RST_Write(1);
//            CyDelay(100);
//            CySoftwareReset();
        }
        else if(strstr(sESP, "REQUESTDATA") != NULL || strstr(str, "REQUESTDATA") != NULL){
            if(keyFlag){
                connection = 1;   
            }
            else{
                requestStartup(sESP);
            }
            return -1;   
        }
        else if(strstr(str, "SETPOINTS") != NULL){
            waitForResponseESP("DONE", sESP, 1000);
            return -1;   
        }
        else if(strstr(str, "DONE") != NULL){
                SetTemp = atoi(str);
                str[0] = ' '; str[1] = ' '; str[2] = ' ';
                SetHumid = atoi(str);
                str[3] = ' '; str[4] = ' '; str[5] = ' ';
                tolT = atoi(str);
                str[6] = ' '; str[7] = ' ';
                tolH = atoi(str);
                
                changeSetPointsEEPROM((uint8_t) SetTemp, (uint8_t) SetHumid, (uint8_t) tolT, (uint8_t) tolH);
                
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
            getEncryptStartupESP(sESP); 

            return -1;
        }
        else if(strstr(sESP, "ALREADY CONNECTED\r\n") != NULL){
            return 0;   
        }
        else if(strstr(sESP, "IPD,0,11") != NULL && !keyFlag){
            requestStartup(sESP);   
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
    
//    token = strtok(sESP, colon); 
//    char *str = strtok(NULL, "");
//    CyWdtClear();
//    modS[0] = str[0];
//    modS[1] = str[1];
//    baseS = str[3];
//    CyWdtClear();
//    
//    modESP = atoi(modS);
//    baseESP = baseS - '0';
//    
//    BASE = baseESP;
//    MOD = modESP;
        
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
    token = strtok(sESP, colon); 
    char *str2 = strtok(NULL, "");
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

void changeSetPointsESP(char* sESP){
    char s[30];
    waitForResponseESP("DONE", sESP, 1000);
    
    SetTemp = atoi(sESP);
    sESP += 2;
    SetHumid = atoi(sESP);
    
    sprintf(s, "%d\r\n%d", SetTemp, SetHumid);
    UART_PutString(s);
    
    CyDelay(10000);
    
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
