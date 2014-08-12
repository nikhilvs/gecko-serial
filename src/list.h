/*
 * list.h
 *
 *  Created on: Jun 3, 2014
 *      Author: nikhilvs9999
 */

#ifndef LIST_H_
#define LIST_H_
#include "gecko_interface.h"


//#define BDID_LEN 12



struct tag * find_node( char *);
void add_update_node( char[], struct tag_data*);
void delete_node( char *);
void delete_all_node();
void destroy_lock();
char* get_all_node_in_string();
void print_all_node();
#endif /* HASH_H_ */
