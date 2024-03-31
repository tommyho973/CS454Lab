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
        CLEARBIT(TRISDbits.TRISD6); /* Set OC7 as output */
        OC7R = 180; /* Set the initial duty cycle to 2.1ms calculated as timer tick duration(5micro seconds) * OC8R = 0.9ms*/
        OC7RS = 180; /* Load OCRS: next pwm duty cycle */
        OC7CON = 0x0006; /* Set OC8: PWM, no fault check, Timer2 */
        
    }
    else {  
        //setup OC8
        CLEARBIT(TRISDbits.TRISD7); /* Set OC8 as output */
        OC8R = 180; /* Set the initial duty cycle to 2.1ms calculated as timer tick duration(5micro seconds) * OC8R = 0.9 ms*/
        OC8RS = 180; /* Load OCRS: next pwm duty cycle */
        OC8CON = 0x0006; /* Set OC8: PWM, no fault check, Timer2 */
        
    }
    
        SETBIT(T2CONbits.TON); /* Turn Timer 2 on */
   
}

    void motor_set_duty(uint8_t chan, uint16_t duty_us) {
    if (chan == 7){
        OC7RS = (duty_us / 5);

    }
    else {
        OC8RS = (duty_us / 5);
    }
}
