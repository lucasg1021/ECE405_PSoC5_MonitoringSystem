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
int flag = 0;
int min = 0;
int max = 100;

  


void menu(){
    char s[50];
    CyWdtClear();
    display_clear(); 
    SW1_Flag = 0;
    SW2_Flag = 0;
    while(flag == 0){
        sprintf(s, "Press [L] for Temp \n [R] for Humidity");  
        display_update();    
        gfx_setTextSize(1);
        gfx_setTextColor(WHITE);
        gfx_setCursor(2, 40);
        gfx_println(s);
        CyWdtClear();
        if((SW1_Flag == 1) || (SW2_Flag == 1)){
            flag = 1;
            display_clear();
           
        }
    } 
    while(flag == 1){        // If a button is pressed
        
        if ((SW1_Flag == 1) && (SW2_Flag == 0)){     // If user selects Temperature settings
           if((ENC_Flag == 1) && (SetTemp < 100)){                       // Increase temperature
                SetTemp = SetTemp + 1;
                ENC_Flag = 0;
           
            }
            else if((ENC_Flag == -1) && (SetTemp > 0)){                   // Decrease temperature
                SetTemp = SetTemp - 1;
                ENC_Flag = 0;
               
            }
            Select = 1;
                if(Select == 1){
                            //print new temp to uart
                    sprintf(s, "Set Temperature: %d F\r\nPress Both Buttons to Confirm Changes", SetTemp);  
                    display_update();
                    gfx_setCursor(2, 20);
                    gfx_println(s);
                    //I2C_MasterSendStop();
                    //I2C_MasterClearStatus();
                }
               
        }
        else if((SW2_Flag == 1) && (SW1_Flag == 0)){    // If user selects Humidity settings
           if((ENC_Flag == 1) && (SetHumid < 100)){                          // Increase humidity
                SetHumid = SetHumid + 1;
                ENC_Flag = 0;
            }           
           else if((ENC_Flag == -1) && (SetHumid > 0)){             //Decrease humidity
                SetHumid = SetHumid - 1;
                ENC_Flag = 0;
            } 
            Select = 2;
            if(Select == 2){
                    sprintf(s, "Set Humidity: %d F\r\n Press Both Buttons to Confirm Changes", SetHumid);
                    display_update();
                    gfx_setCursor(2, 20);
                    gfx_println(s); 
                    //I2C_MasterSendStop();
                    //I2C_MasterClearStatus();
            }
        }
        if((SW1_Flag == 1) && (SW2_Flag == 1)){
            sprintf(s, "Changes Confirmed");
             display_clear();
             display_update();
             gfx_setCursor(10, 30);
             gfx_println(s);
             display_update(); 
             I2C_MasterSendStop();
             I2C_MasterClearStatus();
            
             SW1_Flag = 0;
             SW2_Flag = 0;
             ENC_Flag = 0;
             Select = 0;
             flag = 2;
        }
    }
        setTol();
        flag = 0;
        CyWdtClear();
    
}
/* [] END OF FILE */
// Can clear watchdogs here as well.

