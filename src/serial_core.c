#include "serial_core.h"
#include "list.h"
#include "log.h"
#include "gecko_interface.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <pthread.h>
#include <semaphore.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>

#ifndef RUN_AS_THREAD
#define RUN_AS_THREAD 1
#endif

static unsigned short DO_READ = 0;
static unsigned short INPUT_READY = 1;
static long SIG_IO_COUNT=0;
void signal_handler_IO(int status)
{
//	LOGGER(LOG_DEBUG,"SIGIO : %d\n", status);
	SIG_IO_COUNT++;
	INPUT_READY = 1;
}

struct tag_data gecko_data;



void process_message(char *packet)
{
	LOGGER(LOG_INFO,"\n\t\tparsing message \nreceived packet :%s",packet);
	char device_bdid[BDID_LEN];
	int len = strlen(packet);
	char *split_parts, *saveptr, *saveptr1;
	char first_byte;

	first_byte=packet[0];
//	LOGGER(LOG_INFO,"\nFirst Byte :%c\n",first_byte);
	int i;

	char * packet_without_header = malloc((len+1) * sizeof(char));
	switch(first_byte)
	{
		case ADVERTISE_PACKET :
		{

			for(i=2;i<len;i++)
			{
				packet_without_header[i-2]=packet[i];
			}
			packet_without_header[i-2]='\0';
//			len=strlen((const char *)packet_without_header);
			len=strlen(packet_without_header);
			LOGGER(LOG_DEBUG,"\n\t\tparsing received packet_without_header :%s\n",packet_without_header);




			//parses  CT102:20CD39848D16,137,65,0,0,58%,B,33 packet
			//        UNKWN:20CD39848D16,255,255,255,255,255,G,255,255
			split_parts = strtok_r(( char *)packet_without_header,(const char *) ":,", &saveptr);
			for (i = 0; split_parts != NULL; i++)
			{
				switch (i)
				{
					case 0:
					{
//						strcpy((char *)gecko_data.device_name,(const char *) split_parts);
						strcpy(gecko_data.device_name, split_parts);
						break;
					}
					case 1:
					{
//						LOGGER(LOG_DEBUG,"\n::%s::\n",split_parts);
//						len = strlen((const char *)split_parts);
						len = strlen(split_parts);
//						strncpy((char *)device_bdid,(const char *) split_parts, len);
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
						if(temp!=NULL)
						gecko_data.battery_percent = atoi(temp);
						break;
					}
					case 7:
					{
						gecko_data.beacon_id=*split_parts;
						break;
					}
					case 8:
					{
						gecko_data.temperature = atoi(split_parts);
						break;
					}
					case 9:
					{
						gecko_data.status = atoi(split_parts);
						break;
					}
				}
				split_parts = strtok_r(NULL, ":,", &saveptr);
			}
			add_update_node(device_bdid, &gecko_data);
			break;
		}
		case CONNECT_WRITE_RESPONSE_PACKET :
		case CONNECT_READ_RESPONSE_PACKET :
		case DEBUG_PACKET :
		case ECHO_PACKET :
		case DISCONNECT_RESPONSE_PACKET :
		{
			for(i=0;i<len;i++)
			{
				packet_without_header[i]=packet[i];
			}
			packet_without_header[i]='\0';
//			len=strlen((const char *)packet_without_header);
			len=strlen(packet_without_header);
			LOGGER(LOG_DEBUG,"\n\t\tparsing received packet :%s\n",packet_without_header);
			notify_response(packet_without_header);
			break;
		}
		default : {
			LOGGER(LOG_WARNING,"\nUNKNOWN PACKET HEADER %s\n",packet_without_header);
			break;
		}
	}


//	print_all_node();
	free(packet_without_header);
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
	long prev_io_count=0;

	while (DO_READ)
	{
		while (INPUT_READY)
		{
			if (read(fd, &c, 1) > 0)
			{

				if (STORE_READ)
				{
					prev_io_count=SIG_IO_COUNT;
					if (i > ERROR_READ_LEN)
					{
						LOGGER(LOG_DEBUG,"error read lenr \n");

					}
					buffer[i++] = c;
				}
				if (c == START_DELIMETER)
				{
					i = 0;
					STORE_READ = 1;
				}
				else if (c == STOP_DELIMETER)
				{
					buffer[i - 1] = '\0';
					STORE_READ = 0;
					LOGGER(LOG_DEBUG,"stop delimiter \n");
					process_message(buffer);
					if(prev_io_count==SIG_IO_COUNT)
					{
						INPUT_READY = 0;
						break;
					}

				}
				else if (i >= ERROR_READ_LEN)
				{
					LOGGER(LOG_DEBUG,"error read lenr \n");
				}
			}
		}
		usleep(POLL_SLEEP);
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
	options.c_iflag &= ~ICRNL;


	//disables h/w control
	options.c_cflag &= ~CNEW_RTSCTS;
	options.c_cflag &= ~(HUPCL | PARODD);

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
	write_serial_lock();
	int status = -1;
	int len = strlen(string), i;
	char c;
	c='#';
	write(fd,&c, 1);
	usleep(WRITE_SLEEP);
	for (i = 0; i < len; i++)
	{
		c = string[i];
		write(fd, &c, 1);
		usleep(WRITE_SLEEP);
	}
	c='$';
	write(fd,&c, 1);
	usleep(WRITE_SLEEP);
	status = tcflush(fd, TCOFLUSH);
	status = tcdrain(fd);
	serial_unlock();
//	LOGGER(LOG_INFO,"\nwrote DATA to UART status(0 ok):%d DATA:%s\r\n",status,string);
	printf("\nwrote DATA to UART status(0 ok):%d DATA:%s\r\n",status,string);
	fflush(stdout);
	return status;
}

void stop_reader_thread()
{
	DO_READ = 0;
}

