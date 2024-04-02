/****************************************************/
/*                                                  */
/*   CS-454/654 Embedded Systems Development        */
/*   Instructor: Renato Mancuso <rmancuso@bu.edu>   */
/*   Boston University                              */
/*                                                  */
/*   Description: motor                             */
/*                                                  */
/****************************************************/


#include "motor.h"


// Perform any necessary initialization for the specified motor channel. 
// Use Timer2 for the duty cycle
void motor_init(uint8_t chan) {

    if (chan == 7){
        //setup OC7
        
        // Initialize Output Compare Module for the Y SERVO MOTOR
        CLEARBIT(T2CONbits.TON); // Disable Timer
        CLEARBIT(T2CONbits.TCS); // Select internal instruction cycle clock
        CLEARBIT(T2CONbits.TGATE); // Disable Gated Timer mode
        TMR2 = 0x00; // Clear timer register
        T2CONbits.TCKPS = 0b10; // Select 1:64 Prescaler
        CLEARBIT(IFS0bits.T2IF); // Clear Timer2 interrupt status flag
        CLEARBIT(IEC0bits.T2IE); // Disable Timer2 interrupt enable control bit
        PR2 = 4000; // Set timer period 20ms:
        
        CLEARBIT(TRISDbits.TRISD6); /* Set OC7 as output */
        OC7R = 3580; /* Set the initial duty cycle to 2.1ms calculated as timer tick duration(5micro seconds) * OC8R = 0.9ms*/
        OC7RS = 3580; /* Load OCRS: next pwm duty cycle */
        OC7CON = 0x0006; /* Set OC8: PWM, no fault check, Timer2 */
        
        SETBIT(T2CONbits.TON); /* Turn Timer 2 on */
        
    }
    else {  
        //setup OC8
        // Initialize Output Compare Module for the Y SERVO MOTOR
        CLEARBIT(T2CONbits.TON); // Disable Timer
        CLEARBIT(T2CONbits.TCS); // Select internal instruction cycle clock
        CLEARBIT(T2CONbits.TGATE); // Disable Gated Timer mode
        TMR2 = 0x00; // Clear timer register
        T2CONbits.TCKPS = 0b10; // Select 1:64 Prescaler
        CLEARBIT(IFS0bits.T2IF); // Clear Timer2 interrupt status flag
        CLEARBIT(IEC0bits.T2IE); // Disable Timer2 interrupt enable control bit
        PR2 = 4000; // Set timer period 20ms:
        
        CLEARBIT(TRISDbits.TRISD7); /* Set OC8 as output */
        OC8R = 4000; /* Set the initial duty cycle to 2.1ms calculated as timer tick duration(5micro seconds) * OC8R = 0.9 ms*/
        OC8RS = 4000; /* Load OCRS: next pwm duty cycle */
        OC8CON = 0x0006; /* Set OC8: PWM, no fault check, Timer2 */
        
        SETBIT(T2CONbits.TON); /* Turn Timer 2 on */ 
    }
    
        
   
}

    void motor_set_duty(uint8_t chan, uint16_t duty_us) {
    if (chan == 7){
        OC7RS = duty_us;

    }
    else {
        OC8RS = duty_us;
    }
}
