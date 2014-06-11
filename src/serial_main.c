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
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>
#include <semaphore.h>
#include <syslog.h>
#include "gecko_interface.h"
#include "serial_core.h"
#include "list.h"

#define SERAIL_TEST


sem_t mutex;
void clean_up()
{
//	serial_write(tty_fd, stop_scan);
	close(tty_fd);
	stop_reader_thread();
	sem_destroy(&mutex);
	destroy_lock();
	delete_all_node();
	close_log();
}

// Define the function to be called when ctrl-c (SIGINT) signal is sent to process
void signal_callback_handler(int signum)
{
	LOGGER(LOG_DEBUG,"\nCaught signal %d\n", signum);
	// Cleanup and close up stuff here
	clean_up();
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

void test_write()
{
	int i, tiktok = 1;
	char printbuffer[8];
	for (i = 0; i < 15; i++)
	{
		if (tiktok)
		{
			serial_write(tty_fd, START_SCAN);
			LOGGER(LOG_INFO,"wrote start scan ....");
			tiktok = 0;
		}
		else
		{
			serial_write(tty_fd, STOP_SCAN);
			LOGGER(LOG_INFO,"wrote stop scan ....");
			tiktok = 1;
		}
		sleep(5);
		LOGGER(LOG_INFO,printbuffer);
	}
}

int start_serial(char * port)
{

//	sem_init(&mutex, 0, 0);
//	signal(SIGINT, signal_callback_handler);

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
		perror("error in open device :");
		return EXIT_FAILURE;
	}
	if (configure_serial_port(fd) != -1)
	{
		read_display_current_configuration(fd);
		serial_write(fd, START_SCAN);
//		test_write();
		LOGGER(LOG_INFO,"send start scan command to serial device\n");
		start_serial_reader_thread(fd);

//		while (1) {
//			LOGGER(LOG_INFO,"sleeping ...\n");
//			sleep(3);
//			print_all_node();
//		}
//		sem_wait(&mutex);
	}
	else
	{
		perror("error in configuring port");
	}

	return EXIT_SUCCESS;
}

/* function to show bytes in memory, from location start to start+n*/
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


	register_callback(notify_others);
//	int i = 0x01234567,e,f;
//	show_mem_rep((char *)&i, sizeof(i));
//	char port[13];
	char * arr = "CT102:20CD39848D16,31,23,1,0,45%,B,30\0";


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

	 process_message(arr);


	 arr= "CT102:20CD39848D16,137,65,0,0,58%,B,35";
	 process_message(arr);

	 arr= "CT102:20CD39848D17,137,65,0,0,58%,B,35";
	 process_message(arr);

	 arr= "CT102:20CD39848D16,137,65,0,0,58%,B,33";
	 process_message(arr);


//	 get_device_list();
//
//	LOGGER(LOG_INFO,"hello serail world fhsjkdhfksfh .....");

#ifndef DEVICE_PORT
	if (argc < 2)
	{
		printf(
				"\nNeed UART PORT name as argument : ex- prg-name /dev/ttyUSB0\r\n");
		clean_up();
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

