
#include "log.h"
#include <syslog.h>
#include <stdio.h>
#include <unistd.h>
#include "list.h"
#include "serial_core.h"
#include "gecko_interface.h"




static advertisement_callback  notify_callback=NULL;
static response_callback notify_response_callback=NULL;



/**
 *   <b> LOG LEVELS INFO </b>
 *   Use this function to change log level
 *   Uses syslog to dump logs,default level is LOG_INFO
 *   1.  LOG_EMERG	    0	    system is unusable
 *   2.  LOG_ALERT	    1	    action must be taken immediately
 *   3.  LOG_CRIT	    2	    critical conditions
 *   4.  LOG_ERR		3	    error conditions
 *	 5.  LOG_WARNING	4	    warning conditions
 *   6.  LOG_NOTICE	    5	    normal but significant condition
 *   7.  LOG_INFO 	    6	    informational
 * 	 8.  <b>LOG_DEBUG      7       uses printf instead of syslog</b>
 * 	 @param level log level as explained above
 */
void gecko_set_log_level(int level)
{
	LOGGER(LOG_INFO,"\nbefore :%d",LOG_LEVEL);
#undef LOG_LEVEL
#define LOG_LEVEL level
	close_log();
	init_log();
	LOGGER(LOG_INFO,"\n after :%d\n",LOG_LEVEL);
}

/**
 * This is an internal function, don't use
 */
void notify_advertisement(struct tag * data)
{
	LOGGER(LOG_INFO,"\nin notify\n");
	if(notify_callback!=NULL)
	(*notify_callback)(data);
	else
		LOGGER(LOG_WARNING,"notify_callback call back ptr is null....");
}

/**
 *  This is an internal function,don't use
 */
void notify_response(unsigned char * data)
{
	LOGGER(LOG_INFO,"\nin notify\n");
	if(notify_response_callback!=NULL)
	(*notify_response_callback)(data);
	else
		LOGGER(LOG_WARNING,"response_callback call back ptr is null....");
}




//! Returns list of ble devices detected in scan period
/*!
@param scan_period scan period in seconds
@return The concated list of bdid separated with coma,
*/
unsigned char* gecko_get_scanned_device_list(int scan_period)
{
	delete_all_node();
	usleep(scan_period*1000*1000);
	unsigned char* response = (unsigned char*)get_all_node_in_string();
	LOGGER(LOG_INFO,"\nresponse :%s \n", response);
	return response;
}


//! Sends command to BLE master over uart
/*!
@param command an integer argument.
@return write/flush <b>tcdrain</b> status of the command, 0 in success
*/
int gecko_send_command(char * command)
{
	return serial_write(tty_fd,command);
}




/**
 * Use this function to register the callback,to receive BLE device advertisements.
 *
 */
void gecko_register_advertisement_callback(advertisement_callback  ptr_reg_callback)
{
    LOGGER(LOG_INFO,"inside register_callback\n");
    /* calling our advertisement_callback function my_callback */
    notify_callback=ptr_reg_callback;
//    (*ptr_reg_callback)();
}

/**
 * Use this function to register the callback,to receive response of the send_command
 *
 */
void gecko_register_command_response_callback(response_callback  ptr_reg_callback)
{
    LOGGER(LOG_INFO,"inside register command response callback\n");
    /* calling our advertisement_callback function my_callback */
    notify_response_callback=ptr_reg_callback;
//    (*ptr_reg_callback)();
}
