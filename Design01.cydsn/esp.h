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
#ifndef _ESP_H
#define _ESP_H
    
#include "cyapicallbacks.h"
#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
    
#define DEBUG_MSGS 1    // indicate whether to show debugging messages in terminal

extern volatile int connection, keyFlag, BASE, PRIV;
extern volatile long long MOD;
extern volatile unsigned KEY;
extern volatile char * wifi_ssid;
extern volatile char * wifi_pwd;

void initESP(char* sESP);
void joinWifiESP(char ssid[], char pwd[], char* sESP);
void getEncryptStartupESP(char* sESP);
void initUDPConnectionESP(char* sESP);
void closeConnectionESP(char* sESP);
void encryptESP(char* s, unsigned key, int len);
void requestStartup(char* sESP);
void sendDataESP(char* sESP, float temp, float humid);

int waitForResponseESP(char returnStr[], char* sESP, int Timeout);

uint8_t crc8(char* s, unsigned key, int len);
    
#endif
/* [] END OF FILE */
