#include "gecko_interface.h"
#include <stdio.h>


void notify_others(struct tag * data)
{

	printf("\nOK got callback for %s\n",data->bdid);

}



void notify(struct tag * data)
{
	printf("\nin notify\n");
	void (*notify_callback)(struct tag *);
	notify_callback=notify_others;
	(*notify_callback)(data);
}

char* get_device_list()
{

	return "";
}


