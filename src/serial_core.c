#include "serial_core.h"
#include "list.h"
#include "log.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <pthread.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>

#ifndef RUN_AS_THREAD
#define RUN_AS_THREAD 1
#endif

unsigned int DO_READ = 0;
short INPUT_READY = 0;
void signal_handler_IO(int status)
{
	LOGGER(LOG_INFO,"received SIGIO signal. : %d\n", status);
	INPUT_READY = 1;
}

struct tag_data gecko_data;
void process_message(char *packet)
{
	LOGGER(LOG_INFO,"\n\t\tparsing message \nreceived packet :%s",packet);
	unsigned char device_bdid[BDID_LEN];

	char *split_parts, *saveptr, *saveptr1;
	int i;
	memset(device_bdid,'\0', sizeof(device_bdid));
	memset(gecko_data.device_name,'\0', sizeof(gecko_data.device_name));

//	struct tag_data *gecko_data = malloc(sizeof(struct tag_data));

	int len = strlen(packet);
	char * p = malloc(len * sizeof(char));
	strncpy(p, packet, len);

//	sscanf(p,"%[^':']:%[^','],",gecko_data->device_name,device_bdid);
//	sscanf(p,"%[^','],%[^','],",gecko_data->device_name,device_bdid);
//	device_bdid[BDID_LEN] = '\0';
	split_parts = strtok_r(p, ":,", &saveptr);

	for (i = 0; split_parts != NULL; i++)
	{
		switch (i)
		{
		case 0:
		{
			strcpy(gecko_data.device_name, split_parts);
			break;
		}
		case 1:
		{
			LOGGER(LOG_DEBUG,"::");
			LOGGER(LOG_DEBUG,split_parts);
			LOGGER(LOG_DEBUG,"::\n");
			len = strlen(split_parts);
			strncpy(device_bdid, split_parts, len);
			device_bdid[len] = '\0';
			break;
		}
		case 2:
		{
			gecko_data.x = atoi(split_parts);
			break;
		}
		case 3:
		{
			gecko_data.y = atoi(split_parts);
			break;
		}
		case 4:
		{
			gecko_data.z = atoi(split_parts);
			break;
		}
		case 5:
		{
			gecko_data.seconds_after_last_motion = atoi(split_parts);
			break;
		}
		case 6:
		{
			char * temp = strtok_r(split_parts, "%", &saveptr1);
			gecko_data.battery_percent = atoi(temp);
			break;
		}
		case 7:
		{
//			sprintf(gecko_data.beacon_id,"%c",split_parts);
			break;
		}
		case 8:
		{
			gecko_data.temperature = atoi(split_parts);
			break;
		}
		}
		split_parts = strtok_r(NULL, ":,", &saveptr);
	}
	add_update_node(device_bdid, &gecko_data);
	print_all_node();
	free(p);
//	free(gecko_data);

}

#ifdef RUN_AS_THREAD
void reader(int fd)
#else
void* reader(void* arg)
#endif
{
	char *buffer;
#ifndef RUN_AS_THREAD
	int fd = *((int*) arg);
#endif
	int i = 0;
	short STORE_READ = 0;
	buffer = malloc(READ_BUFFER_SIZE * sizeof(char));
	char c = 'd';

	while (DO_READ)
	{
		while (INPUT_READY)
		{
			if (read(fd, &c, 1) > 0)
			{
				if (STORE_READ)
					buffer[i++] = c;
				if (c == START_DELIMETER)
				{
					i = 0;
					STORE_READ = 1;
					usleep(100);
				}
				else if (c == STOP_DELIMETER)
				{
					buffer[i - 1] = '\0';
					STORE_READ = 0;
					LOGGER(LOG_DEBUG,"stop delimiter \n");
					process_message(buffer);
					INPUT_READY = 0;
					break;
				}
				else if (i == ERROR_READ_LEN)
				{
					LOGGER(LOG_DEBUG,"error read lenr \n");
				}
			}
		}
		sleep(1);
	}
	free(buffer);

#ifndef RUN_AS_THREAD
	return NULL;
#endif

}

/*
 *   Configuration for Gecko Serial port, not making customizable configuration now
 */
int configure_serial_port(int fd)
{
	int status;
	struct termios options;
	memset(&options, 0, sizeof(options));
	tcgetattr(fd, &options);
	/*
	 * Get the current options for the port...
	 */
	status = tcgetattr(fd, &options);

	/*
	 * Set the baud rates to 115200...
	 */
	cfsetispeed(&options, B115200);
	cfsetospeed(&options, B115200);
	/*
	 * Enable the receiver and set local mode...
	 */
	options.c_cflag |= (CLOCAL | CREAD);
	options.c_cflag &= ~CSIZE; /* Mask the character size bits */
	options.c_cflag |= CS8; /* Select 8 data bits */

	// no parity
	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;    // this is redundant
	options.c_cflag |= CS8;            // this is redundant

	//disables software control
	options.c_iflag &= ~(IXON | IXOFF | IXANY);

	//disables h/w control
	options.c_cflag &= ~CNEW_RTSCTS;

	tcflush(fd, TCIOFLUSH);
	status = tcsetattr(fd, TCSANOW, &options);
	/*
	 * Set the new options for the port...
	 */
	return status;
}

void start_serial_reader_thread(int fd)
{
	DO_READ = 1;

	struct sigaction saio;

	saio.sa_handler = signal_handler_IO;
	sigemptyset(&saio.sa_mask);
	saio.sa_flags = 0;
	saio.sa_restorer = NULL;
	sigaction(SIGIO, &saio, NULL);
	/* allow the process to receive SIGIO */
	fcntl(fd, F_SETOWN, getpid());

	fcntl(fd, F_SETFL, FASYNC);  //for async

#ifndef RUN_AS_THREAD
	pthread_attr_t attr;
	pthread_t reader_thread_id;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);

	pthread_create(&reader_thread_id, &attr, &reader, &fd);
	//	pthread_join(reader_thread,NULL);

	pthread_attr_destroy(&attr);
	//	pthread_setcanceltype (PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
#else
	reader(fd);
#endif

}

int open_serial_port(char * port)
{
	tty_fd = open(port, O_RDWR | O_NDELAY | O_NOCTTY);
	if (!isatty(tty_fd))
	{
		perror("Not an proper terminal tty....:");

	}
	return tty_fd;
}

//should use only for writing to serial interface
int serial_write(int fd, char * string)
{
	int status = -1;
	int len = strlen(string), i;
	char c = 'w';
	for (i = 0; i < len; i++)
	{
		c = string[i];
		write(fd, &c, 1);
		usleep(200);
	}
//	if(write(fd, string, strlen(string))!=-1)
//	{
	status = tcflush(fd, TCOFLUSH);
	status = tcdrain(fd);
//	}
//	else
//	{
//		perror("Something wrong happened in serial writer :");
//	}
	LOGGER(LOG_INFO,"wrote ......\r\n");
	return status;
}

void stop_reader_thread()
{
	DO_READ = 0;
}

