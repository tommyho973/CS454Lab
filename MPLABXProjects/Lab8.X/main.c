#include <p33Fxxxx.h>
//do not change the order of the following 3 definitions
#define FCY 12800000UL 
#include <stdio.h>
#include <libpic30.h>
#include <uart.h>

#include <stdlib.h>

#include "lcd.h"
#include "types.h"
#include "motor.h"
#include "touch.h"
#include "timer.h"

uint8_t chan = 7;
int16_t duty_cycle = 0;
uint8_t counter;
int16_t curr = 0;

// Hardcoded for X VALUES NOT SET RIGHT 
double Kp_x = 0.8;
double Kd_x = 0.2;
double Ki_x = 0.0;

// Hardcoded for Y VALUES NOT SET RIGHT
double Kp_y = 0.8;
double Kd_y = 0.2;
double Ki_y = 0.0;

// Middlepoint for the setpoint
int16_t x_setpoint = 1570;
int16_t y_setpoint = 1490;

//Extreme values
int16_t xmax = 0;
int16_t xmin = 0;
int16_t ymax = 0;
int16_t ymin = 0;

// For finding the extremes
uint16_t median1;
uint16_t median2;

int n90 = 3820;
int p90 = 3580;

uint16_t corner1X[5];
uint16_t corner1Y[5];
uint16_t corner2X[5];
uint16_t corner2Y[5];
uint16_t corner3X[5];
uint16_t corner3Y[5];
uint16_t corner4X[5];
uint16_t corner4Y[5];

// Order 3
int N_ord = 3;

//Coefficient values CHANGE THE VALUES, FIND THE CORRECT ONES
double b [4] = {0.0002, 0.0007, 0.0011, 0.0007};
double a [4] = {1.0000, -3.3441, 4.2389, -2.4093};

double beta;
double difference;
double curr_difference;
double alpha;
double prop;
double final;


// FORMULA FOR CALCULATING THE ANGLE TO FEED

//        curr_difference = curr - xmin;
//        beta = curr_difference/difference;
//        gotoLine(7);
//        if (beta < 0){
//            beta = 0;
//        }
//        alpha = (.9 + (1.200*beta))*1000;
//        uint16_t alpha_int = alpha;
//        
//        prop = alpha/20000;
//        final = 4000 -(4000*(prop));
//        
//        duty_cycle = final;


/* Initial configuration by EE */
// Primary (XT, HS, EC) Oscillator with PLL
_FOSCSEL(FNOSC_PRIPLL);

// OSC2 Pin Function: OSC2 is Clock Output - Primary Oscillator Mode: XT Crystal
_FOSC(OSCIOFNC_OFF & POSCMD_XT); 

// Watchdog Timer Enabled/disabled by user software
_FWDT(FWDTEN_OFF);

// Disable Code Protection
_FGS(GCP_OFF); 


void __attribute__((__interrupt__)) _T2Interrupt( void )
{
    /* Interrupt Service Routine code goes here */

    IFS0bits.T2IF = 0; // Clear Timer 2 interrupt flag
}

int comp (const void * elem1, const void * elem2) 
{
    int f = *((int*)elem1);
    int s = *((int*)elem2);
    if (f > s) return  1;
    if (f < s) return -1;
    return 0;
}

uint16_t findMedian(uint16_t array[], size_t num_elements) {
    if (num_elements % 2 == 0) {
        // If number of elements is even
        return (array[num_elements / 2 - 1] + array[num_elements / 2]) / 2;
    } else {
        // If number of elements is odd
        return array[num_elements / 2];
    }
}

int find_extreme(void){
    
    counter = 0;
    lcd_clear();
	lcd_locate(0,0);
    
    
    lcd_printf("--- Lab 08 ---");
    
    motor_init(8);
    touch_select_dim(0);
    duty_cycle = n90; // -90 degrees
    motor_set_duty(8,duty_cycle);

    // Move the Y motor based on what the Y position is
    motor_init(7);
    touch_select_dim(1);
    duty_cycle = n90;
    motor_set_duty(7,duty_cycle);

    __delay_ms(2000);
 
    for(counter = 0; counter < 5; counter++){
        corner1Y[counter] = touch_read();
        touch_select_dim(0);
        __delay_ms(20);
        corner1X[counter] = touch_read();
        touch_select_dim(1);
        __delay_ms(20);
    }
    
    
    //Sort the list
    qsort(corner1X, (counter+1), sizeof(uint16_t), comp);
    qsort(corner1Y, (counter+1), sizeof(uint16_t), comp);

    //Print the medians for corner 1
    median1 = findMedian(corner1X,counter+1);
    median2 = findMedian(corner1Y,counter+1);
    gotoLine(2);//
    lcd_printf("C1: MX=%d , MY=%d    ", median1,median2);
    counter = 0;
    
    xmin = median1;
    ymin = median2;
    
    
    //Top Right Corner
    motor_init(8);
    touch_select_dim(0);
    duty_cycle = p90; // -90 degrees
    motor_set_duty(8,duty_cycle);

//     Move the Y motor based on what the Y position is
    motor_init(7);
    touch_select_dim(1);
    duty_cycle = n90;
    motor_set_duty(7,duty_cycle);

    __delay_ms(2000);

    for(counter = 0; counter < 5; counter++){
        corner2Y[counter] = touch_read();
        touch_select_dim(0);
        __delay_ms(20);
        corner2X[counter] = touch_read();
        touch_select_dim(1);
        __delay_ms(20);
    
    }
    
    // Do the rest of the corners
    qsort(corner2X, (counter+1), sizeof(uint16_t), comp);
    qsort(corner2Y, (counter+1), sizeof(uint16_t), comp);

    //Print the medians for corner 1
    median1 = findMedian(corner2X,counter+1);
    median2 = findMedian(corner2Y,counter+1);
    gotoLine(3);
    lcd_printf("C2: MX=%d , MY=%d    ", median1,median2);
    counter = 0;
    
    xmax = median1;
    
    //Upper Right Corner
    motor_init(8);
    touch_select_dim(0);
    duty_cycle = p90; // -90 degrees
    motor_set_duty(8,duty_cycle);

    // Move the Y motor based on what the Y position is
    motor_init(7);
    touch_select_dim(1);
    duty_cycle = p90;
    motor_set_duty(7,duty_cycle);

    __delay_ms(2000);

    for(counter = 0; counter < 5; counter++){
        corner3Y[counter] = touch_read();
        touch_select_dim(0);
        __delay_ms(20);
        corner3X[counter] = touch_read();
        touch_select_dim(1);
        __delay_ms(20);

    }
    
    // Corner 3
    qsort(corner3X, (counter+1), sizeof(uint16_t), comp);
    qsort(corner3Y, (counter+1), sizeof(uint16_t), comp);

    //Print the medians for corner 3
    median1 = findMedian(corner3X,counter+1);
    median2 = findMedian(corner3Y,counter+1);
    gotoLine(4);
    lcd_printf("C3: MX=%d , MY=%d    ", median1,median2);
    
    counter = 0;
    
    ymax = median2;  
    __delay_ms(50000);
    
    return 0;
}


int main(void)
{	
	__C30_UART=1;	
	lcd_initialize();
	lcd_clear();
	lcd_locate(0,0);
  
    lcd_printf("--- Lab 08 ---");
    
    touch_init();
    
    find_extreme(); //finds the extremes of x and y, useful used with the setpoint 
    
//    timer_init(10000); // This is to set to 50 ms for the target
    
    lcd_clear()
    
    while(1){
//        touch_select_dim(1); // y
//        
//        touch_select_dim(0); // x
//        __delay_ms(20);
//        curr = touch_read();
//
//        gotoLine(3);
//        lcd_printf("X: %d  ", curr);
    }
    
    return 0;
}