/****************************************************/
/*                                                  */
/*   CS-454/654 Embedded Systems Development        */
/*   Instructor: Renato Mancuso <rmancuso@bu.edu>   */
/*   Boston University                              */
/*                                                  */
/*   Description: lab3 UART2 init,TX,RX functions   */
/*                                                  */
/****************************************************/

#include <p33FJ256MC710.h>

#include "uart.h"

inline void uart2_init(uint16_t baud)
{
    U2MODEbits.UARTEN = 0;
    IEC1bits.U2RXIE = 0;
    IEC1bits.U2TXIE = 0;
    IFS1bits.U2RXIF =0;
    IFS1bits.U2TXIF = 0;
    TRISFbits.TRISF2 = 1; // Check if these are for UART2 as well
    TRISFbits.TRISF3 = 0;
    U2MODEbits.BRGH = 0;
    U2BRG = (uint32_t)800000 / baud -1;
    U2MODE = 0;
    U2MODEbits.RTSMD = 0;
    U2MODEbits.UEN = 0;
    U2MODE |= 0x00;
    U2MODEbits.UARTEN = 1;
    U2STA = 0;
    U2STAbits.UTXEN = 1;
}

void uart2_send_8(int8_t data)
{
	/* Implement me please. */
    while(U2STAbits.UTXBF);
    U2TXREG = data;
    while(!U2STAbits.TRMT);
}

int8_t uart2_recv(uint8_t* data)
{
	/* Implement me please. */
    if(U2STAbits.OERR){
        U2STAbits.OERR=0;
    }
    if(U2STAbits.URXDA){
        *data = U2RXREG & 0x00FF;
        return 0;
    }
    return -1;
}
