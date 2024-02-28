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
#include "lab3.h"


uint8_t buffering[MSG_MAX_LEN];
int trollreader = 0;
int writetroll = 0;

inline void uart2_init(uint16_t baud)
{
    U2MODEbits.UARTEN = 0;
    IEC1bits.U2RXIE = 0;
    IEC1bits.U2TXIE = 0;
    IFS1bits.U2RXIF =0;
    IFS1bits.U2TXIF = 0;
    TRISFbits.TRISF4 = 1; // Check if these are for UART2 as well
    TRISFbits.TRISF5 = 0;
    U2MODEbits.BRGH = 0;
    U2BRG = (uint32_t)800000/baud -1;
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

int8_t uart2_recv(uint8_t *data)
{
	/* Implement me please. */
    if(U2STAbits.OERR){
        U2STAbits.OERR=0;
    }
    if(U2STAbits.URXDA == 0){
//        writetroll = (writetroll + 1) % MSG_MAX_LEN;
//        if(writetroll != trollreader){
            *data = U2RXREG & 0x00FF;
            return 0;
//        }
    }
    return -1;
}

uint8_t usart_getc(){
    while(uart2_recv(buffering[writetroll])!= 0);
    uint8_t return_num;
//    trollreader = (trollreader + 1) % MSG_MAX_LEN;
//    if(trollreader != writetroll){
        return_num = buffering[writetroll];
        writetroll = (writetroll + 1) % MSG_MAX_LEN;
//    }
    return return_num;

}