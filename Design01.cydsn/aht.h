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
#ifndef _AHT_H
#define _AHT_H
    
#include "cyapicallbacks.h"
#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"

#define AHT_ADDR (uint8)(0x38)
#define DISPLAY_ADDRESS (0x3C)
    
extern volatile int alertFlag, noticeFlag, mistFlag;
extern volatile int TH, HH;
extern volatile int TL, HL;
extern volatile int tolT, tolH;
extern volatile int SetTemp, SetHumid;

void initializeAHT();
void takeMeasurementAHT();
void restartAHT();
void checkParam(float tempF, float humid);
void setTol();
void printTempHumid(float temp, float humid);
float convertTempF(uint8 num1, uint8 num2, uint8 num3);
float convertHumidity(uint8 num1, uint8 num2, uint8 num3);

#endif 
/* [] END OF FILE */
