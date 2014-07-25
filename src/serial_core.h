/* 
 * File:   serial_core.h
 * Author: nikhilvs
 *
 * Created on May 30, 2014, 11:32 AM
 */

#ifndef SERIAL_CORE_H
#define	SERIAL_CORE_H


#ifdef	__cplusplus
extern "C"
{
#endif

#ifdef	__cplusplus
}
#endif

//#include <termios.h>
#include <semaphore.h>
#include "gecko_interface.h"





#ifndef CNEW_RTSCTS
#define CNEW_RTSCTS CRTSCTS
#endif



//struct termios options;


#define START_SCAN "1,1000\0"
#define STOP_SCAN  "2,1000\0"
#define READ_BUFFER_SIZE 80
#define START_DELIMETER  '#'          // #
#define STOP_DELIMETER   '$'         // $
#define ERROR_READ_LEN   55

#define ONE_MILLISECOND  1000
#define POLL_SLEEP 500*ONE_MILLISECOND
#define WRITE_SLEEP 200*ONE_MILLISECOND

#define ADVERTISE_PACKET   '1'
#define CONNECT_WRITE_RESPONSE_PACKET  '3'
#define CONNECT_READ_RESPONSE_PACKET  '4'
#define DISCONNECT_RESPONSE_PACKET  '5'

int tty_fd;


void write_serial_lock();
void serial_unlock();
int configure_serial_port(int);
void start_serial_reader_thread(int);
void serial_reader();
int serial_write(int, char *);
int open_serial_port(char *);
void stop_reader_thread();
void dump(char *);


void notify_response(unsigned char *);
void notify_advertisement(tag * );

void process_message(char *);

#endif	/* SERIAL_CORE_H */

