/*
 * log.h
 *
 *  Created on: Jun 10, 2014
 *      Author: nikhilvs9999
 */

#ifndef LOG_H_
#define LOG_H_
#include <syslog.h>


#define LOG_LEVEL LOG_INFO

void init_log();

void close_log();


void log_info(char * string);



void LOGGER(int , const char *, ...);




#endif /* LOG_H_ */
