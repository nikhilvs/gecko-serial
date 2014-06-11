
#include "log.h"
#include <syslog.h>
#include <stdio.h>
#include <unistd.h>
#include "list.h"
#include "serial_core.h"
#include "gecko_interface.h"




static callback  notify_callback=NULL;

void notify_others(struct tag * data)
{
	LOGGER(LOG_INFO,"\nOK got callback in test app %s\n", data->bdid);
}

void notify(struct tag * data)
{
	LOGGER(LOG_INFO,"\nin notify\n");
//	void (*notify_callback)(struct tag *);
//	notify_callback = notify_others;
	if(notify_callback!=NULL)
	(*notify_callback)(data);
	else
		LOGGER(LOG_INFO,"call back ptr is null....");
}

unsigned char* get_device_list()
{
	sleep(SCAN_PERIOD);
	unsigned char* response = (unsigned char*)get_all_node_in_string();
	LOGGER(LOG_INFO,"\nresponse :%s \n", response);
	return response;
}

int send_command(char * command)
{
	return serial_write(tty_fd,command);
}


void register_callback(callback  ptr_reg_callback)
{
    LOGGER(LOG_INFO,"inside register_callback\n");
    /* calling our callback function my_callback */
    notify_callback=ptr_reg_callback;
//    (*ptr_reg_callback)();
}
