/****************************************************/
/*                                                  */
/*   CS-454/654 Embedded Systems Development        */
/*   Instructor: Renato Mancuso <rmancuso@bu.edu>   */
/*   Boston University                              */
/*                                                  */
/*   Description: simple HelloWorld application     */
/*                for Amazing Ball platform         */
/*                                                  */
/****************************************************/

#include <p33Fxxxx.h>
//do not change the order of the following 3 definitions
#define FCY 12800000UL 
#include <stdio.h>
#include <libpic30.h>
#include <uart.h>

#include "lcd.h"
#include "led.h"
#include "joystick.h"
#include "types.h"

/* Initial configuration by EE */
// Primary (XT, HS, EC) Oscillator with PLL
_FOSCSEL(FNOSC_PRIPLL);

// OSC2 Pin Function: OSC2 is Clock Output - Primary Oscillator Mode: XT Crystal
_FOSC(OSCIOFNC_OFF & POSCMD_XT); 

// Watchdog Timer Enabled/disabled by user software
_FWDT(FWDTEN_OFF);

// Disable Code Protection
_FGS(GCP_OFF);  


int main(){
	/* LCD Initialization Sequence */
	__C30_UART=1;	
	lcd_initialize();
	lcd_clear();
	lcd_locate(0,0);
	lcd_printf("Tommy Ho");
    gotoLine(2);
    lcd_printf("Jianying Liu");
    gotoLine(3);
    lcd_printf("Arianit Balidemaj");
    gotoLine(4);
    led_initialize();
    uint8_t i;
    for(i = 0; i < 3; i++){
      SETLED(LED4_PORT);  
      __delay_ms(1000);
      CLEARLED(LED4_PORT);
      __delay_ms(1000);
    }
    initialize_buttons()
	/* Do nothing */
    SETBIT(B1TRIS);
    SETBIT(B2TRIS);
    CLEARBIT(LED1_TRIS);
    uint8_t counter = 0;
    uint8_t prev = 1;

    while(1){
        uint8_t v1 = B1PORT;
        __delay_ms(50);
        uint8_t v2 = B1PORT;
        __delay_ms(50);
        uint8_t x1 = B2PORT;
        __delay_ms(50);
        uint8_t x2 = B2PORT;

        
        if(BTN1_PRESSED()&&(v2 <.5)&& (v2 != prev)){
            SETLED(LED1_PORT);
            counter ++;
            lcd_clear_row(4);
            gotoLine(4);
            lcd_printf("# Pressed DEC: %d",counter);
            lcd_clear_row(5);
            gotoLine(5);
            lcd_printf("# Pressed DEC: %x",counter);
            prev = v2;
        }
        if(BTN1_RELEASED()){
            CLEARLED(LED1_PORT);
            prev = 1;
        }
        if(BTN2_PRESSED()){
            SETLED(LED2_PORT);
        }
        if(BTN2_RELEASED()){
            CLEARLED(LED2_PORT);
        }
        if(B1PORT == B2PORT){
            CLEARLED(LED3_PORT);
        }
        if(B1PORT != B2PORT){
            SETLED(LED3_PORT);
        }
//        LEDPORT &= ~(1<<LED1);
//        LEDPORT |= ~(B1PORT)<<LED1;
//        LEDPORT &= ~(1<<LED2);
//        LEDPORT |= ~(B2PORT)<<LED2;
//        LEDPORT &= ~((1<<LED3)||(1<<LED4));
        
    }        
    
    
    return 0;
}


