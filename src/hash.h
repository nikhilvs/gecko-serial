/*
 * hash.h
 *
 *  Created on: Jun 3, 2014
 *      Author: nikhilvs9999
 */

#ifndef HASH_H_
#define HASH_H_

#include "uthash.h"


struct device_data {
    short temperature;
    short battery_percent;
    unsigned char event_type[15];
    unsigned char device_name[6];
};
typedef struct device_data dev_data;

struct device {
    dev_data *data;
    unsigned char bdid[12]; /* we'll use this field as the key */
    UT_hash_handle hh; /* makes this structure hashable */
};




typedef struct device dev;



void hash_example();
void add_user(dev *);
struct device *find_user(char *);
void delete_user(char *);
void print_users();
void delete_all();












#endif /* HASH_H_ */
