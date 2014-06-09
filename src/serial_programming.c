/*
 ============================================================================
 Name        : serial_programming.c
 Author      : Nikhil VS
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>
#include <semaphore.h>
#include <syslog.h>

#include "serial_core.h"

#define SERAIL_TEST

char start_scan[] = { '#', '1', '1', '0', '0', '0', '$' };
char stop_scan[] = { '#', '2', '1', '0', '0', '0', '$' };
sem_t mutex;
void clean_up() {
//	serial_write(tty_fd, stop_scan);
	close(tty_fd);
	stop_reader_thread();
	sem_destroy(&mutex);
	delete_all_node();
}

// Define the function to be called when ctrl-c (SIGINT) signal is sent to process
void signal_callback_handler(int signum) {
	printf("\nCaught signal %d\n", signum);
	// Cleanup and close up stuff here
	clean_up();
	// Terminate program
	exit(signum);
}
//struct termios options;

void log_str(char * string) {
//	write(STDOUT_FILENO, string, strlen(string));

	openlog("seriallog", LOG_PID|LOG_CONS, LOG_USER);
	syslog(LOG_INFO,string);
	closelog();
}

void log_char(char c) {
	write(STDOUT_FILENO, &c, 1);
}
void log_int(int c) {
	write(STDOUT_FILENO, &c, 1);
}

void dump(char *str)
{
    char *p = str;
    int len=strlen(str),n;
    printf("\n");
    for (n = 0; n < len; ++n)
    {
        printf("%2.2x ", *p);
        ++p;
    }

    printf("\t");

    p = str;
    for (n = 0; n < len; ++n)
    {
        printf("%c", *p ? *p : ' ');
        ++p;
    }

    printf("\n", str);
}

// cflag=6322 baud=10002
// 1110010110010    7346
// 1100010110010    6322
// 110010111111     3263
void read_display_current_configuration(int fd) {
	struct termios tio;
	memset(&tio, 0, sizeof(tio));
	tcgetattr(fd, &tio);
	printf("\nsize tio.c_cflag :%d", sizeof(tio.c_cflag));
	printf("c_cflag :%d", tio.c_cflag);
	printf("c_iflag :%d", tio.c_iflag);
	printf("c_ispeed :%d", tio.c_ispeed);
	printf("c_oflag :%d", tio.c_oflag);
	printf("c_ospeed :%d", tio.c_ospeed);
	printf("c_cc :%s", tio.c_cc);
	printf("c_line : %c\n", tio.c_line);

}

void test_write() {
	int i, tiktok = 1;
	char printbuffer[8];
	for (i = 0; i < 15; i++) {
		if (tiktok) {
			serial_write(tty_fd, start_scan);
			log_str("wrote start scan ....");
			tiktok = 0;
		} else {
			serial_write(tty_fd, stop_scan);
			log_str("wrote stop scan ....");
			tiktok = 1;
		}
		sleep(5);
		log_str(printbuffer);
	}
}

int start_serial(char * port) {

//	sem_init(&mutex, 0, 0);
//	signal(SIGINT, signal_callback_handler);


	struct sigaction saio;
	saio.sa_handler = signal_callback_handler;
	sigemptyset(&saio.sa_mask);
	saio.sa_flags = 0;
	saio.sa_restorer = NULL;
	sigaction(SIGINT,&saio,NULL);

	//char * port = "/dev/ttyUSB0";

	printf("Using port %s", port);
	int fd = open_serial_port(port);
	if (fd == -1) {
		perror("error in open device :");
		return EXIT_FAILURE;
	}
	if (configure_serial_port(fd) != -1) {
		read_display_current_configuration(fd);
		serial_write(fd, start_scan);
		log_str("send start scan command to serial device\n");
		start_serial_reader_thread(fd);

//		while (1) {
//			log_str("sleeping ...\n");
//			sleep(3);
//			print_all_node();
//		}
//		sem_wait(&mutex);
	} else {
		perror("error in configuring port");
	}

	return EXIT_SUCCESS;
}

/* function to show bytes in memory, from location start to start+n*/
void show_mem_rep(char *start, int n)
{
    int i;
    for (i = 0; i < n; i++)
         printf(" %.2x", start[i]);
    printf("\n");
}

#ifdef SERAIL_TEST
int main(int argc, char** argv) {


//	int i = 0x01234567,e,f;
//	show_mem_rep((char *)&i, sizeof(i));
	char * arr= "CT102:20CD39848D16,31,23,1,0,45%,B,30";

	int a=1,b=2;

	printf("%d",a&&b);
//	char a[5],b[12],g[20];
//
//	dump(arr);
//	sscanf(arr,"%[^':']:%[^','],",a,b);
//
//	log_str("#");
//	log_str(a);
//	log_str("#");
//	log_str(b);
//	log_str("#");
//	log_str(g);
//	log_str("#");
//	printf("%d#%d",e,f);


//	 process_message(arr);
//
//
//	 arr= "CT102:20CD39848D16,137,65,0,0,58%,B,35";
//	 process_message(arr);
//
//	 arr= "CT102:20CD39848D17,137,65,0,0,58%,B,35";
//	 process_message(arr);
//
//	 arr= "CT102:20CD39848D16,137,65,0,0,58%,B,33";
//	 process_message(arr);

	log_str("hello serail world fhsjkdhfksfh .....");

	if (argc < 2) {
		printf(
				"\nNeed UART PORT name as argument : ex- prg-name /dev/ttyUSB0\r\n");
		clean_up();
		exit(0);
	}

//	hash_example();
	return start_serial(argv[1]);
	return 0;
}
#endif
