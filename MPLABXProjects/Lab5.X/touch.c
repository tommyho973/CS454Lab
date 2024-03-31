/****************************************************/
/*                                                  */
/*   CS-454/654 Embedded Systems Development        */
/*   Instructor: Renato Mancuso <rmancuso@bu.edu>   */
/*   Boston University                              */
/*                                                  */
/*   Description: lab 5  */
/*                                                  */
/****************************************************/

#include <p33FJ256MC710.h>

#include "touch.h"

void touch_init(void) {
    // ADC config
    //disable ADC
    CLEARBIT(AD1CON1bits.ADON); //Possibly need a second adc
    
    //initialize PIN
    SETBIT(TRISBbits.TRISB4); // JOYSTICK X
    SETBIT(TRISBbits.TRISB5); // JOYSTICK Y
    
    CLEARBIT(AD1PCFGLbits.PCFG4); //Sets the pin for joystick x analog to AN4
    CLEARBIT(AD1PCFGLbits.PCFG5); //Sets the pin for joystick x analog to AN5
    
    CLEARBIT(AD1CON1bits.AD12B); //set 10b Operation Mode
    AD1CON1bits.FORM = 0; //set integer output
    AD1CON1bits.SSRC = 0x7; //set automatic conversion
    
    //Configure AD1CON2
    AD1CON2 = 0; //not using scanning sampling
    //Configure AD1CON3
    CLEARBIT(AD1CON3bits.ADRC); //internal clock source
    AD1CON3bits.SAMC = 0x1F; //sample-to-conversion clock = 31Tad
    AD1CON3bits.ADCS = 0x2; //Tad = 3Tcy (Time cycles)
    //Leave AD1CON4 at its default value
    
    //enable ADC
    SETBIT(AD1CON1bits.ADON);
}

void touch_select_dim(uint8_t dimension) {
    if (dimension == 0) { // 0 meaning x dimension
        AD1CHS0bits.CH0SA = 0x004; // This should set it to AN4
    }
    else {
        AD1CHS0bits.CH0SA = 0x005; // Should set it to AN5
    }
}

uint16_t touch_read(void) {
    // ADC read
    SETBIT(AD1CON1bits.SAMP); //start to sample
    while(!AD1CON1bits.DONE); //wait for conversion to finish
    CLEARBIT(AD1CON1bits.DONE); //MUST HAVE! clear conversion done bit
    return ADC1BUF0;
}