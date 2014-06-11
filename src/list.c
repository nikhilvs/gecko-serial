#include "list.h"
#include <stdio.h>
#include "serial_core.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "gecko_interface.h"
#include "log.h"
#include <syslog.h>

//#define LIST_TEST 0
struct tag *start = NULL;
pthread_rwlock_t lock;
static short LOCK_INIT = 0;
//void notify(struct tag * );
static void init_lock()
{
	if (pthread_rwlock_init(&lock, NULL) != 0)
		LOGGER(LOG_INFO,"can't init rwlock");
	else
		LOCK_INIT = 1;
}

void destroy_lock()
{
	if (pthread_rwlock_destroy(&lock) != 0)
		LOGGER(LOG_INFO,"can't destroy rwlock");
	else
		LOCK_INIT = 0;
}

void read_lock()
{
	if (!LOCK_INIT)
		init_lock();
	if (pthread_rwlock_rdlock(&lock) != 0)
		LOGGER(LOG_INFO,"can't get rwdlock");
}

void write_lock()
{
	if (!LOCK_INIT)
		init_lock();
	if (pthread_rwlock_wrlock(&lock) != 0)
		LOGGER(LOG_INFO,"can't get rdlock");
}

void unlock()
{
	pthread_rwlock_unlock(&lock);
}

struct tag* create_node()
{
	struct tag *node;
	node = (struct tag *) malloc(sizeof(struct tag));
	memset(node, 0, sizeof(struct tag));
	memset(node->bdid, 0, sizeof(node));
	return node;
}

void add_update_node(unsigned char *bdid, struct tag_data* data)
{

	struct tag *node = find_node(bdid);
	write_lock();
	if (node == NULL)
	{
		node = create_node();
		node->next = start;
		start = node;

		strncpy(node->bdid, bdid, strlen(bdid));
		node->bdid[strlen(bdid)] = '\0';
	}
	else
	{
		LOGGER(LOG_DEBUG,"\nnode in update block");
	}
	node->data.battery_percent = data->battery_percent;
	node->data.temperature = data->temperature;
	node->data.x = data->x;
	node->data.y = data->y;
	node->data.z = data->z;
	node->data.beacon_id = data->beacon_id;
	node->data.seconds_after_last_motion = data->seconds_after_last_motion;
	strcpy(node->data.device_name, data->device_name);
	notify(node);
	unlock();

//	free(data);
}

struct tag * find_node(unsigned char *bdid)
{
	struct tag *temp;

	read_lock();
	for (temp = start; temp != NULL; temp = temp->next)
	{
		if (!strcmp(temp->bdid, bdid))
		{
			LOGGER(LOG_DEBUG,"Search successful for node\n");
			break;
		}
	}
	unlock();
	return temp;
}

void delete_node(unsigned char *bdid)
{
	LOGGER(LOG_DEBUG,"node delete\n");
	struct tag *node, *prev_node = NULL, *next_node;
	write_lock();
	node = start;
	next_node = node->next;
	while (node != NULL)
	{
		if (!strcmp(node->bdid, bdid))
		{
			if (prev_node == NULL)   //first node
			{
				start = next_node;
			}
			else if (next_node == NULL)  // last node
			{
				//nothing to do
				prev_node->next = NULL;
			}
			else
			{
				prev_node->next = next_node;
			}
			free(node);
			break;
		}
		prev_node = node;
		node = node->next;
		next_node = node->next;
	}
	unlock();
}

void delete_all_node()
{
	struct tag *current, *next;

	current = start;
	write_lock();
	while (current != NULL)
	{
		next = current->next;
		free(current);
		current = next;
	}
	start = NULL;
	unlock();
	LOGGER(LOG_DEBUG,"\n\n\t\tTAG INFO AFTER DELETE ALL(shud be empty )\n\n");
	print_all_node();
}

int get_number_of_connected_devices()
{
	struct tag* node;
	int count = 0;
	node = start;
	while (node != NULL)
	{
		node = node->next;
		count++;
	}
	return count;
}

unsigned char* get_all_node_in_string()
{
	struct tag* node;
//	int dev_count = (int)get_number_of_connected_devices();
//	char * device_list=malloc((dev_count*sizeof(char))+dev_count);
	static unsigned char device_list[DEVICE_LIST_LEN];
	memset(device_list, '\0', sizeof(device_list));
	node = start;
	while (node != NULL)
	{
		strcat(device_list, node->bdid);
		strcat(device_list, ",");
		node = node->next;
	}

	return device_list;
}

void print_all_node()
{
	struct tag* node;

	read_lock();

	node = start;
	if(node != NULL)
	LOGGER(LOG_INFO,"\n\n\t\tTAG INFO\n\n");
	while (node != NULL)
	{
		LOGGER(LOG_INFO,"\r\n%s:%s,%d,%d,%d,%d,%d,%c,%d\n",node->data.device_name,node->bdid,node->data.x,node->data.y,node->data.z,node->data.seconds_after_last_motion,node->data.battery_percent,node->data.beacon_id,node->data.temperature);
		node = node->next;
	}
	unlock();
}

#ifdef LIST_TEST
int main()
{
	char *bdid="20CD39848D16";
	struct tag_data *data=malloc(sizeof(struct tag_data));

	data->battery_percent=23;
	strcpy(data->device_name,"gecko");
	strcpy(data->event_type,"motion");
	data->temperature=33;
	add_update_node(bdid,data);
	print_all_node();

	bdid="20CD39848D17";
	data->battery_percent=24;
	strcpy(data->device_name,"picko");
	strcpy(data->event_type,"motion");
	data->temperature=32;
	add_update_node(bdid,data);
	print_all_node();

	bdid="20CD39848D16";
	data->battery_percent=22;
	strcpy(data->device_name,"picko");
	strcpy(data->event_type,"moti1n");
	data->temperature=30;
	add_update_node(bdid,data);
	print_all_node();

	bdid="20CD39848D18";
	data->battery_percent=21;
	strcpy(data->device_name,"pinko");
	strcpy(data->event_type,"moton");
	data->temperature=37;
	add_update_node(bdid,data);
	print_all_node();

	LOGGER(LOG_INFO,"deleting .....");

	delete_node("20CD39848D17");

	print_all_node();

	delete_all_node();

	free(data);
	return 0;

}
#endif
