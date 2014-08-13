#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <error.h>
#include "gpio.h"

#define UNIT_TEST 0

int reserve_gpio_port(int gpio_pin)
{
	int fd;
	char buf[MAX_BUF];
	if ((fd = open(GPIO_EXPORT_FILE, O_WRONLY)) != -1)
	{
		sprintf(buf, "%d", gpio_pin);
		write(fd, buf, strlen(buf));
	}
	else
	{
		perror("Export File open");
	}
	close(fd);
	return fd;
}

int set_gpio_direction(int direction)
{
	int fd;
	if ((fd = open(GPIO_DIRECTION_FILE, O_WRONLY)) != -1)
	{
		if (direction == OUT)
		{
			write(fd, "out", 3);
//			set_gpio_value(HIGH);
		}
		else
			write(fd, "in", 2);
	}
	else
	{
		perror("Direction File open");
	}
	close(fd);
	return fd;
}

int set_gpio_value(int value)
{
	int fd;
	if ((fd = open(GPIO_VALUE_FILE, O_WRONLY)) != -1)
	{
		if(value==HIGH)
			write(fd,"1",1);
		else
			write(fd,"0",1);
	}
	else
	{
		perror("Values File open");
	}
	close(fd);
	return fd;
}

int unreserve_gpio_port(char* unreserve_file_path, int gpio_pin)
{
	char buf[MAX_BUF];
	int fd;
	if ((fd = open(GPIO_UNEXPORT_FILE, O_WRONLY)) != -1)
	{
		sprintf(buf, "%d", gpio_pin);
		write(fd, buf, strlen(buf));
	}
	else
	{
		perror("Unreserve File open");
	}
	close(fd);
	return fd;
}

int do_power_reset()
{
	int status = -1;
	status = set_gpio_value(LOW);
	usleep(SET_LOW_SLEEP);
	status = set_gpio_value(HIGH);
	usleep(RESTART_SLEEP);
	return status;
}

int initialize_gecko_gpio()
{
	int status = -1;
	if (reserve_gpio_port(GPIO_PORT) != -1 && set_gpio_direction(OUT) != -1)
	{
		status = set_gpio_value(HIGH);
	}
	return status;
}

#if UNIT_TEST==1
int main(int argc, char** argv)
{
	initialize_gecko_gpio();
	printf("Sleeping for some time \n");
	usleep(10*1000*1000);
	printf("Doing power reset .... \n");
	do_power_reset();
	printf("Done \n");
	return EXIT_SUCCESS;
}
#endif
