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
int16_t poss_x;
int16_t poss_y;


// Hardcoded for X VALUES NOT SET RIGHT 
double Kp_x = 0.8;
double Kd_x = 0.2;
double Ki_x = 0.0;

// Hardcoded for Y VALUES NOT SET RIGHT
double Kp_y = 0.8;
double Kd_y = 0.2;
double Ki_y = 0.0;

// Middlepoint for the setpoint
int16_t x_setpoint; //1570;
int16_t y_setpoint; //1490;

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

// Order 3
int N_ord = 3;

// Cut off frequency
int fc = 5;

// Sampling frequency
double fs = (fc *2)/50;

//Coefficient values CHANGE THE VALUES, FIND THE CORRECT ONES
double b [4] = {0.0002, 0.0007, 0.0011, 0.0007};
double a [4] = {1.0000, -3.3441, 4.2389, -2.4093};

//States for the filter in and out for x and y
double x_values_in[4] = {0,0,0,0};
double y_values_in[4] = {0,0,0,0};
double x_values_out[4] = {0,0,0,0};
double y_values_out[4] = {0,0,0,0};


double beta;
double difference;

// For the prev error for PID
double curr_difference;
double prev_difference_x = 0;
double prev_difference_y =0 ;

double alpha;
double prop;
double final;


/* Initial configuration by EE */
// Primary (XT, HS, EC) Oscillator with PLL
_FOSCSEL(FNOSC_PRIPLL);

// OSC2 Pin Function: OSC2 is Clock Output - Primary Oscillator Mode: XT Crystal
_FOSC(OSCIOFNC_OFF & POSCMD_XT); 

// Watchdog Timer Enabled/disabled by user software
_FWDT(FWDTEN_OFF);

// Disable Code Protection
_FGS(GCP_OFF); 


void __attribute__((__interrupt__)) _T2Interrupt( void ) // Might need to change to timer 3
{
    /* Interrupt Service Routine code goes here */
    
    // X MOTOR
    motor_init(8);
    touch_select_dim(0);
    curr = touch_read();
    poss_X = curr;
    curr = filter_position(curr, x_values_in, x_values_out);
    pid_controller(curr, xmax, xmin, x_setpoint, Kp_x, Kd_x, Ki_x,0);
    motor_set_duty(8,duty_cycle);
    
    // // Y MOTOR
    // motor_init(7);
    // touch_select_dim(1);
    // curr = touch_read();
    // poss_Y = curr;
    // curr = filter_position(curr, y_values_in, y_values_out);
    // pid_controller(curr, ymax, ymin, y_setpoint, Kp_y, Kd_y, Ki_y,1);
    // motor_set_duty(7,duty_cycle);

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

    //Middle point
    x_setpoint = (xmax + xmin)/2;
    y_setpoint = (ymax + ymin)/2;

    return 0;
}

int filter_position(double unfiltered, double in_array, double out_array){
    
    double filtered = 0.0;
    
    // Shift the values
    for (int i = N_ord; i > 0; i--){
        in_array[i] = in_array[i-1];
        out_array[i] = out_array[i-1];
    }

    for (int i = 0; i < N_ord+1; i++){
        filtered += b[i]*in_array[i];
        if (i > 0){
            filtered -= a[i]*out_array[i];
        }
    }

    out_array[0] = filtered;

    return filtered;
}

int pid_controller(int16_t filtered, int16_t max, int16_t min, int16_t setpoint, double Kp, double Kd, double Ki, int state){
    
    double error = 0.0;
    double prop = 0.0;
    double integral = 0.0;
    double derivative = 0.0;
    double addition = 0.0;

    // Prop Calculation
    error = setpoint - filtered;
    prop = Kp*error;

    // Integral Calculation
    integral = Ki*error;

    // Derivative Calculation
    if (state == 0){ // X
        curr_difference = prev_difference_x;
        derivative = Kd*(error - curr_difference);
        prev_difference_x = error;
    }
    else{ // Y
        curr_difference = prev_difference_y;
        derivative = Kd*(error - curr_difference);
        prev_difference_y = error;
    }
    
    addition = prop + integral + derivative;

    // Duty Cycle Calculation
    addition = addition * 120; // 120 is the difference from 0 degree to 90 or -90 degrees
    addition = addition + 3700; 
    
    duty_cycle = addition;
    
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
	lcd_locate(0,0);
    lcd_printf("--- Lab 08 ---");
    
    while(1){

        // Print the Kp, Kd, Ki values
        gotoLine(1);
        lcd_printf("Kp: %.2f, Kd: %.2f, Ki: %.2f    ", Kp_x, Kd_x, Ki_x);
        gotoLine(2);
        lcd_printf("Kp: %.2f, Kd: %.2f, Ki: %.2f    ", Kp_y, Kd_y, Ki_y);
        gotoLine(3);
        lcd_printf("SetX: %d, SetY: %d     ", x_setpoint, y_setpoint);
        gotoLine(4);
        lcd_printf("PX: %d, PY: %d     ", poss_x, poss_y);

        __delay_ms(50);

        
    }
    
    return 0;
}