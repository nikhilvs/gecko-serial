/**
 * @file gecko_interface.h
 * @author nikhilvs9999
 * @date Jun 9, 2014
 * @brief API to interact with BLE master device over UART.
 *
 * This library provide the functionality to interact with gecko BLE master over uart.
 *
 */

#ifndef GECKO_INTERFACE_H_
#define GECKO_INTERFACE_H_





#define BDID_LEN 13
#define DEVICE_NAME_LEN 11
#define SCAN_PERIOD  7
#define DEVICE_LIST_LEN 280



void gecko_set_log_level(int);


//! Struct for storing BLE peripheral device data.
/*! This stucture stores the latest advertisement packet information*/
struct tag_data
{
	unsigned short temperature;  /**< temperature value read from tag */
	unsigned short battery_percent;   /**< battery percent value read from tag */
	char  device_name[DEVICE_NAME_LEN];  /**< device name read from tag */
	char  beacon_id;  /**< beacon id read from tag,for ota purpose */
	unsigned short x;    /**< x coordinate */
	unsigned short y;	 /**< y coordinate */
	unsigned short z;    /**< z coordinate */
	unsigned int   seconds_after_last_motion; /**< seconds after last motion read from tag */
	unsigned short status; /**< status of advertisement(motion detected(0),short press(1),long press(2)) */
};

//! Struct for managing device data.
/*! This is just an data structure to store all device info, since there wont be more devices, i have used linked list  */
struct tag
{
	struct tag_data data;
	unsigned char bdid[BDID_LEN]; /**< BDID of the tag */
	struct tag *next;
};




int gecko_start_serial_service(char * );

typedef struct tag tag;
typedef void (*advertisement_callback)(struct tag *);
typedef void (*response_callback)( char*);


int gecko_send_command(char * );


unsigned char* gecko_get_scanned_device_list(int scan_period);



void gecko_register_advertisement_callback(advertisement_callback);

void gecko_register_command_response_callback(response_callback );


void gecko_clean_up();







#endif /* GECKO_INTERFACE_H_ */
