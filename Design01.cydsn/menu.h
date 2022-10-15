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
#ifndef _MENU_H
#define _MENU_H

#include "cyapicallbacks.h"
#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"

extern volatile int SetTemp;
extern volatile int SetHumid;
extern volatile int TH, HH;
extern volatile int TL, HL;
extern volatile int tol;
extern volatile int tolh;
extern volatile float tempF, humid;
extern volatile int Select;
extern volatile int ENC_Flag;
extern volatile int SW1_Flag;
extern volatile int SW2_Flag;

void menu();
void printNew(int Select);

#endif
/* [] END OF FILE */
