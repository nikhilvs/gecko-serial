/*
 ============================================================================
 Name        : serial_usage.c
 Author      : Nikhil VS
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include "gecko_interface.h"
#include "timer_api.h"
#include "gpio.h"

#define UART_PORT  "/dev/ttyS2"

#define  GECKO_SEND_TIMEOUT     20
#define  TIMER_RESTART_INTERVAL 20
#define  RETRY_LIMIT            3

sem_t mutex1;

const char *cmd[] =
{ "3,%s,68,9,4", "3,%s,71,10,12"};
//{ "8,%s-echo-test" };
const char *disconnect_command = "5,%s";
const char *disconnect_command_response[] =
{ "5,%s,1", "5,%s,0" };

char testBDID[20];
const char *success_response[] =
{ "3,%s,71,1", "3,%s,68,1" };
const char *failure_response[] =
{ "3,%s,71,0", "3,%s,68,0" };

const char *valid_response[] =
{ "3,%s,71,1", "3,%s,68,1", "3,%s,71,0", "3,%s,68,0"};
const char *debug_disconnect_ack = "5,%s,3";

//char *cmd[] =
//{ "3,D0FF5066A254,68,9,4", "3,D0FF5066A254,71,10,12" };
//char *disconnect_command = "5,D0FF5066A254";
//char *disconnect_command_response[] =
//{ "5,D0FF5066A254,1", "5,D0FF5066A254,0" };
//
//char testBDID[20];
//char *success_response[] =
//{ "3,D0FF5066A254,71,1", "3,D0FF5066A254,68,1" };
//char *failure_response[] =
//{ "3,D0FF5066A254,71,0", "3,D0FF5066A254,68,0" };
//
//char *valid_response[] =
//{ "3,D0FF5066A254,71,1", "3,D0FF5066A254,68,1", "3,D0FF5066A254,71,0",
//		"3,D0FF5066A254,68,0" };
//char *debug_disconnect_ack = "5,D0FF5066A254,3";

const char *debug_ack = "6,3";

char *currently_running_command = NULL;
int retry_count = RETRY_LIMIT;
short gpio_initialized = 0;

sigset_t mask;
timer_t timerid;
struct itimerspec its;

void wait_for_release()
{
	sem_wait(&mutex1);
}
void unblock()
{
	sem_post(&mutex1);
}
void power_reset()
{
	/*
	 * Will work only if gecko hardware with reset pin is configured
	 */
	int status;
	printf("Doing power reset ......");
	if (!gpio_initialized)
	{
		printf("inside init power reset ......");
		if ((status = initialize_gecko_gpio()) != -1)
			gpio_initialized = 1;
	}
	if (gpio_initialized)
	{
		printf("inside do power reset ......");
		do_power_reset();
	}

}

void send_command(char *command)
{
	gecko_send_command(command);
	retry_count = RETRY_LIMIT;
	start_timer(GECKO_SEND_TIMEOUT, TIMER_RESTART_INTERVAL, &mask, timerid,
			&its);
}
/**
 * Timer Added for retrying failed command
 */

void timer_handler(int sig, siginfo_t *si, void *uc)
{
	/* Note: calling printf() from a signal handler is not
	 strictly correct, since printf() is not async-signal-safe;
	 see signal(7) */

	printf("Caught signal %d retrying command :%s\n", sig,
			currently_running_command);

	if (currently_running_command != NULL
			&& strlen(currently_running_command) > 1)
	{
		gecko_send_command(currently_running_command);
		retry_count--;
		if (retry_count == 0)
		{
			printf(
					"Retry Limit Over ....Possible cause : Master BLE has power reset problem ...\n");
			stop_timer(&mask, timerid, &its);
			power_reset();
			gecko_send_command("1,1000\0");
			unblock();
		}
	}
}

/*
 * After receiving  response to connect command,we should send disconnect command
 *
 */
void send_disconnect_comand()
{
	static char cmd[120];
	sprintf(cmd, disconnect_command, testBDID);
	currently_running_command = cmd;
	usleep(4 * 1000 * 1000);
	printf("\nsending disconnect command :%s", currently_running_command);
	fflush(stdout);
	send_command(currently_running_command);
	fflush(stdout);
}

/*
 *
 * Call back function for receiving advertisement packet
 *
 */
void callback_advrtsmnt(struct tag * data)
{
	if (strcmp((char *) data->bdid, testBDID) == 0)
	{
		printf("\nGot callback advertistment test device %s status: %d\n",
				testBDID, data->data.status);
	}
	else
	{
		printf(
				"\nOK got callback advertisement in test app for %s status :%d\n",
				data->bdid, data->data.status);
	}
}

/*
 *
 * Call back function for receiving response of an previously send request
 *
 */
void callback_response(char * data)
{
	short i, len, got_response = 0;
	printf("\nOK got callback uart response %s\n", data);
	fflush(stdout);
	/**
	 * After successfully sending connect command, we need to send disconnect command.
	 *
	 * Here i send disconnect command other than the responses of disconnect command
	 */
//	if(data[0]!='5' && (strlen(data)>3))
	len = sizeof(valid_response) / sizeof(*valid_response);
	fflush(stdout);

	char response[5][120];
	char debug_ack[50];
	char debug_dis_ack[50];
	for (i = 0; i < len; i++)
	{
		sprintf(response[i], valid_response[i], testBDID);
		if (strcmp(data, response[i]) == 0)
		{
			stop_timer(&mask, timerid, &its);
			printf("\nOK Got write response back\n");
			fflush(stdout);
			got_response = 1;
			send_disconnect_comand();
			break;
		}

	}
	if (got_response)
		return;

	sprintf(debug_ack, debug_ack, testBDID);
	sprintf(debug_dis_ack, debug_disconnect_ack, testBDID);
	if ((strcmp(data, debug_dis_ack) == 0))
	{
		printf("\nOK Got Disconnnect command ACK\n");
		fflush(stdout);
	}
	else if ((strcmp(data, debug_ack) == 0))
	{
		printf("\nOK Got Debug command ACK\n");
		fflush(stdout);
	}
	else
	{
		/**
		 * Allows to send next command , only after receiving response of disconnect command
		 */

		for (i = 0; i < 2; i++)
		{
			sprintf(response[i], disconnect_command_response[i], testBDID);
			if (strcmp(data, response[i]) == 0)
			{
				stop_timer(&mask, timerid, &its);
				printf(
						"\nOK Got Disconnnect command Response ACK, doing mutex change for next command\n");
				fflush(stdout);
				unblock();
				break;
			}
		}
	}
}

/*
 * Example  function to illustrate/unit test the commands
 *
 */
void* gecko_tester(void * arg)
{
	sem_init(&mutex1, 0, 1);
	/**
	 * Sends list of command to test the functionality of BLE Tags
	 */

	int i, j;

	int len;
	int no_commands=sizeof(cmd) / sizeof(*cmd);
	static char command[100];

	printf("\nInside Gecko Tester");

	printf("\ntimer ID is 0x%lx\n", (long) timerid);

	initialize_timer(&mask, &timerid, &its, timer_handler);

	printf("\nWait for some secs to see advertisement packet ......\n");
	fflush(stdout);
	usleep(10 * 1000 * 1000);

	for (j = 0; j < 5; j++)
		for (i = 0; i < no_commands; i++)
		{
			sem_wait(&mutex1);
			len = strlen(cmd[i]);

			fflush(stdout);
			usleep(2 * 1000 * 1000);
//			strcpy(command, cmd[i]);
			memset(command, '\0', strlen(command));
			sprintf(command, cmd[i], testBDID);
//			command[len] = '\0';
			currently_running_command = command;
			printf("\nsending command :%s", currently_running_command);
			fflush(stdout);
			/**
			 * API to send command to UART
			 */

			send_command(currently_running_command);
//		gecko_send_command(currently_running_command);
		}
	wait_for_release();
	/**
	 * Gets the scanned list of gecko tags in surrounding
	 */
	printf("\nScanned List :%s", gecko_get_scanned_device_list(5));
	fflush(stdout);
	printf("\n bdele timer ID is 0x%lx\n", (long) timerid);
	delete_timer(timerid);
	sem_destroy(&mutex1);
	return NULL;
}

/*
 * Start's the blocking UART port service as separate thread.
 *
 */
void* start_uart(void* arg)
{
	char* port = (char *) arg;

	printf("port :%s", port);
	fflush(stdout);
	/**
	 * Starts the blocking gecko service
	 */
	gecko_start_serial_service(port);
	return NULL;
}

int main(int argc, char** argv)
{

	if (argc < 3)
	{
		printf("Please enter in example1 <serialPort> <testBDID>\n\n");
		printf(
				"Where \n<serialPort> is the the serial device name, e.g. /dev/ttyUSB0\n");
		printf("<testBDID> is the test BDID, e.g.D0FF5066A254 \n");
		exit(0);
	}

	printf("\n\t!!!Hello gecko  World!!!\n"); /* prints !!!Hello World!!! */

	char* port = argv[1];
	strcpy(testBDID, argv[2]);

	printf("Connecting to port '%s' and testing with device '%s'\n", port,
			testBDID);
	gecko_set_log_level(7);
	gecko_register_advertisement_callback(callback_advrtsmnt);
	gecko_register_command_response_callback(callback_response);

	pthread_attr_t attr;
	pthread_t reader_thread_id;
	pthread_t uart_thread_id;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&uart_thread_id, &attr, &start_uart, port);
	pthread_attr_destroy(&attr);

	pthread_create(&reader_thread_id, NULL, &gecko_tester, NULL);
	pthread_join(reader_thread_id, NULL);

	gecko_clean_up();
	return EXIT_SUCCESS;
}
