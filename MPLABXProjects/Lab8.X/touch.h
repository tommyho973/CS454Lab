/****************************************************/
/*                                                  */
/*   CS-454/654 Embedded Systems Development        */
/*   Instructor: Renato Mancuso <rmancuso@bu.edu>   */
/*   Boston University                              */
/*                                                  */
/*   Description: lab5 touch functions header       */
/*                                                  */
/****************************************************/

#include <p33Fxxxx.h>
#include "types.h"

void touch_init(void);

void touch_select_dim(uint8_t dimension);

uint16_t touch_read(void);