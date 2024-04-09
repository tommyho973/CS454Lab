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

uint8_t chan = 7;
int16_t duty_cycle = 0;
int16_t xmax = 0;
int16_t xmin = 0;
int16_t ymax = 0;
int16_t ymin = 0;
uint8_t counter;
double beta;
double difference;
double curr_difference;
double alpha;
double prop;
double final;
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

uint16_t median1;
uint16_t median2;

/* Initial configuration by EE */
// Primary (XT, HS, EC) Oscillator with PLL
_FOSCSEL(FNOSC_PRIPLL);

// OSC2 Pin Function: OSC2 is Clock Output - Primary Oscillator Mode: XT Crystal
_FOSC(OSCIOFNC_OFF & POSCMD_XT); 

// Watchdog Timer Enabled/disabled by user software
_FWDT(FWDTEN_OFF);

// Disable Code Protection
_FGS(GCP_OFF); 


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


int main(void)
{	
	__C30_UART=1;	
	lcd_initialize();
	lcd_clear();
	lcd_locate(0,0);
    
    lcd_printf("--- Lab 06 ---");
    gotoLine(2);
    
    touch_init(); 
    while(1) {
    
    counter = 0;
    lcd_clear();
	lcd_locate(0,0);
    
    
    lcd_printf("--- Lab 06 ---");
        
    
        //Bottom Left Corn#include <stdlib.h>er
        // Move the X motor based on what the X position is
    motor_init(8);
    touch_select_dim(0);
    duty_cycle = n90; // -90 degrees
    motor_set_duty(8, duty_cycle);

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
    
    counter = 0;
    //Sort the list
    qsort(corner1X, (counter+1), sizeof(uint16_t), comp);
    qsort(corner1Y, (counter+1), sizeof(uint16_t), comp);

    //Print the medians for corner 1
    median1 = findMedian(corner1X,counter+1);
    median2 = findMedian(corner1Y,counter+1);
    gotoLine(2);
    lcd_printf("C1: MX=%d , MY=%d    ", median1,median2);
    
//    gotoLine(2);
//    lcd_printf("C1: X=%d , Y=%d  ", corner1X[counter], corner1Y[counter]);


    //Top Right Corner
    motor_init(8);
    touch_select_dim(0);
    duty_cycle = p90; // -90 degrees
    motor_set_duty(8, duty_cycle);

    // Move the Y motor based on what the Y position is
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
    
    counter = 0;
    // Do the rest of the corners
    qsort(corner2X, (counter+1), sizeof(uint16_t), comp);
    qsort(corner2Y, (counter+1), sizeof(uint16_t), comp);

    //Print the medians for corner 1
    median1 = findMedian(corner2X,counter+1);
    median2 = findMedian(corner2Y,counter+1);
    gotoLine(3);
    lcd_printf("C2: MX=%d , MY=%d    ", median1,median2);
    
//    gotoLine(3);
//    lcd_printf("C2: X=%d , Y=%d  ", corner2X[counter], corner2Y[counter]);


    //Upper Right Corner
    motor_init(8);
    touch_select_dim(0);
    duty_cycle = p90; // -90 degrees
    motor_set_duty(8, duty_cycle);

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
    
    counter = 0;
    // Corner 3
    qsort(corner3X, (counter+1), sizeof(uint16_t), comp);
    qsort(corner3Y, (counter+1), sizeof(uint16_t), comp);

    //Print the medians for corner 1
    median1 = findMedian(corner3X,counter+1);
    median2 = findMedian(corner3Y,counter+1);
    gotoLine(4);
    lcd_printf("C3: MX=%d , MY=%d    ", median1,median2);
    
//    gotoLine(4);
//    lcd_printf("C3: X=%d , Y=%d  ", corner3X[counter], corner3Y[counter]);

    

    //Bottom Right Corner
    motor_init(8);
    touch_select_dim(0);
    duty_cycle = n90; // -90 degrees
    motor_set_duty(8, duty_cycle);

    // Move the Y motor based on what the Y position is
    motor_init(7);
    touch_select_dim(1);
    duty_cycle = p90;
    motor_set_duty(7,duty_cycle);

    __delay_ms(2000);

    for(counter = 0; counter < 5; counter++){
        corner4Y[counter] = touch_read();
        touch_select_dim(0);
        __delay_ms(20);
        corner4X[counter] = touch_read();
        touch_select_dim(1);
        __delay_ms(20);
    }
    
    // Corner 4
    qsort(corner4X, (counter+1), sizeof(uint16_t), comp);
    qsort(corner4Y, (counter+1), sizeof(uint16_t), comp);

    //Print the medians for corner 1
    median1 = findMedian(corner4X,counter+1);
    median2 = findMedian(corner4Y,counter+1);
    gotoLine(5);
    lcd_printf("C4: MX=%d , MY=%d    ", median1,median2);
    

//    gotoLine(5);
//    lcd_printf("C4: X=%d , Y=%d  ", corner4X[counter], corner4Y[counter]);
    
    
    __delay_ms(5000);
    
}
    
    return 0;
}