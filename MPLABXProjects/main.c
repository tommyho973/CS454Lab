//do not change the order of the following 3 definitions
#define FCY 12800000UL 
#include <stdio.h>
#include <libpic30.h>

#include <p33Fxxxx.h>
#include "types.h"
#include "uart.h"
#include "crc16.h"
#include "lab3.h"
#include "lcd.h"
#include "timer.h"

// Primary (XT, HS, EC) Oscillator without PLL
_FOSCSEL(FNOSC_PRIPLL);
// OSC2 Pin Function: OSC2 is Clock Output - Primary Oscillator Mode: XT Crystanl
_FOSC(OSCIOFNC_ON & POSCMD_XT);
// Watchdog Timer Enabled/disabled by user software
_FWDT(FWDTEN_OFF);
// Disable Code Protection
_FGS(GCP_OFF);   

int timer_expired = 0;

uint8_t bufferArray[MSG_MAX_LEN];

uint8_t start_byte=0;
uint8_t data_array[MSG_BYTES_MSG];

uint16_t crc = 0;
uint8_t crc1 = 0;
uint8_t crc2 = 0;
uint16_t calculated_crc = 0;


int failed_sends = 0;
uint8_t ACK= 0x1;
uint8_t NACK = 0x0;
    
char message[MSG_BYTES_MSG];


void __attribute__((__interrupt__,no_auto_psv)) _T1Interrupt(void){
    failed_sends++;
    //    uart2_send_8(NACK);
    CLEARBIT(IFS0bits.T1IF);
    timer_expired = 1;
    
}

int main(void)
{	
    __C30_UART=1;	
	lcd_initialize();char 
	lcd_clear();
	lcd_locate(0,0);

	/* Q: What is my purpose? */
	/* A: You pass butter. */
	/* Q: Oh. My. God. */
    
    uint16_t message_length = MSG_MAX_LEN;
   
   int i = 0;
   uart2_init(BAUD_9600);

   while(1){
       timer_expired = 0;
       while(uart2_recv(&bufferArray[i]) != 0);
       if (bufferArray[0] != 0x00){
           lcd_printf("YOO");
           uart2_send_8(NACK);
       }
       set_timer1(32767);
       i++;
       while(1){
           while(uart2_recv(&bufferArray[i]) != 0);
           if(i == 3){
               message_length = bufferArray[3] + 4;
           }
           
           if(i >= message_length){
               //TURN OFF TIMER HERE
               CLEARBIT(T1CONbits.TON);           
               break;
           } 
     
            if (timer_expired == 1 ){
                CLEARBIT(T1CONbits.TON);
                continue;
                // Do more to go back top
            }
            i++;
       };
       
  
   
   crc1 = bufferArray[1];
   crc2 = bufferArray[2];
   int j;
   crc = ((uint16_t)crc1 | ((uint16_t)crc2 << 8));
   
   for(j = 4; j < message_length; j++){
       calculated_crc = crc_update(calculated_crc,bufferArray[j]);
   }
   
   __C30_UART=1;	
   lcd_initialize();
   if(crc == calculated_crc){
           // print the failed on the lcd
           lcd_clear();
           lcd_locate(0,0);
           lcd_clear_row(0);
           lcd_printf("Recv fail: %d", failed_sends);
           gotoLine(1);
           lcd_clear_row(1);
           lcd_printf("recv_buf:");
           lcd_printf("%d %x %x %d", bufferArray[0], bufferArray[1], bufferArray[2], bufferArray[3]);
           gotoLine(2);
           lcd_clear_row(2);
           int k;
           for(k = 4; k < message_length; k++){
               lcd_printf("%d ", bufferArray[k]);
               char c = bufferArray[k];
               message[k-4] = c;
           }
           gotoLine(3);
           lcd_printf("crc: %x crc: %x",calculated_crc, crc);
           gotoLine(4);
           lcd_printf("str: %s", message);
           resetLCD();
           uart2_send_8(ACK);
        }
   else{
           failed_sends++;
           uart2_send_8(NACK);
        }
   }    
    return 0;    
    
}	