/****************************************************/
/*                                                  */
/*   CS-454/654 Embedded Systems Development        */
/*   Instructor: Renato Mancuso <rmancuso@bu.edu>   */
/*   Boston University                              */
/*                                                  */
/*   Description: lab5 motor functions header       */
/*                                                  */
/****************************************************/

#include <p33Fxxxx.h>
#include "types.h"

void motor_init(uint8_t chan);

void motor_set_duty(uint8_t chan, uint16_t duty_us);
