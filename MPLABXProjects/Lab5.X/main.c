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
    lcd_locate(1,0);
    
    initialize_buttons();
    
    
    touch_init(); // starts ADC for joystick
    set_timer2(4000);
//    motor_init(7);
    
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
        curr = touch_read();
        lcd_clear_row(2);
        lcd_locate(1,0);
        lcd_printf("X-max?: %d", curr);

        if(BTN1_PRESSED()&&(v2 <.5)&& (v2 != prev)){ //If you know that you have pressed the button, then you can take a snapshot of the x axis max 
            prev = v2;
            lcd_clear_row(2);
            lcd_locate(1,0);
            lcd_printf("X-max: %d", curr);
            break;
        }   
    }

    if(BTN1_RELEASED()){
            prev = 1;
        }

    while(1){ // X -MIN LOOP
        // Debouncing 
        uint8_t v1 = B1PORT;
        __delay_ms(10);
        uint8_t v2 = B1PORT;
        __delay_ms(10); //Change back to 50 if need be
        
        lcd_clear_row(3);
        lcd_locate(2,0);
        curr = touch_read();
        lcd_printf("X-min?: %d", curr);

        if(BTN1_PRESSED()&&(v2 <.5)&& (v2 != prev)){ //If you know that you have pressed the button, then you can take a snapshot of the x axis max 
            prev = v2;
            lcd_clear_row(3);
            lcd_locate(2,0);
            lcd_printf("X-min: %d", curr);
            break;
        }   
    }

    if(BTN1_RELEASED()){
            prev = 1;
        }

    touch_select_dim(1); //Select y axis
    while(1) { // Y - MAX LOOP
        // Debouncing 
        uint8_t v1 = B1PORT;
        __delay_ms(10);
        uint8_t v2 = B1PORT;
        __delay_ms(10); //Change back to 50 if need be
        
        lcd_clear_row(4);
        lcd_locate(3,0);
        curr = touch_read();
        lcd_printf("Y-max?: %d", curr);

        if(BTN1_PRESSED()&&(v2 <.5)&& (v2 != prev)){ //If you know that you have pressed the button, then you can take a snapshot of the x axis max 
            prev = v2;
            lcd_clear_row(4);
            lcd_locate(3,0);
            lcd_printf("Y-max: %d", curr);
            break;
        }   
    }
    
    if(BTN1_RELEASED()){
            prev = 1;
        }

    while(1) { // Y-min
        // Debouncing 
        uint8_t v1 = B1PORT;
        __delay_ms(10);
        uint8_t v2 = B1PORT;
        __delay_ms(10); //Change back to 50 if need be
        
        lcd_clear_row(5);
        lcd_locate(4,0);
        curr = touch_read();
        lcd_printf("Y-min?: %d", curr);

        if(BTN1_PRESSED()&&(v2 <.5)&& (v2 != prev)){ //If you know that you have pressed the button, then you can take a snapshot of the x axis max 
            prev = v2;
            lcd_clear_row(5);
            lcd_locate(4,0);
            lcd_printf("Y-min: %d", curr);
            break;
        }   
    }

    if(BTN1_RELEASED()){
            prev = 1;
        }

    // Move the X motor based on what the X position is
    motor_init(8);
    touch_select_dim(0);
    while(1){
        uint8_t v1 = B1PORT;
        __delay_ms(10);
        uint8_t v2 = B1PORT;
        __delay_ms(10); //Change back to 50 if need be

        curr = touch_read();
        duty_cycle; // Math to calculate the duty cycle
        motor_set_duty(8, duty_cycle);
        lcd_clear_row(6);
        lcd_locate(5,0);
        lcd_printf("PW X?: %d", duty_cycle)
        
        if(BTN1_PRESSED()&&(v2 <.5)&& (v2 != prev)){ //If you know that you have pressed the button, then you can take a snapshot of the x axis max 
            prev = v2;
            lcd_clear_row(6);
            lcd_locate(5,0);
            lcd_printf("PW X: %d", duty_cycle);
            break;
        }   
    }

    if(BTN1_RELEASED()){
            prev = 1;
        }

    // Move the Y motor based on what the Y position is
    motor_init(7);
    touch_select_dim(1);
    while(1){
        uint8_t v1 = B1PORT;
        __delay_ms(10);
        uint8_t v2 = B1PORT;
        __delay_ms(10); //Change back to 50 if need be

        curr = touch_read();
        duty_cycle; // Math to calculate the duty cycle
        motor_set_duty(7, duty_cycle);
        lcd_clear_row(7);
        lcd_locate(6,0);
        lcd_printf("PW Y?: %d", duty_cycle)
        
        if(BTN1_PRESSED()&&(v2 <.5)&& (v2 != prev)){ //If you know that you have pressed the button, then you can take a snapshot of the x axis max 
            prev = v2;
            lcd_clear_row(7);
            lcd_locate(6,0);
            lcd_printf("PW Y: %d", duty_cycle);
            break;
        }   
    }

    if(BTN1_RELEASED()){
            prev = 1;
        }

    return 0;
}