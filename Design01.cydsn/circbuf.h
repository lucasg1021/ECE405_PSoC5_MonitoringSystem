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
#ifndef _CIRCBUF_H
#define _CIRCBUF_H

#include "cyapicallbacks.h"
#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"

// Circular buffer definition
// Buffer is used to store most recent strings returned from ESP
typedef struct{
    uint8_t * const buffer;
    int head;
    int tail;
    const int length;
} circBufESP;

int circBufPush(circBufESP *circBuf, uint8_t data);
int circBufPop(circBufESP *circBuf, uint8_t *data);

#endif
/* [] END OF FILE */
