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
    
#define WIFI_SSID ""    //esp.h
#define WIfI_PWD ""    // esp.h 
    
void initESP(char* sESP);
void joinWifiESP(char ssid[], char pwd[], char* sESP);
void getEncryptStartupESP(char* sESP);
void initUDPConnectionESP(char* sESP);
void closeConnectionESP(char* sESP);
int waitForResponseESP(char returnStr[], char* sESP, int Timeout);
    
#endif
/* [] END OF FILE */
