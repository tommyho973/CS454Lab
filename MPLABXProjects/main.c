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

void __attribute__((__interrupt__)) _T1Interrupt(void){
    
    CLEARBIT(IFS0bits.T1IF);
}

int main(void)
{	
	/* Q: What is my purpose? */
	/* A: You pass butter. */
	/* Q: Oh. My. God. */
    uint8_t data = 0;
    uint16_t crc = 0;
    int counter = 0;
    uart2_init(BAUD_9600);
    
    
    //data first seen, fire interrupt 1 second after
    if(uart2_recv(&data)==0){
        set_timer1(32767);
    }

    // Convert the decimal data value to its binary byte version form
    
    
    // Repeat until all parts of data bytes of the message to compute the CRC of the entire message body
    // Loop for each byte and do crc = crc_update(crc, data[i]) where i is incremented every for loop
    while()
    
    // Stop the timer once while loop finishes, while loop indicates that the message has been fully read
    
    
    
    
    
    
}	

