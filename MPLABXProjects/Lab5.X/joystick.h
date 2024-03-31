/****************************************************/
/*                                                  */
/*   CS-454/654 Embedded Systems Development        */
/*   Instructor: Renato Mancuso <rmancuso@bu.edu>   */
/*   Boston University                              */
/*                                                  */
/*   Description: template file to implement basic  */
/*                macros and functions to interface */
/*                with the analog joystick.         */
/*                                                  */
/****************************************************/

#ifndef __JOYSTICK_H
#define __JOYSTICK_H

#include <p33Fxxxx.h>
#include "types.h"

/* Please NOTE: */
/* BTN1 (BUTTON 1) is the "fire" button on the joystick*/
/* BTN2 (BUTTON 2) is the "thumb" button on the koystick */
#define B1TRIS TRISEbits.TRISE8
//#define B2TRIS TRISDbits.TRISD10
#define B1PORT PORTEbits.RE8
//#define B2PORT PORTDbits.RD10

#define initialize_buttons()\
        AD1PCFGHbits.PCFG20 = 1;\
        B1TRIS = 1;
        

#define BTN1_PRESSED() \
    v1 == v2 && v2 == 0


#define BTN1_RELEASED() \
    v1 == v2 && v2 != 0 

//#define BTN2_PRESSED() \
//    x1 == x2 && x2 == 0
//
//
//#define BTN2_RELEASED() \
//    x1 == x2 && x2 != 0

/* === Function prototypes implemented in joystic.h go here === */


#endif /* __JOYSTICK_H */


