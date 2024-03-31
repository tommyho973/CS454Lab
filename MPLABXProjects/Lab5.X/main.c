#include <p33Fxxxx.h>
//do not change the order of the following 3 definitions
#define FCY 12800000UL 
#include <stdio.h>
#include <libpic30.h>
#include <uart.h>

#include "lcd.h"
#include "types.h"
#include "motor.h"
#include "touch.h"
#include "led.h"
#include "timer.h"
#include "joystick.h"

/* Initial configuration by EE */
// Primary (XT, HS, EC) Oscillator with PLL
_FOSCSEL(FNOSC_PRIPLL);

// OSC2 Pin Function: OSC2 is Clock Output - Primary Oscillator Mode: XT Crystal
_FOSC(OSCIOFNC_OFF & POSCMD_XT); 

// Watchdog Timer Enabled/disabled by user software
_FWDT(FWDTEN_OFF);

// Disable Code Protection
_FGS(GCP_OFF); 


uint8_t chan = 7;
uint16_t duty_cycle = 0;
uint16_t xmax = 0;
uint16_t xmin = 0;
uint16_t ymax = 0;
uint16_t ymin = 0;

uint16_t curr = 0;

/* FIXXXX*/
void __attribute__((__interrupt__)) _T2Interrupt( void )
{
    /* Interrupt Service Routine code goes here */
    motor_set_duty(chan, duty_cycle); // Write Duty Cycle value for next PWM cycle
    IFS0bits.T2IF = 0; // Clear Timer 2 interrupt flag
}

int main(void)
{	
	__C30_UART=1;	
	lcd_initialize();
	lcd_clear();
	lcd_locate(0,0);
    
    lcd_printf("--- Lab 05 ---");
    gotoLine(2);
    
    initialize_buttons();
    
    
    touch_init(); // starts ADC for joystick
    set_timer2(4000);
//    motor_init(7);
//    motor_init(8);
    
    // Code for the trigger buttons debouncing
    initialize_buttons(); // for triggers
    SETBIT(B1TRIS); // fire button
//    SETBIT(B2TRIS);
    uint8_t prev = 1;
    
    
    touch_select_dim(0); //Select x axis
    while(1) { //Loop for X MAX
        // Debouncing 
        uint8_t v1 = B1PORT;
        __delay_ms(10);
        uint8_t v2 = B1PORT;
        __delay_ms(10); //Change back to 50 if need be
        
// SO IT READS THE VALUES CORRECTLY BUT THE PROBLEM IS SOMETHING RELATED TO HOW WE ARE SAMPLING
        lcd_clear_row(2);
        lcd_clear_row(3);
        gotoLine(2);
        curr = touch_read();
        lcd_printf("X-max?: %d", curr);

        if(BTN1_PRESSED()&&(v2 <.5)&& (v2 != prev)){ //If you know that you have pressed the button, then you can take a snapshot of the x axis max 
            prev = v2;
            lcd_clear_row(2);
            lcd_clear_row(3);
            lcd_printf("X-max: %d", curr);
            break;
        }   
        if(BTN1_RELEASED()){
            prev = 1;

        }
    }

    while(1){ // X -MIN LOOP
        
    }
    
    while(1) { // Y - MAX LOOP
        
    }
    
    while(1) { // Y-min
        
    }
    return 0;
}