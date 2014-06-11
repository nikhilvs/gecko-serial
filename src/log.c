/*
 * log.c
 *
 *  Created on: Jun 11, 2014
 *      Author: nikhilvs9999
 */
#include "log.h"

#include <syslog.h>
#include <stdio.h>
#include <stdarg.h>
#include "gecko_interface.h"



void init_log()
{
	setlogmask (LOG_UPTO (LOG_LEVEL));
	openlog("gecko_serial", LOG_PID | LOG_CONS | LOG_NDELAY, LOG_USER);

}
void close_log()
{
	closelog();
}

void log_info(char * string)
{

	syslog(LOG_INFO, string);

}
void LOGGER(int level, const char *format, ...)
{
    va_list args;
    va_start(args, format);
	if (LOG_LEVEL != LOG_DEBUG)
	{
	  vsyslog(level,format,args);
	}
	else
	  vprintf(format, args);
    va_end(args);
}

