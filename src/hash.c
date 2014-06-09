

#include "uthash.h"
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include "hash.h"



dev *deviceTable = NULL;
pthread_rwlock_t lock;
static short LOCK_INIT=0;
static void init_lock()
{
	if (pthread_rwlock_init(&lock,NULL) != 0) log_str("can't create rwlock");
	else LOCK_INIT=1;
}
void add_user(struct device *s)
{
	if(!LOCK_INIT)
		init_lock();
	log_str("in add device");
//	HASH_ADD_STR(deviceTable,bdid,s);
	if (pthread_rwlock_wrlock(&lock) != 0) log_str("can't get wrlock");
	HASH_ADD_KEYPTR( hh, deviceTable, s->bdid, strlen(s->bdid), s );
	pthread_rwlock_unlock(&lock);
}

struct device *find_user(char * key) {


	if(!LOCK_INIT)
		init_lock();
    struct device *s;
    int len = 12;
    char * p = malloc(len*sizeof(char));
    memcpy(p,key,len);
    if (pthread_rwlock_rdlock(&lock) != 0) log_str("can't get rdlock");
    log_str("\nsearching key :");
    log_str(p);
    log_str("\n");
    HASH_FIND_STR( deviceTable,p, s );
    pthread_rwlock_unlock(&lock);
    printf("\nfind addr : %p\n",s);
    free(p);
    return s;
}

void delete_user(char * bdid) {
	if(!LOCK_INIT)
		init_lock();
	struct device *s = find_user(bdid);
	if(s!=NULL)
    {
		HASH_DEL( deviceTable, s);
		free(s);
    }

}

void delete_all() {
	if(!LOCK_INIT)
		init_lock();
  struct device *current_user, *tmp;

  HASH_ITER(hh, deviceTable, current_user, tmp) {
    HASH_DEL(deviceTable,current_user);  /* delete it (users advances to next) */
    log_str("in delee all");
    free(current_user->data);
    free(current_user);            /* free it */
  }
}

void print_users() {
    struct device *s;
    char  print_statement[150];
    log_str("\nDEVICE INFO");
    for(s=deviceTable; s != NULL; s=(struct device*)(s->hh.next)) {
        sprintf(print_statement," \r\nDevice id :%s:  , Battery :%d ,Temperature :%d ,Name :%s ,Event Name :%s \n", s->bdid, s->data->battery_percent,s->data->temperature,s->data->device_name,s->data->event_type);
        log_str(print_statement);
    }
    log_str("\n print users");
}

