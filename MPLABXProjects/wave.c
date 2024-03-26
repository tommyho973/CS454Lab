/****************************************************/
/*                                                  */
/*   CS-454/654 Embedded Systems Development        */
/*   Instructor: Renato Mancuso <rmancuso@bu.edu>   */
/*   Boston University                              */
/*                                                  */
/*   Description: template file for digital and     */
/*                analog square wave generation     */
/*                via the LabJack U3-LV USB DAQ     */
/*                                                  */
/****************************************************/

#include "u3.h"
#include <unistd.h>
// Included by us libraries
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <string.h>


/* This function should initialize the DAQ and return a device
 * handle. The function takes as a parameter the calibratrion info to
 * be filled up with what obtained from the device. */
HANDLE init_DAQ(u3CalibrationInfo * caliInfo)
{
	HANDLE hDevice;
	int localID = -1;
	
	/* Invoke openUSBConnection function here */
    hDevice = openUSBConnection(localID);
	
	/* Invoke getCalibrationInfo function here */
    if (getCalibrationInfo(hDevice, caliInfo)) {
        return NULL;
    }
	
	return hDevice;
}
float minVoltage, maxVoltage;
float desiredHz;
long handle;
int digital_output_state = 0;
float analog_output_value = 0.0;
HANDLE hDevice;
u3CalibrationInfo caliInfo;

void digital_handler(int signum, siginfo_t * info, void * nothing){
  (void) signum;
  (void) nothing;
  (void) info;
    digital_output_state = !digital_output_state;
    eDO(hDevice,1,2,digital_output_state);
    
}
void analog_handler(int signum,siginfo_t * info, void * nothing){
  (void) signum;
  (void) nothing;
  (void) info;
    if (analog_output_value == maxVoltage){
      analog_output_value = minVoltage;
    }
    else {
      analog_output_value = maxVoltage;
    }
    
    eDAC(hDevice, &caliInfo,1,0,analog_output_value,0,0,0);
}

int main(int argc, char **argv)
{
    char str[255]; //MAX VOLT INPUT
    char str2[255]; //MIN VOLT INPUT
    char str3[255]; //DESIRED VOLT INPUT
    char str4[255]; //Digital Output?
    char str5[255]; //Analog Output?
    char command[255];
    
	/* Invoke init_DAQ and handle errors if needed */
    
    hDevice = init_DAQ(&caliInfo);
	

	/* Provide prompt to the user for a voltage range between 0
	 * and 5 V. Require a new set of inputs if an invalid range
	 * has been entered. */
    
       
        char c;
        int i = 0;
	while (1) {
	  i = 0;
          printf("Provide max voltage between 0 and 5V: ");
          while((c = fgetc(stdin))!= EOF && c!= '\n'){
              str[i++] = c;
          }
        str[i] = '\0';

	if (str[0] == '-') {
	   printf("invalid range \n");
	   continue;
	}
        
        maxVoltage = atof(str); // Converts string to number
        
        printf("Provide min voltage between 0 and 5V: ");
        i = 0;
        while((c = fgetc(stdin))!= EOF && c!= '\n'){
                str2[i++] = c;
        }
        str2[i] = '\0';

	if (str2[0] == '-') {
	   printf("invalid range \n");
	   continue;
	}
        
        minVoltage = atof(str2); // Converts string to number

	printf("Max: %f and Min: %f",maxVoltage, minVoltage);
	
        if (minVoltage >= 0 && maxVoltage <= 5 && maxVoltage > minVoltage) {
	  printf("Nice \n");
	  break;
        } else {
            printf("invalid range \n");
        }
     }// HERE WAS THE }
	
	/* Compute the maximum resolutiuon of the CLOCK_REALTIME
	 * system clock and output the theoretical maximum frequency
	 * for a square wave */
    struct timespec resolution;
    
    if(clock_getres(CLOCK_REALTIME, &resolution)==-1){
        perror("clock_getres");
        return 1;
    };
    
    float seconds = (float)resolution.tv_sec + (float)resolution.tv_nsec / 1e9;
    float max_frequency = 1.0/ (seconds);
	printf("Clock realtime resolution is: %f Hz\n", max_frequency);
    
	/* Provide prompt to the user to input a desired square wave
	 * frequency in Hz. */
    printf("Provide a desired square wave frequency in Hz: ");
    i = 0;
    while((c = fgetc(stdin))!= EOF && c!= '\n'){
        str3[i++] = c;
    }
    str3[i] = '\0';
        
    desiredHz = atof(str3); // Converts string to number
    
	/* Program a timer to deliver a SIGRTMIN signal, and the
	 * corresponding signal handler to output a square wave on
	 * BOTH digital output pin FIO2 and analog pin DAC0. */
    
    //CREATES TIMER 1 that sends a SIGRTMIN signal
    i = 0;
    printf("Do you want to output a digital wave [y/n]? ");
    while((c = fgetc(stdin))!= EOF && c!= '\n'){
      str4[i++] = c;
    }
    str4[i] = '\0';

    i = 0;
    printf("Do you want to output an analog wave [y/n]? ");
    while((c = fgetc(stdin)) != EOF && c!= '\n'){
      str5[i++] = c;
    }
    str5[i] = '\0';

    
    if(strcmp(str4,"y")==0){ 
    struct sigaction sa_digital;
    struct sigevent timer1_event;
    memset(&sa_digital,0,sizeof(sa_digital));
    sa_digital.sa_handler = digital_handler;
    sigemptyset(&sa_digital.sa_mask);
    sa_digital.sa_flags = 0;
    timer1_event.sigev_notify = SIGEV_SIGNAL;
    timer1_event.sigev_signo = SIGRTMIN;
    
    if(sigaction(SIGRTMIN,&sa_digital,NULL) != 0){
        perror("sigaction digital");
        exit(1);
    }
    

    //CALCULATES THE DESIRED FREQUENCY AND STORES IT IN itimerspec
    struct itimerspec timer_spec;
    double seconds1 = 1/ (2*desiredHz);

    long secc1 = seconds1;
    long nsecc1 = ((seconds1 - secc1)* 1e9);
    
    timer_spec.it_interval.tv_sec = secc1;
    timer_spec.it_interval.tv_nsec = nsecc1;
    timer_spec.it_value = timer_spec.it_interval;
    timer_t timer;
    
    if(timer_create(CLOCK_REALTIME,&timer1_event, &timer) != 0){
        perror("timer_create");
        exit(1);
       }
    if(timer_settime(timer,0,&timer_spec,NULL) != 0){
        perror("timer_settime");
        exit(1);
       }
    }
    

	/* The square wave generated on the DAC0 analog pin should
	 * have the voltage range specified by the user in the step
	 * above. */
    
    //CREATES THE TIMER 2 that sends a SIGRTMAX signal and in the handler generates the wave

    if(strcmp(str5,"y") == 0){
    struct sigaction sa_analog;
    struct sigevent timer2_event;
    memset(&sa_analog,0,sizeof(sa_analog));
    sa_analog.sa_handler = analog_handler;
    sigemptyset(&sa_analog.sa_mask);
    sa_analog.sa_flags = 0;
    timer2_event.sigev_notify = SIGEV_SIGNAL;
    timer2_event.sigev_signo = SIGRTMAX;
    if(sigaction(SIGRTMAX,&sa_analog,NULL) != 0){
        perror("sigaction analog");
        exit(1);
    }
    timer_t timer2;
    struct itimerspec timer2_spec;

    double seconds = 1/ (2*desiredHz);

    long secc = seconds;
    long nsecc = ((seconds - secc)* 1e9);
    
    timer2_spec.it_interval.tv_sec = secc;
    timer2_spec.it_interval.tv_nsec = nsecc;
    timer2_spec.it_value = timer2_spec.it_interval;
    if(timer_create(CLOCK_REALTIME,&timer2_event, &timer2) != 0){
        perror("timer_create");
        exit(1);
    }
    if(timer_settime(timer2,0,&timer2_spec,NULL) != 0){
        perror("timer_settime");
        exit(1);
    }
    
    }

    sigset_t main_mask, wait_mask;
    struct sigaction sa, old_sa;
    sa.sa_sigaction = digital_handler;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGRTMIN);
    sigemptyset(&main_mask);
    sigaddset(&main_mask,SIGRTMIN);
    sigaction(SIGRTMIN, &sa, &old_sa);

    struct sigaction sa2;
    sa2.sa_sigaction = analog_handler;
    sa2.sa_flags = SA_SIGINFO;
    sigemptyset(&sa2.sa_mask);
    sigaddset(&sa.sa_mask,SIGRTMAX);
    sigaction(SIGRTMAX,&sa2,NULL);
    sigaddset(&main_mask,SIGRTMAX);

    sigprocmask(SIG_BLOCK,&main_mask,NULL);
    sigemptyset(&wait_mask);    

    //    while(1){
    //  sigsuspend(&wait_mask);
    // }


    
	/* Display a prompt to the user such that if the "exit"
	 * command is typed, the USB DAQ is released and the program
	 * is terminated. */
    //STILL NEED TO DO THIS PART
    printf("Enter command:");
    i = 0;
    while((c = fgetc(stdin))!= EOF && c!= '\n'){
        command[i++] = c;
    }
    command[i] = '\0';

    if (strcmp(command, "exit") == 0){
      closeUSBConnection(hDevice);
      return EXIT_SUCCESS;
       }
    return -1;
}

