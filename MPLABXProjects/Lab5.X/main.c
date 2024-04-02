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
int16_t duty_cycle = 0;
int16_t xmax = 0;
int16_t xmin = 0;
int16_t ymax = 0;
int16_t ymin = 0;
uint8_t v1,v2;
int16_t curr = 0;

double beta;
double difference;
double curr_difference;
double alpha;
double prop;
double final;



/* FIXXXX*/
void __attribute__((__interrupt__)) _T2Interrupt( void )
{
    /* Interrupt Service Routine code goes here */
//    motor_set_duty(chan, duty_cycle); // Write Duty Cycle value for next PWM cycle
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
//    motor_init(7);
    
    // Code for the trigger buttons debouncing
    initialize_buttons(); // for triggers
    SETBIT(B1TRIS); // fire button
//    SETBIT(B2TRIS);
    uint8_t prev = 1;
    
    
    touch_select_dim(0); //Select x axis
    while(1) { //Loop for X MAX
        // Debouncing 
        v1 = B1PORT;
        __delay_ms(10);
        v2 = B1PORT;
        __delay_ms(10); //Change back to 50 if need be
        
// SO IT READS THE VALUES CORRECTLY BUT THE PROBLEM IS SOMETHING RELATED TO HOW WE ARE SAMPLING
        curr = touch_read();
        gotoLine(2);
        lcd_printf("X-max?: %d       ", curr);

        if(BTN1_PRESSED()&& (v2 != prev)){ //If you know that you have pressed the button, then you can take a snapshot of the x axis max 
            prev = v2;
            
            lcd_clear_row(2);
            gotoLine(2);
            xmax = curr;
            lcd_printf("X-max: %d      ", xmax);
            break;
            }
    }
    
    
    
    while (1){
        v1 = B1PORT;
        __delay_ms(50);
        v2 = B1PORT;
        __delay_ms(50); 
        
        if(BTN1_RELEASED()){
            prev = 1;
            gotoLine(4);
            break;
        }
    }
    
    while(1){ // X -MIN LOOP
        // Debouncing 
        v1 = B1PORT;
        __delay_ms(10);
        v2 = B1PORT;
        __delay_ms(10); //Change back to 50 if need be
        
        gotoLine(3);
        curr = touch_read();
        lcd_printf("X-min?: %d       ", curr);

        if(BTN1_PRESSED()&&(v2 <.5)&& (v2 != prev)){ //If you know that you have pressed the button, then you can take a snapshot of the x axis max 
            prev = v2;
            lcd_clear_row(3);
            gotoLine(3);
            xmin = curr;
            lcd_printf("X-min: %d      ", xmin);
            break;
        }   
    }

//    while(!BTN1_RELEASED()){
//            prev = 1;
//        }
    
    while (1){
        v1 = B1PORT;
        __delay_ms(50);
        v2 = B1PORT;
        __delay_ms(50); 
        
        if(BTN1_RELEASED()){
            prev = 1;
            gotoLine(5);
            break;
        }
    }

    touch_select_dim(1); //Select y axis
    while(1) { // Y - MAX LOOP
        // Debouncing 
        v1 = B1PORT;
        __delay_ms(10);
        v2 = B1PORT;
        __delay_ms(10); //Change back to 50 if need be
        
        gotoLine(4);
        curr = touch_read();
        lcd_printf("Y-max?: %d      ", curr);

        if(BTN1_PRESSED()&& (v2 != prev)){ //If you know that you have pressed the button, then you can take a snapshot of the x axis max 
            prev = v2;
            lcd_clear_row(4);
            gotoLine(4);
            ymax = curr;
            lcd_printf("Y-max: %d      ", ymax);
            break;
        }   
    }
    
//    while(!BTN1_RELEASED()){
//            prev = 1;
//        }
    
    while (1){
        v1 = B1PORT;
        __delay_ms(50);
        v2 = B1PORT;
        __delay_ms(50); 
        
        if(BTN1_RELEASED()){
            prev = 1;
            gotoLine(5);
            break;
        }
    }

    while(1) { // Y-min
        // Debouncing 
        uint8_t v1 = B1PORT;
        __delay_ms(10);
        uint8_t v2 = B1PORT;
        __delay_ms(10); //Change back to 50 if need be
        
        gotoLine(5);
        curr = touch_read();
        lcd_printf("Y-min?: %d       ", curr);

        if(BTN1_PRESSED()&&(v2 <.5)&& (v2 != prev)){ //If you know that you have pressed the button, then you can take a snapshot of the x axis max 
            prev = v2;
            lcd_clear_row(5);
            gotoLine(5);
            ymin = curr;
            lcd_printf("Y-min: %d      ", ymin);
            break;
        }   
    }

    while (1){
        v1 = B1PORT;
        __delay_ms(50);
        v2 = B1PORT;
        __delay_ms(50); 

        if(BTN1_RELEASED()){
            prev = 1;
            gotoLine(5);
            break;
        }
    }
    
    // Move the X motor based on what the X position is
    motor_init(8);
    touch_select_dim(0);
    difference = (xmax - xmin);
    while(1){
        uint8_t v1 = B1PORT;
        __delay_ms(10);
        uint8_t v2 = B1PORT;
        __delay_ms(10); //Change back to 50 if need be

        curr = touch_read();
        
        curr_difference = curr - xmin;
        beta = curr_difference/difference;
        gotoLine(7);
        if (beta < 0){
            beta = 0;
        }
        alpha = (.9 + (1.200*beta))*1000;
        uint16_t alpha_int = alpha;
        
        prop = alpha/20000;
        final = 4000 -(4000*(prop));
        
        duty_cycle = final; // Math to calculate the duty cycle
        
        motor_set_duty(8, duty_cycle);
        gotoLine(6);
        lcd_printf("PW X?: %d          ", alpha_int);
        
        if(BTN1_PRESSED()&& (v2 != prev)){ //If you know that you have pressed the button, then you can take a snapshot of the x axis max 
            prev = v2;
            lcd_clear_row(6);
            gotoLine(6);
            lcd_printf("PW X: %d        ", alpha_int);
            break;

        }   
    }

    while (1){
        v1 = B1PORT;
        __delay_ms(50);
        v2 = B1PORT;
        __delay_ms(50); 
        
        if(BTN1_RELEASED()){
            prev = 1;
            gotoLine(6);
            break;
        }
    }

    // Move the Y motor based on what the Y position is
    motor_init(7);
    touch_select_dim(1);
    difference = (ymax - ymin);
    while(1){
        uint8_t v1 = B1PORT;
        __delay_ms(10);
        uint8_t v2 = B1PORT;
        __delay_ms(10); //Change back to 50 if need be

        curr = touch_read();
        
        
        curr_difference = curr - ymin;
        beta = curr_difference/difference;
        gotoLine(7);
        if (beta < 0){
            beta = 0;
        }
        alpha = (.9 + (1.200*beta))*1000;
        uint16_t alpha_int = alpha;
        prop = alpha/20000;
        final = 4000 -(4000*(prop));
        
        duty_cycle = final; // Math to calculate the duty cycle
        motor_set_duty(7, duty_cycle);

        gotoLine(7);
        lcd_printf("PW Y?: %d       ", alpha_int)
        
        if(BTN1_PRESSED()&&(v2 <.5)&& (v2 != prev)){ //If you know that you have pressed the button, then you can take a snapshot of the x axis max 
            prev = v2;
            lcd_clear_row(7);
            gotoLine(7);
            lcd_printf("PW Y: %d             ", alpha_int);
            break;
        }   
    }

    while (1){
        v1 = B1PORT;
        __delay_ms(50);
        v2 = B1PORT;
        __delay_ms(50); 
        
        if(BTN1_RELEASED()){
            prev = 1;
            break;
        }
    }
   
    while(1){
//        uint8_t v1 = B1PORT;
//        __delay_ms(10);
//        uint8_t v2 = B1PORT;
//        __delay_ms(10);
//        
//        
//        if(BTN1_PRESSED()&& (v2 != prev)){ //If you know that you have pressed the button, then you can take a snapshot of the x axis max 
//            prev = v2;
//            break;
//        }
//        
////        if(BTN1_RELEASED()){
////            prev = 1;
////            break;
////        }
        
    }
    
    return 0;
}