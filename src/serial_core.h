/* 
 * File:   serial_core.h
 * Author: nikhilvs
 *
 * Created on May 30, 2014, 11:32 AM
 */


#ifndef SERIAL_CORE_H
#define	SERIAL_CORE_H

#ifndef CNEW_RTSCTS
    #define CNEW_RTSCTS CRTSCTS
#endif
#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif


#include <termios.h>

struct termios options;

#define READ_BUFFER_SIZE 70
#define START_DELIMETER  '#'          // #
#define STOP_DELIMETER   '$'         // $
#define ERROR_READ_LEN   52
int tty_fd;
int configure_serial_port(int);
void start_serial_reader_thread(int);
void serial_reader();
int serial_write(int,char *);
int open_serial_port(char *);
void stop_reader_thread();
void log_str(char *);
void log_char(char ) ;
void dump(char *);

void process_message(char *);





#endif	/* SERIAL_CORE_H */




