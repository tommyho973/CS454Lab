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
    //    uart2_send_8(NACK); // ASKKSKSKSKSKSKSKKSKKSKS
    CLEARBIT(IFS0bits.T1IF);
    timer_expired = 1;
    
}

int main(void)
{	
    __C30_UART=1;	
	lcd_initialize();
    
	lcd_clear();
	lcd_locate(0,0);
	lcd_printf("Hello World4!");	
	/* Do nothing */
	while(1){
		
	}
	/* Q: What is my purpose? */
	/* A: You pass butter. */
	/* Q: Oh. My. God. */
    
//    uint16_t message_length = MSG_MAX_LEN;
    
//    uart2_init(BAUD_9600);
//    
//    
//    while(1){
//        timer_expired = 0;
//        int i = 0;
    
//        data_array[i] = usart_getc();
    //    while(data_array[i]!= 0x00) {
    //        data_array[i] = usart_getc();
    //    }
    
//        set_timer1(32767);
//        while(timer_expired != 1){
//            i++;
//            data_array[i] = usart_getc();
//            if(i == 3){
//                message_length = data_array[3] + 4;
//            }
//            if(i >= message_length){
//                //TURN OFF TIMER HERE
//                CLEARBIT(T1CONbits.TON);           
//                break;
//            } 
//      
//    if (timer_expired == 1 ){
//        CLEARBIT(T1CONbits.TON);
//        // Do more to go back top
//    }
//        };
//    
//    crc1 = data_array[1];
//    crc2 = data_array[2];
//    int j;
//    crc = ((uint16_t)crc1 | ((uint16_t)crc2 << 8));
//    for(j = 4; j < message_length; j++){
//        calculated_crc = crc_update(calculated_crc,data_array[j]);
//    }
// 
//    if(crc == calculated_crc){
//        // Data chilling
//            printf("YOU CHILLING");
//            // print the failed on the lcd
//            lcd_locate(0,0);
//            lcd_clear_row(0);
//            lcd_printf("Recv fail: %d", failed_sends);
//            gotoLine(1);
//            lcd_clear_row(1);
//            lcd_printf("recv_buf:");
//            lcd_printf("%d %x %x %d", data_array[0], data_array[1], data_array[2], data_array[3]);
//            gotoLine(2);
//            lcd_clear_row(2);
//            int k;
//            for(k = 4; k < message_length; k++){
//                lcd_printf("%d ", data_array[k]);
//                char c = data_array[k];
//                message[k-4] = c;
//            }
//            gotoLine(3);
//            lcd_printf("crc: %x crc: %x",calculated_crc, crc);
//            gotoLine(4);
//            lcd_printf("str: %s", message);
//            resetLCD();
//            lcd_printf("hello");
//            uart2_send_8(ACK);
//    }
//    else{
//            printf("YOU NOOOTTTTTTT CHILLING");
//            failed_sends++;
//            uart2_send_8(NACK);
//            printf("Failed\n");
//    }
//    }
//    
    
    // Questions:
    // 1. How do we handle the NACK such that we have to resend the message with alot of NACKS or handle it differently and do we need this big while loop
    // 2. Edge case: how do we handle overflow buffer and what is the wrap around method.
    // 3. How do we run this together to test lol

    
    
    return 0;    
    
}	
