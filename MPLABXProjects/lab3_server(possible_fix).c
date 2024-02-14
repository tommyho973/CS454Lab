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

void increment(int *pointer)
{
(*pointer)++;
}


int main(int argc, char* argv[])
{
	double troll_pct=0.3;		// Perturbation % for the troll (if needed)
	int ifd,ofd,i,N,troll=0;	// Input and Output file descriptors (serial/troll)
	int ch;
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
    
    tcgetattr(ifd, &oldtio);
	tio.c_cflag 	= B9600 | CS8 | CLOCAL |CREAD;
	tio.c_iflag 	= 0;
	tio.c_oflag 	= 0;
	tio.c_lflag 	= 0;
	tcflush(ifd, TCIFLUSH);
	tcsetattr(ifd, TCSANOW, &tio);

	while(1)
	{

		//
		// WRITE ME: Read a line of input (Hint: use fgetc(stdin) to read each character)
		//
//        char phrase[100];
//        int i = 0;
//        int c;
//        while ((c = fgetc(stdin)) != EOF && c != '\n' && i < 999) {
//            phrase[i++] = (char)c;
//        }   
//        phrase[i] = '\0';
        for(i = 0; i < sizeof(str) - 1 && (((ch = fgetc(stdin))!= EOF) && (ch != "\n")); increment(&i)){
			str[i] = ch;
		}
		str[i] = "\0";
        

		if (strcmp(str, "quit") == 0) break;

		//
		// WRITE ME: Compute crc (only lowest 16 bits are returned)
        //uint16_t crc = pc_crc16(phrase, i);
		//
        
        int crc = pc_crc16(str, strlen(str));
        printf("CRC: %d\n",crc);
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
            
            write(ofd, str, strlen(str));

		
			printf("Message sent, waiting for ack... ");

			
			//
			// WRITE ME: Wait for MSG_ACK or MSG_NACK
			//
            
			read(ifd,&ack,sizeof(ack));

			printf("%s\n", ack ? "ACK" : "NACK, resending");
		}
		printf("\n");
	}


	//
	// WRITE ME: Reset the serial port parameters
	//
	tcflush(ifd, TCIFLUSH);
	tcsetattr(ifd, TCSANOW, &oldtio);
    
	// Close the serial port
	close(ifd);
	
	return EXIT_SUCCESS;
}
