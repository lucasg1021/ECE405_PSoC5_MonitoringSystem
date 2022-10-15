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
#include "menu.h"
#include "aht.h"
#include "I2C.h"
#include "ssd1306.h"
#include "Tout.h"
#include "LED_T_Y.h"
#include "LED_H_Y.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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

void menu(){
               // If button is pressed
        if ((SW1_Flag == 1) & (SW2_Flag != 1)){                        // If user selects Temperature settings
           if(ENC_Flag == 1){                     // Increase temperature
                SetTemp = SetTemp + 1;
            }
            else if(ENC_Flag == -1){              // Decrease temperature
                SetTemp = SetTemp - 1;
            }
            Select = 1;
                printNew(Select);                      // Print to screen
        }
        else if((SW2_Flag == 1) & (SW1_Flag != 1)){                  // If user selects Humidity settings
           if(ENC_Flag == 1){                    // Increase humidity
                SetHumid = SetHumid + 1;
            }           
           else if(ENC_Flag == -1){             //Decrease humidity
                SetHumid = SetHumid - 1;
            } 
            Select = 2;
            printNew(Select);
        }
        setTol();
        CyWdtClear();
    }

void printNew(int Select){
    CyWdtClear();
    char s[50];
    
    if(Select == 1){
        //print new temp to uart
        sprintf(s, "Set Temperature: %d F\r\nPress and Hold Buttons to Confirm Changes", SetTemp);
    
        //clear and set up OLED settings, print temp
        //display_clear();    
        display_update();    
        gfx_setTextSize(1);
        gfx_setTextColor(WHITE);
        gfx_setCursor(6, 6);
        gfx_println(s);
    }
    else if(Select == 2){
        //print temp to uart
    sprintf(s, "Set Humidity: %d F\r\n Press and Hold Buttons to Confirm Changes", SetHumid);
    
    //clear and set up OLED settings, print temp
    //display_clear();    
    display_update();    
    gfx_setTextSize(1);
    gfx_setTextColor(WHITE);
    gfx_setCursor(6, 6);
    gfx_println(s);
    } 
    if((SW1_Flag == 1) & (SW2_Flag == 1)){
            sprintf(s, "Changes Confirmed");
    
             //clear and set up OLED settings, print temp
            //display_clear();    
            display_update();    
            gfx_setTextSize(1);
            gfx_setTextColor(WHITE);
            gfx_setCursor(6, 6);
            gfx_println(s);
            SW1_Flag = 0;
            SW2_Flag = 0;
    }
    else{
        //print standard to uart
        sprintf(s, "Menu");
    
        //clear and set up OLED settings, print temp
        //display_clear();    
        display_update();    
        gfx_setTextSize(1);
        gfx_setTextColor(WHITE);
        gfx_setCursor(6, 6);
        gfx_println(s);
    }
}
/* [] END OF FILE */
// Can clear watchdogs here as well.

