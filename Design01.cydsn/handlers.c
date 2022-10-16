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
#include "aht.h"

extern uint8 i2cBuffer[17];


CY_ISR(uart_int_Handler){
    char c = UART_GetChar();
    UART_PutChar(c);
    UART_ClearRxBuffer();
}

CY_ISR(enc_int_Handler){

    
}

CY_ISR(rx_int_Handler){
    
    
}

CY_ISR(sw2_int_Handler){
    
    
    
}


CY_ISR(tout_int_Handler){
//    if(tempF > TH){
//         //Turn off lamp
//          // Alarm high
//        }
//        else if(tempF < TL){
//            //Turn on lamp  
//            // Alarm low
//        }
//        if(humid > HH){
//            // Alarm High
//        }
//        else if(humid < HL){
//            //Turn on mister for X min
//            // Alarm low
//        }
//        else{
//         //Alarms OK   
//        }
//        Tout_ClearInterrupt();
    
}

//CY_ISR(esp_int_Handler){
//    char c = ESPUART_GetChar();
//    circBufPush(&espBuf, c);
//    ESPUART_ClearRxBuffer();
//}
/* [] END OF FILE */
