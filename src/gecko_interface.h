/*
 * gecko_interface.h
 *
 *  Created on: Jun 9, 2014
 *      Author: nikhilvs9999
 */

#ifndef GECKO_INTERFACE_H_
#define GECKO_INTERFACE_H_





#define BDID_LEN 13
#define DEVICE_NAME_LEN 11
#define SCAN_PERIOD  7
#define DEVICE_LIST_LEN 280
#define DEVICE_PORT  "/dev/ttyS2"


#define START_SCAN "#11000$"
#define STOP_SCAN  "#21000$"

/*
 *      LOG LEVELS INFO
 *
 *    LOG_EMERG	    0	   *  system is unusable *
 *    LOG_ALERT	    1	   * action must be taken immediately
 *    LOG_CRIT	    2	   * critical conditions *
 *    LOG_ERR		3	   * error conditions *
 *	  LOG_WARNING	4	   * warning conditions *
 *    LOG_NOTICE	5	   * normal but significant condition *
 *    LOG_INFO 	    6	   * informational *
 * 	  LOG_DEBUG            * uses printf instead of syslog
 */
#define LOG_LEVEL LOG_INFO


/*
 *
 * struct tag_data{
 * 	unsigned short temperature;
 *  unsigned short battery_percent;
 * 	unsigned char event_type[8];
 * 	unsigned char device_name[11];
 * };
 *
 *	struct tag{
 * 	  struct tag_data data;
 * 	  unsigned char bdid[BDID_LEN];
 *    };
 */


struct tag_data
{
	unsigned short temperature;
	unsigned short battery_percent;
	unsigned char  device_name[DEVICE_NAME_LEN];
	unsigned char  beacon_id;
	unsigned short x;
	unsigned short y;
	unsigned short z;
	unsigned int   seconds_after_last_motion;
};

struct tag
{
	struct tag_data data;
	unsigned char bdid[BDID_LEN];
	struct tag *next;
};



int send_command(char * );



/*
 *
 * Call this function to receive the in memory list of devices, note that this function is not reentrant.
 *
 */
unsigned char* get_device_list();

typedef void (*callback)(struct tag * data);

void register_callback(callback ptr_reg_callback);
void notify_others(struct tag *);

/*
 * Use this function to test the call back function without help of UART
 */
int test_main();

#endif /* GECKO_INTERFACE_H_ */
