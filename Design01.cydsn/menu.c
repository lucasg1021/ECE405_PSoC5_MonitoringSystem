
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
#include "EEPROM_functions.h"

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
        gfx_setCursor(2, 20);
        gfx_println(s);
        CyWdtClear();
        if((SW1_Flag == 1) || (SW2_Flag == 1)){
            flag = 1;
            display_clear();
           
        }
    } 

    if ((SW1_Flag == 1) && (SW2_Flag == 0)){     // If user selects Temperature settings
        SW1_Flag = 0; SW2_Flag = 0;
        
        while(flag == 1){
            sprintf(s, "Press [L] for tol. \n [R] for set points");  
            display_update();    
            gfx_setTextSize(1);
            gfx_setTextColor(WHITE);
            gfx_setCursor(2, 20);
            gfx_println(s);
            CyWdtClear();
        
            if(SW1_Flag == 1 && SW2_Flag == 0){
                flag = 2;
                display_clear();
            }
            else if(SW2_Flag == 1 && SW1_Flag == 0){
                flag = 3;
                display_clear();
            }
        
        }
        
        SW1_Flag = 0;
        SW2_Flag = 0;
        // Change tolerance
        while(flag == 2){
            //print new temp to uart
            sprintf(s, "Temperature tol.: %d F\r\nPress Both Buttons to Confirm Changes", tolT);  
            display_update();
            gfx_setCursor(2, 20);
            gfx_println(s);
            I2C_MasterSendStop();
            I2C_MasterClearStatus();
            
            if(ENC_Flag == 1){
                tolT++;
                ENC_Flag = 0;
                display_clear();
            }
            else if(ENC_Flag == -1){
                tolT--;
                ENC_Flag = 0;
                display_clear();
            }
            else if(SW1_Flag && SW2_Flag){
                sprintf(s, "Changes Confirmed");
                display_clear();
                display_update();
                gfx_setCursor(10, 30);
                gfx_println(s);
                display_update(); 
                I2C_MasterSendStop();
                I2C_MasterClearStatus();
                
                flag = 0;
            }

        }
        
        // Change set point
        while(flag == 3){
            //print new temp to uart
            sprintf(s, "Set Temp.: %d F\r\nPress Both Buttons to Confirm Changes", SetTemp);  
            display_update();
            gfx_setCursor(2, 20);
            gfx_println(s);
            I2C_MasterSendStop();
            I2C_MasterClearStatus();
            
            if(ENC_Flag == 1){
                SetTemp++;
                ENC_Flag = 0;
                display_clear();
            }
            else if(ENC_Flag == -1){
                SetTemp--;
                ENC_Flag = 0;
                display_clear();
            }
            else if(SW1_Flag && SW2_Flag){
                sprintf(s, "Changes Confirmed");
                display_clear();
                display_update();
                gfx_setCursor(10, 30);
                gfx_println(s);
                display_update(); 
                I2C_MasterSendStop();
                I2C_MasterClearStatus();
                
                flag = 0;
            }

        }
           
    }
    else if((SW2_Flag == 1) && (SW1_Flag == 0)){    // If user selects Humidity settings
        SW1_Flag = 0; SW2_Flag = 0;
        
        while(flag == 1){
            sprintf(s, "Press [L] for tol. \n [R] for set points");  
            display_update();    
            gfx_setTextSize(1);
            gfx_setTextColor(WHITE);
            gfx_setCursor(2, 20);
            gfx_println(s);
            CyWdtClear();
        
            if(SW1_Flag == 1 && SW2_Flag == 0){
                flag = 2;
                display_clear();
            }
            else if(SW2_Flag == 1 && SW1_Flag == 0){
                flag = 3;
                display_clear();
            }
        
        }
        
        SW1_Flag = 0;
        SW2_Flag = 0;
        // Change tolerance
        while(flag == 2){
            //print new temp to uart
            sprintf(s, "Humidity tol.: %d F\r\nPress Both Buttons to Confirm Changes", tolH);  
            display_update();
            gfx_setCursor(2, 20);
            gfx_println(s);
            I2C_MasterSendStop();
            I2C_MasterClearStatus();
            
            if(ENC_Flag == 1){
                tolH++;
                ENC_Flag = 0;
                display_clear();
            }
            else if(ENC_Flag == -1){
                tolH--;
                ENC_Flag = 0;
                display_clear();
            }
            else if(SW1_Flag && SW2_Flag){
                sprintf(s, "Changes Confirmed");
                display_clear();
                display_update();
                gfx_setCursor(10, 30);
                gfx_println(s);
                display_update(); 
                I2C_MasterSendStop();
                I2C_MasterClearStatus();
                
                flag = 0;
            }

        }
        
        // Change set point
        while(flag == 3){
            //print new temp to uart
            sprintf(s, "Set Humidity: %d F\r\nPress Both Buttons to Confirm Changes", SetHumid);  
            display_update();
            gfx_setCursor(2, 20);
            gfx_println(s);
            I2C_MasterSendStop();
            I2C_MasterClearStatus();
            
            if(ENC_Flag == 1){
                SetHumid++;
                ENC_Flag = 0;
                display_clear();
            }
            else if(ENC_Flag == -1){
                SetHumid--;
                ENC_Flag = 0;
                display_clear();
            }
            else if(SW1_Flag && SW2_Flag){
                sprintf(s, "Changes Confirmed");
                display_clear();
                display_update();
                gfx_setCursor(10, 30);
                gfx_println(s);
                display_update(); 
                I2C_MasterSendStop();
                I2C_MasterClearStatus();
                
                flag = 0;
            }

        }
           
    }

    // write to EEPROM
    changeSetPointsEEPROM(SetTemp, SetHumid, tolT, tolH);
    setTol();
    flag = 0;
    SW1_Flag = 0;
    SW2_Flag = 0;
    ENC_Flag = 0;
    CyWdtClear();
    
}
/* [] END OF FILE */
// Can clear watchdogs here as well.
