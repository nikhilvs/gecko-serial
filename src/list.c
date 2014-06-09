#include "list.h"
#include <stdio.h>
#include "serial_core.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "gecko_interface.h"

//#define LIST_TEST 0
 struct tag *start=NULL;
 pthread_rwlock_t lock;
 static short LOCK_INIT=0;
 static void init_lock()
 {
 	if (pthread_rwlock_init(&lock,NULL) != 0) log_str("can't init rwlock");
 	else LOCK_INIT=1;
 }

 void destroy_lock()
  {
  	if (pthread_rwlock_destroy(&lock,NULL) != 0) log_str("can't destroy rwlock");
  	else LOCK_INIT=0;
  }

 void read_lock()
 {
	 if(!LOCK_INIT)
	 	 init_lock();
	 if (pthread_rwlock_rdlock(&lock) != 0) log_str("can't get rwdlock");
 }

 void write_lock()
 {
	 if(!LOCK_INIT)
	 	 init_lock();
	 if (pthread_rwlock_wrlock(&lock) != 0) log_str("can't get rdlock");
 }

 void unlock()
 {
	 pthread_rwlock_unlock(&lock);
 }


 struct tag* create_node()
{
	struct tag *node;
	node =(struct tag *)malloc(sizeof(struct tag));
	memset(node,0,sizeof(struct tag));
//	printf("\nmemset value : %d\n",i);
	memset(node->bdid,0,sizeof(node));
//	node->data=(struct tag_data*)malloc(sizeof(struct tag_data));
//	memset(node->data,0,sizeof(node));
	return node;
}



void add_update_node(unsigned char *bdid, struct tag_data* data)
{


	struct tag *node =find_node(bdid);
//	char print_data[100];

	write_lock();
	if(node==NULL)
	{
		node= create_node();
		node->next=start;
		start=node;

		strncpy(node->bdid,bdid,strlen(bdid));
		node->bdid[strlen(bdid)]='\0';
//		sprintf(print_data,"bdid_len:%d  bdid_len_original:%d   bdid:%s\n",strlen(bdid),strlen(node->bdid),bdid);
//		log_str(print_data);
	}
	else
	{
		log_str("\nin update block");
	}
	node->data.battery_percent=data->battery_percent;
	node->data.temperature=data->temperature;
	strcpy(node->data.device_name,data->device_name);
	strcpy(node->data.event_type,data->event_type);
	notify(node);
	unlock();

//	free(data);
}

struct tag * find_node(unsigned char *bdid)
{
	struct tag *temp;

	read_lock();
	for(temp=start;temp!=NULL;temp=temp->next)
	{
		if(!strcmp(temp->bdid,bdid))
		{
			log_str("Got node\n");
			break;
		}
	}
	unlock();
	return temp;
}


void delete_node(unsigned char *bdid)
{
	log_str("in delete\n");
	struct tag *node,*prev_node=NULL,*next_node;
	write_lock();
	node=start;
	next_node=node->next;
	while(node!=NULL)
	{
		if(!strcmp(node->bdid,bdid))
		{
			if(prev_node==NULL)   //first node
			{
				start=next_node;
			}
			else if(next_node==NULL)  // last node
			{
				//nothing to do
				prev_node->next=NULL;
			}
			else
			{
				prev_node->next=next_node;
			}
			free(node);
			break;
		}
		prev_node=node;
		node=node->next;
		next_node=node->next;
	}
	unlock();
}

void delete_all_node()
{
	struct tag *current,*next;

	current=start;
	write_lock();
	while(current!=NULL)
	{
		next=current->next;
		free(current);
		current=next;
	}
	start=NULL;
	unlock();
	print_all_node();
}

int get_number_of_connected_devices()
{
	struct tag* node;
	int count=0;
	node=start;
	while(node!=NULL)
	{
	node=node->next;
	count++;
	}
	return count;
}

char* get_all_node_string()
{
	struct tag* node;
	int dev_count=get_number_of_connected_devices();
	char * device_list=malloc((dev_count*sizeof(char))+dev_count);
	node=start;
	while(node!=NULL)
	{
		strcat(device_list,node->bdid);
		strcat(device_list,",");
		node=node->next;
	}

	return device_list;
}

void print_all_node()
{
	struct tag* node;
	char * print_tty = malloc(150*sizeof(char));

	read_lock();
	log_str("\n\n\t\tTAG INFO\n\n");
	node=start;
	while(node!=NULL)
	{
		sprintf(print_tty,"\n\nID:%s\nID_LEN:%d\nNAME:%s\nEVENT NAME:%s\nBATTERY:%d\nTEMPERATURE:%d",node->bdid,strlen(node->bdid),node->data.device_name,node->data.event_type,node->data.battery_percent,node->data.temperature);
		log_str(print_tty);
		log_str(":\n\n");

		node=node->next;
	}
	unlock();
	free(print_tty);
}

#ifdef LIST_TEST
int  main()
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


	log_str("deleting .....");

	delete_node("20CD39848D17");

	print_all_node();

	delete_all_node();

	free(data);
	return 0;

}
#endif
