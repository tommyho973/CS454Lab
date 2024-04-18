/****************************************************/
/*                                                  */
/*   CS-454/654 Embedded Systems Development        */
/*   Instructor: Renato Mancuso <rmancuso@bu.edu>   */
/*   Boston University                              */
/*                                                  */
/*   Description: lab5 timer initialization func.   */
/*                                                  */
/****************************************************/


#include "timer.h"

// PRESCALAR IS SET TO 1:64 BUT UNSURE IF THAT IS GOOD
void set_timer2(uint16_t period){
	/* Implement me please. */
    // Initialize and enable Timer2
        // Initialize Output Compare Module for the Y SERVO MOTOR
        CLEARBIT(T2CONbits.TON); // Disable Timer
        CLEARBIT(T2CONbits.TCS); // Select internal instruction cycle clock
        CLEARBIT(T2CONbits.TGATE); // Disable Gated Timer mode
        TMR2 = 0x00; // Clear timer register
        T2CONbits.TCKPS = 0b10; // Select 1:64 Prescaler
        CLEARBIT(IFS0bits.T2IF); // Clear Timer2 interrupt status flag
        CLEARBIT(IEC0bits.T2IE); // Disable Timer2 interrupt enable control bit
        PR2 = period; // Set timer period 20ms:
}

