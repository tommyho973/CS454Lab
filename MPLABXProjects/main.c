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


uint8_t start_byte=0;
uint8_t data_array[MSG_BYTES_MSG];
uint8_t message_length = 0;

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
    CLEARBIT(IFS0bits.T1IF);
    timer_expired = 1;
    
}

int main(void)
{	

	/* Q: What is my purpose? */
	/* A: You pass butter. */
	/* Q: Oh. My. God. */
    __C30_UART=1;
    lcd_initialize();
    uart2_init(BAUD_9600);
    lcd_clear();  
    while(1){
        timer_expired = 0; 
        calculated_crc = 0;
        failed_sends= 0;
        while(1){
            int received = uart2_recv(&start_byte);
            if(received == 0){
                if(start_byte == 0){
                    resetLCD(); 
                    set_timer1(32767);
                    break;
                }
            }
        }
       while(timer_expired != 1){
            int received = uart2_recv(&crc1);
            if(received == 0){
                if(crc1 != 0){
                    break;
                }
            }
        }
       while(timer_expired != 1){
            int received = uart2_recv(&crc2);
            if(received == 0){
                if(crc1 != 0){
                    break;
                    
                }
            }
        }
        
        while(timer_expired != 1){
            int received = uart2_recv(&message_length);
            if(received == 0){
                if(crc1 != 0){

                    break;
                    
                }
            }
        }
        crc = ((uint16_t)crc2 | ((uint16_t)crc1 << 8));
        int i = 0;
        while(timer_expired != 1){
            int received = uart2_recv(&data_array[i]);
            if (received == 0){
                if(data_array[i] != 0){
                    calculated_crc = crc_update(calculated_crc,data_array[i]);
                    i++;
                }
                if((crc == calculated_crc)&&(i == message_length)){
//                    lcd_printf("CRCs: %x %x", crc, calculated_crc);
                    CLEARBIT(T1CONbits.TON);
                    uart2_send_8(ACK);
                    break;
                }
                
                
            }
        }
        
        if (timer_expired == 1){
            CLEARBIT(T1CONbits.TON);
            uart2_send_8(NACK);    
        }
        clearLCD();
        lcd_locate(0,0);
        lcd_printf("Recv fail: %d\n\r", failed_sends);
        lcd_printf("recv_buf:");
        lcd_printf("%d %x %x %d ", start_byte, crc1, crc2, message_length);
        int k;
        for(k = 0; k < message_length; k++){
            lcd_printf("%x ", data_array[k]);
            char c = data_array[k];
            message[k] = c;
        }
        lcd_printf("\n\r");
        lcd_printf("crc: %x crc: %x\n\rstr:",calculated_crc, crc);
        for(k = 0; k < message_length; k++){
            lcd_printf("%c", message[k]);
        }
        uart2_send_8(ACK);

        
     
    }
    return 0;
}	