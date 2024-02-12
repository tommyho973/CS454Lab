/****************************************************/
/*                                                  */
/*   CS-454/654 Embedded Systems Development        */
/*   Instructor: Renato Mancuso <rmancuso@bu.edu>   */
/*   Boston University                              */
/*                                                  */
/*   Description: template file for serial          */
/*                communication server              */
/*                                                  */
/****************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include "pc_crc16.h"
#include "lab3.h"
#include "lab3_troll.h"
#include "types.h"


#define GREETING_STR						\
    "CS454/654 - Lab 3 Server\n"				\
    "Author: Renato Mancuso (BU)\n"				\
    "(Enter a message to send.  Type \"quit\" to exit)\n"

#define USAGE_STR							\
	"\nUSAGE: %s [-v] [-t percentage] <dev>\n"			\
	"   -v \t\t Verbose output\n"					\
	"   -t \t\t Invoke troll with specified bit flipping percentage\n" \
	"   <dev> \t Path to serial terminal device to use, e.g. /dev/ttyUSB0\n\n"

#define TROLL_PATH "./lab3_troll"

int main(int argc, char* argv[])
{
	double troll_pct=0.3;		// Perturbation % for the troll (if needed)
	int ifd,ofd,i,N,troll=0;	// Input and Output file descriptors (serial/troll)
	char str[MSG_BYTES_MSG],opt;	// String input
	struct termios oldtio, tio;	// Serial configuration parameters
	int VERBOSE = 0;		// Verbose output - can be overriden with -v
	int dev_name_len;
	char * dev_name = NULL;
	
	/* Parse command line options */
	while ((opt = getopt(argc, argv, "-t:v")) != -1) {
		switch (opt) {
		case 1:
			dev_name_len = strlen(optarg);
			dev_name = (char *)malloc(dev_name_len);
			strncpy(dev_name, optarg, dev_name_len);
			break;
		case 't':
			troll = 1; 
			troll_pct = atof(optarg);                    
			break;
		case 'v':
			VERBOSE = 1;
			break;
		default:
			break;
		}
	}

	/* Check if a device name has been passed */
	if (!dev_name) {
		fprintf(stderr, USAGE_STR, argv[0]);
		exit(EXIT_FAILURE);
	}
	
	// Open the serial port (/dev/ttyS1) read-write
	ifd = open(dev_name, O_RDWR | O_NOCTTY);
	if (ifd < 0) {
		perror(dev_name);
		exit(EXIT_FAILURE);
	}

	printf(GREETING_STR);

	// Start the troll if necessary
	if (troll)
	{
		// Open troll process (lab5_troll) for output only
		FILE * pfile;		// Process FILE for troll (used locally only)
		char cmd[128];		// Shell command

		snprintf(cmd, 128, TROLL_PATH " -p%f %s %s", troll_pct,
			 (VERBOSE) ? "-v" : "", dev_name);

		pfile = popen(cmd, "w");
		if (!pfile) { perror(TROLL_PATH); exit(-1); }
		ofd = fileno(pfile);
	}
	else ofd = ifd;		// Use the serial port for both input and output

	//
 	// WRITE ME: Set up the serial port parameters and data format
	//
//    CLEARBIT(U2MODEbits.UARTEN);
//    IEC1bits.U2RXIE= 0;
//    IEC1bits.U2TXIE = 0;
//    IFS1bits.U2RXIF = 0;
//    IFS1bits.U2TXIF= 0;
//    TRISFbits.TRISF2 = 1;
//    TRISFbits.TRISF3 = 0;
//    U2MODEbits.BRGH = 0;
//    U2BRG = (uint32_t)800000/9600 -1;
//    U2MODE = 0;
//    U2MODEbits.RTSMD = 0;
//    U2MODEbits.UEN = 0;
//    U2MODE |= 0x00;
//    U2MODEbits.UARTEN = 1;
//    U2STA = 0;
//    U2STAbits.UTXEN = 0;

    CLEARBIT(U1MODEbits.UARTEN);
    
    IEC0bits.U1RXIE= 0;
    IEC0bits.U1TXIE = 0;
    
    IFS0bits.U1RXIF = 0;
    IFS0bits.U1TXIF= 0;
    
    TRISFbits.TRISF2 = 1;
    TRISFbits.TRISF3 = 0;
    
    U1MODEbits.BRGH = 0;
    U1BRG = (uint32_t)800000/9600 -1;
    
    U1MODE = 0;
    U1MODEbits.RTSMD = 0;
    U1MODEbits.UEN = 0;
    U1MODE |= 0x00;
    
    U1MODEbits.UARTEN = 1;
    U1STA = 0;
    U1STAbits.UTXEN = 0;
    
    tios.
    

	while(1)
	{

		//
		// WRITE ME: Read a line of input (Hint: use fgetc(stdin) to read each character)
		//
        char phrase[100];
        int i = 0;
        int c;
        while ((c = fgetc(stdin)) != EOF && c != '\n' && i < 999) {
            phrase[i++] = (char)c;
        }   
        phrase[i] = '\0';
        
//        int i = 0;
//        while ((str[i] = fgetc(stdin)) != '\n' && i < MSG_BYTES_MSG - 1) {
//            i++
//        }   
//        str[i] = '\0';
        

		if (strcmp(str, "quit") == 0) break;

		//
		// WRITE ME: Compute crc (only lowest 16 bits are returned)
        uint16_t crc = pc_crc16(phrase, i);
		//
        
        //uint16_t crc = pc_crc16(str, strlen(str))
        
        int ack = 1;
        int attempts = 0;
	
		while (!ack)
		{
			printf("Sending (attempt %d)...\n", ++attempts);

			
			// 
			// WRITE ME: Send message
			//
//            while(U1STAbits.UTXBF);
//            U1TXREG = crc;
//            while(!U1STAbits.TRMT);
            
            write()

		
			printf("Message sent, waiting for ack... ");

			
			//
			// WRITE ME: Wait for MSG_ACK or MSG_NACK
			//
            while(MSG_ACK == 0){


			printf("%s\n", ack ? "ACK" : "NACK, resending");
            }
		}
		printf("\n");
	}


	//
	// WRITE ME: Reset the serial port parameters
	//
//    CLEARBIT(U2MODEbits.UARTEN);
//    IEC1bits.U2RXIE= 0;
//    IEC1bits.U2TXIE = 0;
//    IFS1bits.U2RXIF = 0;
//    IFS1bits.U2TXIF= 0;
//    TRISFbits.TRISF2 = 1;
//    TRISFbits.TRISF3 = 0;
//    U2MODEbits.BRGH = 0;
//    U2BRG = (uint32_t)800000/9600 -1;
//    U2MODE = 0;
//    U2MODEbits.RTSMD = 0;
//    U2MODEbits.UEN = 0;
//    U2MODE |= 0x00;
//    U2MODEbits.UARTEN = 1;
//    U2STA = 0;
//    U2STAbits.UTXEN = 0;
    
    CLEARBIT(U1MODEbits.UARTEN);
    
    IEC0bits.U1RXIE= 0;
    IEC0bits.U1TXIE = 0;
    
    IFS0bits.U1RXIF = 0;
    IFS0bits.U1TXIF= 0;
    
    TRISFbits.TRISF2 = 1;
    TRISFbits.TRISF3 = 0;
    
    U1MODEbits.BRGH = 0;
    U1BRG = (uint32_t)800000/9600 -1;
    
    U1MODE = 0;
    U1MODEbits.RTSMD = 0;
    U1MODEbits.UEN = 0;
    U1MODE |= 0x00;
    
	// Close the serial port
	close(ifd);
	
	return EXIT_SUCCESS;
}

