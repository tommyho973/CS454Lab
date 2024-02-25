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

void __attribute__((__interrupt__)) _T1Interrupt(void){
    
    CLEARBIT(IFS0bits.T1IF);
    timer_expired = 1;
}

int main(void)
{	
	/* Q: What is my purpose? */
	/* A: You pass butter. */
	/* Q: Oh. My. God. */
    
    uint8_t data = 0;
    uint8_t start_byte=0;
    uint8_t data_counter = 0;
    uint8_t data_array[MSG_BYTES_MSG];
    
    uint8_t crc_bytes[2];
    uint16_t crc = 0;
    uint16_t calculated_crc = 0;
    
    uint8_t expected_length = 0;
    uint8_t actual_length = 0;
    
    int failed_sends = 0;
    uint8_t ACK= 0x1;
    uint8_t NACK = 0x0;
    
    char message[MSG_BYTES_MSG];
    
    uart2_init(BAUD_9600);
   
    set_timer1(32767);
    
    while(1) {
    //data first seen, fire interrupt 1 second after
        while(1) {

            if((uart2_recv(&start_byte)==0) && ( start_byte== 0x0)){
                break;
            }
            if(timer_expired == 1){
                CLEARBIT(IEC0bits.T1IE);
                printf("MESSED UP with first byte time");
                // Indicate time error            break;
                uart2_send_8(NACK);
            }
        }

        // Reading the received CRC
        if((uart2_recv(&crc_bytes[0]) != 0) || (uart2_recv(&crc_bytes[1]) != 0)){
            CLEARBIT(IEC0bits.T1IE);
            printf("MESSED UP with crc");
            uart2_send_8(NACK);
    //        return NACK;
        }
        crc = ((uint16_t)crc_bytes[0]) | ((uint16_t)(crc_bytes[1]) << 8);

        // Reading message body length
        if (uart2_recv(&expected_length) != 0){
            CLEARBIT(IEC0bits.T1IE);
            printf("MESSED UP with length");
            uart2_send_8(NACK);
    //        return NACK;
        }

        // Read the message itself
        while(actual_length < expected_length){
            if(uart2_recv(&data) == 0){
                data_array[data_counter] = data;
                data_counter++;
                calculated_crc = crc_update(calculated_crc, data);
            }

            if(timer_expired == 1){
                printf("MESSED UP with body message time");
                uart2_send_8(NACK);
    //            return NACK;
            }
        }

        // Stop timer we done
        CLEARBIT(IEC0bits.T1IE);

        if((crc == calculated_crc) && (expected_length == actual_length)){
            // Data chilling
            printf("YOU CHILLING");
            // print the failed on the lcd
            lcd_locate(0,0);
            lcd_clear_row(0);
            lcd_printf("Recv fail: %d", failed_sends);
            gotoLine(1);
            lcd_clear_row(1);
            lcd_printf("recv_buf:");
            lcd_printf("%d %x %x %d", start_byte, crc_bytes[0], crc_bytes[1], expected_length);
            gotoLine(2);
            lcd_clear_row(2);
            int i = 0;
            while(i < actual_length){
                lcd_printf("%d ", data_array[i]);
                char c = data_array[i];
                message[i] = c;
                i++;
            }
            gotoLine(3);
            lcd_printf("crc: %x crc: %x",calculated_crc, crc);
            gotoLine(4);
            lcd_printf("str: %s", message);
            uart2_send_8(ACK);
        }
        else{
            printf("YOU NOOOTTTTTTT CHILLING");
            failed_sends++;
            uart2_send_8(NACK);
        }
    }
    
    // Questions:
    // 1. How do we handle the NACK such that we have to resend the message with alot of NACKS or handle it differently and do we need this big while loop
    // 2. Edge case: how do we handle overflow buffer and what is the wrap around method.
    // 3. How do we run this together to test lol

    
    
    return 0;    
    
}	

