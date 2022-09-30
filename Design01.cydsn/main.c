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
#include <math.h>

#define AHT_ADDR (uint8)(0x38)
#define DISPLAY_ADDRESS (0x3C)

#define WIFI_SSID "wifi_ssid"
#define WIfI_PWD "wifi_pwd"

#define TIMEOUT_ESP 10000


CY_ISR(uart_int_Handler){
    char c = UART_GetChar();
    UART_PutChar(c);
    UART_ClearRxBuffer();
}

CY_ISR(esp_int_Handler){
    char c;
    char s[80];
    int i = 0;
    
    while((c = ESPUART_GetChar()) != '\0'){
        s[i] = c;
        i++;
        UART_PutChar(c);
    }

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

float convertTempF(uint8 num1, uint8 num2, uint8 num3);
float convertHumidity(uint8 num1, uint8 num2, uint8 num3);


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
    uint8 i2cWrBuf[3];
    uint8 i2cRdBuf[7];
     
    float tempF, humidity;
    
    int lengthESP = 10;
    
    // initialize wifi settings and join network
    initESP();
    
    // listen on port 54321
    ESPUART_PutString("AT+CIPSERVER=1,54321\r\n\n");
    CyDelay(10000);

    // send 6 bytes of data
    ESPUART_PutString("AT+CIPSEND=0,6\r\n\n");
    CyDelay(1000);
    
    // send to connected device
    ESPUART_PutString("hiiiii");
    CyDelay(1000);
    
    //close TCP connection
    ESPUART_PutString("AT+CIPCLOSE=0\r\n\n");
    CyDelay(1000);


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
        takeMeasurementAHT();

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

        CyDelay(1000);
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
    char c;
    
    ESPUART_PutString("AT+CWMODE=1\r\n\n");
    UART_PutString(s);
    CyDelay(1000);
    
    // set up UART settings
//    sprintf(s, "AT+UART=57600,8,1,0,0\r\n\n");
//    ESPUART_PutString(s);
//    CyDelay(1000);  
    
    // enable dhcp station mode
    ESPUART_PutString("AT+CWDHCP=1,1\r\n\n");
    CyDelay(1000);
    
    joinWifiESP(WIFI_SSID, WIfI_PWD);

//  show device's current IP
    ESPUART_PutString("AT+CIFSR\r\n\n");
    CyDelay(5000);
    
//    enable multiple connections
    ESPUART_PutString("AT+CIPMUX=1\r\n\n");
    CyDelay(1000);
    
}

void joinWifiESP(char ssid[], char pwd[]){
    char s[80];
    
    sprintf(s, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pwd);
    ESPUART_PutString(s);
    CyDelay(5000);
}


// incomplete functions that may need to be used in order to parse returned info from ESP e.g. current IP, whether a command executely correctly etc.
char* getStringESP(int Timeout){
    char s[80];
    char* sESP = malloc(50);
    memset(sESP, '\0', 50);
    
    int i = 0;
    int t = 0;
    
    do{
        // wait for next char from ESP
        while(ESPUART_GetRxBufferSize() == 0){
            
            if(t == Timeout){
                UART_PutString("getStringESP() Timed out");
                sESP = "TIMEOUT";
                return sESP;
            }
            
            CyDelay(1);
            t++;
        }
                
        sESP[i] = ESPUART_GetChar();
        
        t = 0;
        i++;
        
    }while(sESP[i-1] != '\n');
    
    return sESP;
}

int sendCommandESP(char command[], int Timeout){
    volatile char e[6];
    volatile char ok[3];
    char* sESP = malloc(150);
        
    uint8 flag = 0, i = 0;
    
    sprintf((char *)e, "ERROR"); 
    sprintf((char *)ok, "OK"); 
    memset(sESP, '\0', 150);
    
    ESPUART_PutString(command);
    
    while(1){
        sESP = getStringESP(Timeout);
        UART_PutString(sESP);
        
        if(strstr(sESP, (char *)ok) != NULL){
            return 0;
        }
        else if(strstr(sESP, (char *)e) != NULL){
            return 1;   
        }
    }

}
/* [] END OF FILE */
