/* ========================================
 *
 * ECE405 Pet Environment Monitoring System
 *              Fall 2022
 *
 * ========================================
*/
#include "project.h"
#include "ssd1306.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define AHT_ADDR (uint8)(0x38)
#define DISPLAY_ADDRESS (0x3C)

#define WIFI_SSID ""
#define WIfI_PWD ""

#define TIMEOUT_ESP 10000
#define ESP_CIRCBUF_LEN 64


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

// declare circular buffer data array and variable
uint8_t espStringData[ESP_CIRCBUF_LEN];
circBufESP espBuf = {
    .buffer = espStringData,
    .head = 0,
    .tail = 0,
    .length = ESP_CIRCBUF_LEN
};

int commandFlag = 0;

CY_ISR(uart_int_Handler){
    char c = UART_GetChar();
    UART_PutChar(c);
    UART_ClearRxBuffer();
}

CY_ISR(esp_int_Handler){
    char c = ESPUART_GetChar();
    
    circBufPush(&espBuf, c);
   
    ESPUART_ClearRxBuffer();
}

void initializeAHT();
void takeMeasurementAHT();
void restartAHT();
void printTempHumid(float temp, float humid);

void initESP();
void joinWifiESP(char ssid[], char pwd[]);
char* getStringESP(int Timeout);
int sendCommandESP(char command[], int Timeout);
int waitForResponseESP(char returnStr[], int Timeout);

float convertTempF(uint8 num1, uint8 num2, uint8 num3);
float convertHumidity(uint8 num1, uint8 num2, uint8 num3);

int connection = 0; // flag indicating whether a device is currently connected (0 for no connection, 1 for connected)

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    ESPUART_Start();
    ESPUART_ClearRxBuffer();
    ESPUART_ClearTxBuffer();
    UART_Start();
    rx_int_StartEx(uart_int_Handler);
    esprx_int_StartEx(esp_int_Handler);   
    I2C_Start();
    
    char s[80], sESP[80];
    uint8 i2cWrBuf[3], i2cRdBuf[7], cESP;
    float tempF, humidity;
    
    
    // initialize wifi settings and join network
    initESP();

    // restart and initialize temp/humid sensor
    restartAHT();
    I2C_MasterSendStop();
    I2C_MasterClearStatus();
    CyDelay(100); // wait 40ms after AHT power on
    initializeAHT();
    CyDelay(80); // wait 80ms for measurement to complete
    
    // initialize OLED
    // must stop sending and clear status to work with AHT
    display_init(DISPLAY_ADDRESS);
    I2C_MasterSendStop();
    I2C_MasterClearStatus();
    
    for(;;)
    {            
        // every loop check for connection, if none listen on port 54321
        if(connection == 0){
            ESPUART_PutString("AT+CIPSERVER=1,54321\r\n\n");
            if(!waitForResponseESP("Android\n", 5000)){
                connection = 1;   
            }
        }
        
        takeMeasurementAHT();   // measure temp and humid

        // read AHT measurement
        i2cWrBuf[0] = 0b01110001; // slave addr ; b[0] = 1 for read mode
        I2C_MasterWriteBuf(AHT_ADDR, (uint8 *)i2cWrBuf, 1, I2C_MODE_COMPLETE_XFER);
        while(SDA_Read());
        while(!(I2C_MasterStatus() & I2C_MSTAT_WR_CMPLT));
        I2C_MasterClearStatus();
        CyDelay(10);
        I2C_MasterReadBuf(AHT_ADDR, (uint8 *)i2cRdBuf, 7, I2C_MODE_COMPLETE_XFER); // get status
        while (!I2C_MSTAT_RD_CMPLT);
        I2C_MasterClearStatus();
        CyDelay(10);
        
        while(i2cRdBuf[0] & (1 << 7)); //check that bit 7 (busy) is low
        
        // convert readings to temp and humidity
        humidity = convertHumidity(i2cRdBuf[1], i2cRdBuf[2], i2cRdBuf[3]);
        tempF = convertTempF(i2cRdBuf[5], i2cRdBuf[4], i2cRdBuf[3]);
        
        // print to OLED
        printTempHumid(tempF, humidity);
                
        //clear I2C buffer
        I2C_MasterClearReadBuf();
        memset(i2cRdBuf, 0, sizeof i2cRdBuf);
        
        // check if anything in ESP buffer
        if(!circBufPop(&espBuf, &cESP)){
            // check if TCP connection has been closed
            if(!waitForResponseESP("CLOSED\r\n", 1000)){
                connection = 0;
            }
            
        }
        
        // if connected to app, send temp and humidity information
        if(connection){
            
            // send 11 bytes of data
            ESPUART_PutString("AT+CIPSEND=0,11\r\n\n");
            waitForResponseESP(">", 2000);
            
            // send to connected device
            sprintf(sESP,"%.2f %.2f", tempF, humidity);
            ESPUART_PutString(sESP);
            waitForResponseESP("OK\r\n", 1000);
            
            //close TCP connection
            ESPUART_PutString("AT+CIPSTATUS\r\n\n");
            if(!waitForResponseESP("STATUS:3\r\n", 1000)){
                //close TCP connection
                ESPUART_PutString("AT+CIPSERVER=0\r\n\n");
                waitForResponseESP("OK\r\n", 5000);
                
                connection = 0;
            }
            
        }

        CyDelay(250);
    }
}


/* ============== FUNCTIONS ============== */


void initializeAHT(){
    uint8 i2cWrBuf[4];
    uint8 i2cRdBuf[1];
    
    i2cWrBuf[0] = 0b01110001; // slave addr ; b[0] = 1 for read mode
    
    I2C_MasterWriteBuf(AHT_ADDR, (uint8 *)i2cWrBuf, 1, I2C_MODE_COMPLETE_XFER);
    while(!(I2C_MasterStatus() & I2C_MSTAT_WR_CMPLT));
    I2C_MasterClearStatus();
    CyDelay(10);
    
    I2C_MasterReadBuf(AHT_ADDR, i2cRdBuf, 1, I2C_MODE_COMPLETE_XFER);
    while(!I2C_MSTAT_RD_CMPLT);
    I2C_MasterClearStatus();
    CyDelay(10);
    
    i2cWrBuf[0] = 0b01110000; // slave addr ; b[0] = 0 for write mode
    
    // check if calibration command needed
    if((int)i2cRdBuf & (0 << 3)){
        i2cWrBuf[1] = 0xBE; // initialization cmd
        i2cWrBuf[2] = 0x08;
        i2cWrBuf[3] = 0x00;
        I2C_MasterWriteBuf(AHT_ADDR, (uint8 *)i2cWrBuf, 4, I2C_MODE_COMPLETE_XFER);
        while(!(I2C_MasterStatus() & I2C_MSTAT_WR_CMPLT));
        I2C_MasterClearStatus();
        
        CyDelay(10);
    }
    
    takeMeasurementAHT();
   
}

void takeMeasurementAHT(){
    uint8 i2cWrBuf[4];
    
    i2cWrBuf[0] = 0b01110000; // slave addr ; b[0] = 0 for write mode
    i2cWrBuf[1] = 0xAC; // start measurement cmd
    i2cWrBuf[2] = 0b00110011;
    i2cWrBuf[3] = 0b00000000;
    I2C_MasterWriteBuf(AHT_ADDR, (uint8 *)i2cWrBuf, 4, I2C_MODE_COMPLETE_XFER);
    while(!(I2C_MasterStatus() & I2C_MSTAT_WR_CMPLT));
    I2C_MasterClearStatus();
    
    //datasheet recommends 80 second delay for measurement to take place
    CyDelay(80);
}

void restartAHT(){
    uint8 i2cWrBuf[2];
    
    i2cWrBuf[0] = 0b01110000; // slave addr ; b[0] = 0 for write mode
    i2cWrBuf[1] = 0b10111010;
    I2C_MasterWriteBuf(AHT_ADDR, (uint8 *)i2cWrBuf, 2, I2C_MODE_COMPLETE_XFER);
    while(!(I2C_MasterStatus() & I2C_MSTAT_WR_CMPLT));
    I2C_MasterClearStatus();
    CyDelay(10);
}

void printTempHumid(float temp, float humid){
    
    char s[50];
    
    //print temp to uart
    sprintf(s, "Temperature: %.2f F\r\n", temp);
    
    //clear and set up OLED settings, print temp
    display_clear();    
    display_update();    
    gfx_setTextSize(1);
    gfx_setTextColor(WHITE);
    gfx_setCursor(2, 2);
    gfx_println(s);
    
    //print humidity to uart and OLED
    sprintf(s, "Humidity: %.2f \%% \r\n", humid);
    gfx_setCursor(2, 20);
    gfx_println(s);
    display_update(); 
    I2C_MasterSendStop();
    I2C_MasterClearStatus();
}

float convertTempF(uint8 num1, uint8 num2, uint8 num3){
    // combine 2 8 bit temperature data bytes and lower 4 bits of humidity/temperature byte
    int num16b = (num2 << 8) | num1;   
    int lowernum3 = num3 & 0x0F;
    int num20b = (lowernum3 << 16) | num16b;
    
    //convert to temp in C using formula in datasheet
    float tempC = (((double)num20b * 200.0)/pow(2.0, 20.0)) - 50.0;
    
    //convert to F
    float tempF = (tempC * 9/5) + 32.0;
    
    return tempF;
}


float convertHumidity(uint8 num1, uint8 num2, uint8 num3){
    // combine 2 8 bit humidity data bytes and upper 4 bits of humidity/temperature byte
    int num16b = (num1 << 8) | num2;
    int uppernum3 = num3 >> 4;
    int num20b = (num16b << 4) | uppernum3;
    
    //convert to humidity percentage using formula in datasheet
    float result = ((double)num20b/pow(2.0, 20.0)) * 100.0;
    
    return result;
}

void initESP(){
    char s[80];  
    char OK[] = "OK\r\n";
    
        ESPUART_PutString("AT+CSYSWDTDISABLE\r\n\n");
    waitForResponseESP(OK, 5000);
    
    ESPUART_PutString("AT+CWMODE=3\r\n\n");
    waitForResponseESP(OK, 5000);

    // set up UART settings
//    sprintf(s, "AT+UART=57600,8,1,0,0\r\n\n");
//    ESPUART_PutString(s);
//    CyDelay(1000);  
    
    // enable dhcp station mode
    ESPUART_PutString("AT+CWDHCP=1,1\r\n\n");
    waitForResponseESP(OK, 5000);
    
    joinWifiESP(WIFI_SSID, WIfI_PWD);

//  show device's current IP
//    ESPUART_PutString("AT+CIFSR\r\n\n");
//    CyDelay(1000);
//    waitForResponseESP("OK", 5000);
    
//    enable multiple connections
    ESPUART_PutString("AT+CIPMUX=1\r\n\n");
    waitForResponseESP(OK, 5000);
    
    CyDelay(1000);
    
}

void joinWifiESP(char ssid[], char pwd[]){
    char s[80];
    
    sprintf(s, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pwd);
    ESPUART_PutString(s);
    waitForResponseESP("OK\r\n", 10000);
    CyDelay(1000);
}

int waitForResponseESP(char returnStr[], int Timeout){
    uint8_t c;
    char s[80];
    char ERROR[] = "ERROR\r\n";
    int i = 0;
    int time = 0;
    
    memset(s, '\0', 80);
    while(strstr(s, returnStr) == NULL){
        while(circBufPop(&espBuf, &c) != 0){
            time++;
            CyDelay(1);
            
            if(time == Timeout){
            UART_PutString("Timed out waiting for response\r\n");
            return -1;
            }
        }
        s[i] = c;
        UART_PutChar(c);
        i++;
        time = 0;
        
        if(strstr(s, "ERROR\r\n") != NULL){
            return -1;   
        }
        // THIS DOESNT WORK - figure out why ESP sometimes infinitely prints "0, CONNECT FAIL" and stop the loop
        else if(strstr(s, "FAIL\r\n") != NULL){
            ESP_RST_Write(0);
            CyDelay(1000);
            ESP_RST_Write(1);
            CyDelay(1000);
            CySoftwareReset();
        }
        else if(strstr(s, "Android\r\n") != NULL){
            connection = 1;
            return -1;   
        }
        
    }
    
    return 0;
}

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
