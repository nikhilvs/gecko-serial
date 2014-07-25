/*
 ============================================================================
 Name        : serial_programming.c
 Author      : Nikhil VS
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "log.h"
#include <syslog.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>
#include <semaphore.h>
#include <pthread.h>
#include <syslog.h>
#include "gecko_interface.h"
#include "serial_core.h"
#include "list.h"

//#define SERAIL_TEST
#define DEVICE_PORT 1

static short LOCK_INIT = 0;
static pthread_mutex_t lock;
static void init_serial_lock()
{
	if (pthread_mutex_init(&lock, NULL) != 0)
	    {
		LOGGER(LOG_CRIT,"\n mutex init failed\n");
	    }
	else
		LOCK_INIT = 1;
}

void destroy_serial_lock()
{
	if (pthread_mutex_destroy(&lock) != 0)
		LOGGER(LOG_INFO,"can't destroy rwlock");
	else
		LOCK_INIT = 0;
}

void read_serial_lock()
{
	if (!LOCK_INIT)
		init_serial_lock();
	pthread_mutex_lock(&lock);
}

void write_serial_lock()
{
	if (!LOCK_INIT)
		init_serial_lock();
	if (pthread_mutex_lock(&lock) != 0)
		LOGGER(LOG_INFO,"can't get rdlock");
}

void serial_unlock()
{
	pthread_mutex_unlock(&lock);
}







/**
 * Clean up function, which releases all the resources used by this library.
 * By default SIGINT signal is registered to call this function.
 * Resources freed in this function :
 * 1. Closes device file descriptor
 * 2. Free the mutex and locks
 * 3. Closes syslog
 * @warning fail to call this function, can result in memory leak.
 */
void gecko_clean_up()
{
//	serial_write(tty_fd, stop_scan);
	close(tty_fd);
	stop_reader_thread();
	destroy_serial_lock();
	destroy_lock();
	delete_all_node();
	close_log();
}

// Define the function to be called when ctrl-c (SIGINT) signal is sent to process
void signal_callback_handler(int signum)
{
	LOGGER(LOG_DEBUG,"\nCaught signal %d\n", signum);
	// Cleanup and close up stuff here
	gecko_clean_up();
	// Terminate program
	exit(signum);
}
//struct termios options;


// cflag=6322 baud=10002
// 1110010110010    7346
// 1100010110010    6322
// 110010111111     3263
void read_display_current_configuration(int fd)
{
	struct termios tio;
	memset(&tio, 0, sizeof(tio));
	tcgetattr(fd, &tio);
	LOGGER(LOG_DEBUG,"\nsize tio.c_cflag :%d", sizeof(tio.c_cflag));
	LOGGER(LOG_DEBUG,"\nsize tio.c_cflag :%d", sizeof(tio.c_cflag));
	LOGGER(LOG_DEBUG,"c_cflag :%d", tio.c_cflag);
	LOGGER(LOG_DEBUG,"c_iflag :%d", tio.c_iflag);
	LOGGER(LOG_DEBUG,"c_ispeed :%d", tio.c_ispeed);
	LOGGER(LOG_DEBUG,"c_oflag :%d", tio.c_oflag);
	LOGGER(LOG_DEBUG,"c_ospeed :%d", tio.c_ospeed);

}



/**  Main function which starts the service, this will be an blocking function.If any error is there it returns immediately.
 * Returns immediately on error, else will be an blocking function.
 * @param port serial device port name.
 * @return error status of <b>open()</b> or <b>tcsetattr()</b>.
 * @note This is an blocking function, so it should be called from different thread.
 *
 * */
int gecko_start_serial_service(char * port)
{

	init_serial_lock();
	struct sigaction saio;
	saio.sa_handler = signal_callback_handler;
	sigemptyset(&saio.sa_mask);
	saio.sa_flags = 0;
	saio.sa_restorer = NULL;
	sigaction(SIGINT, &saio, NULL);

	//char * port = "/dev/ttyUSB0";

	LOGGER(LOG_DEBUG,"Using port %s", port);
	int fd = open_serial_port(port);
	if (fd == -1)
	{
		LOGGER(LOG_ERR,"error in open device %s",port);
		return EXIT_FAILURE;
	}
	if (configure_serial_port(fd) != -1)
	{
		if(LOG_LEVEL==LOG_DEBUG)
		read_display_current_configuration(fd);

		serial_write(fd, START_SCAN);
		LOGGER(LOG_DEBUG,"send start scan command to serial device\n");
		start_serial_reader_thread(fd);
	}
	else
	{
		LOGGER(LOG_CRIT,"Error in opening serial port\n");
		perror("error in configuring port");
	}

	return EXIT_SUCCESS;
}

/** /fn function to show bytes in memory, from location start to start+n **/
void show_mem_rep(char *start, int n)
{
	int i;
	for (i = 0; i < n; i++)
		LOGGER(LOG_DEBUG," %.2x", start[i]);
	LOGGER(LOG_DEBUG,"\n");
}

#ifdef SERAIL_TEST
int main(int argc, char** argv)
#else
int test_main()
#endif
{


//	register_advertisement_callback(callback_advrtsmnt);
//	register_command_response_callback(callback_response);
//	int i = 0x01234567,e,f;
//	show_mem_rep((char *)&i, sizeof(i));
//	char port[13];
//	char * arr = "1,CT102:20CD39848D16,31,23,1,0,45%,B,30\0";


//	char a[6],b[13],g[4]={},f='a';
//	int x=0,y=0,z=0,u=0,v=0,time=0,t=0;
////
////	dump(arr);
//	printf("%s\n",arr);
//	sscanf(arr,"%[^':']:%[^','],%[^','],%[^','],%[^','],%[^','],%[^','],%c[^','],%[]",a,b,&x,&y,&z,&time,g,&f,&t);
//	a[5]='\0';
//	b[12]='\0';
//	g[3]='\0';
////
//	printf("\r\n%s:%s,%d,%d,%d,%d,%s,%c,%d\n",a,b,x,y,z,time,g,f,t);

//	 arr="3,20CD39848D16,79,0";
//	 process_message(arr);
//
//
//	 arr= "1,CT102:20CD39848D16,137,65,0,0,58%,B,35\0";
//	 process_message(arr);
//
//	 arr= "1,CT102:20CD39848D17,137,65,0,0,58%,B,35\0";
//	 process_message(arr);
//
//	 arr= "1,CT102:20CD39848D16,137,65,0,0,58%,B,33\0";
//	 process_message(arr);


//	 get_device_list();
//
//	LOGGER(LOG_INFO,"hello serail world fhsjkdhfksfh .....");

#ifndef DEVICE_PORT
	if (argc < 2)
	{
		printf(
				"\nNeed UART PORT name as argument : ex- prg-name /dev/ttyUSB0\r\n");
		gecko_clean_up();
		exit(0);
	}
	strcpy(port,argv[1]);
#else
//	strcpy(port, DEVICE_PORT);
#endif


//	clean_up();
//	return start_serial(port);
	return 0;
}

