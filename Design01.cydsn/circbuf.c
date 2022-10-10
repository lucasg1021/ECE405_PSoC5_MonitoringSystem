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
#include "circbuf.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define ESP_CIRCBUF_LEN 64  //esp.h

int circBufPush(circBufESP *circBuf, uint8_t data){
    int next;
    
    next = circBuf->head + 1;
    
    //check if length exceeded
    if(next >= circBuf->length){
        //next location is 0
        next = 0;   
    }
    
    //check if buffer full
    if(next == circBuf->tail){
        return -1;   
    }
    
    // load data into current location and increment location
    circBuf->buffer[circBuf->head] = data;
    circBuf->head = next;
    
    return 0;
}

int circBufPop(circBufESP *circBuf, uint8_t *data){
    int next;
    
    // if head == tail, buffer empty
    if(circBuf->head == circBuf->tail){
        return -1;
    }
    
    next = circBuf->tail + 1;
    
    if(next >= circBuf->length){
        next = 0;
    }
    
    *data = circBuf->buffer[circBuf->tail];
    circBuf->tail = next;
    return 0;
}
/* [] END OF FILE */
