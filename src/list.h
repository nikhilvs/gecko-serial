/*
 * list.h
 *
 *  Created on: Jun 3, 2014
 *      Author: nikhilvs9999
 */

#ifndef LIST_H_
#define LIST_H_

#define  BDID_LEN 13
#define DEVICE_NAME_LEN 6
#define EVENT_TYPE_LEN 15

//#define BDID_LEN 12


struct tag_data{
	short temperature;
	short battery_percent;
	char event_type[8];
	char device_name[11];
};

struct tag{
	struct tag_data data;
    unsigned char bdid[BDID_LEN];
    struct tag *next;
};


struct tag * find_node(unsigned char *);
void add_update_node(unsigned char [], struct tag_data*);
void delete_node(unsigned char *);
void delete_all_node();
void print_all_node();
#endif /* HASH_H_ */
