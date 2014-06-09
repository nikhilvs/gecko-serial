//
//
//#include "uthash.h"
//#include <string.h>
//#include <stdio.h>
//#include "hash.h"
//
//struct device {
//    short temperature;
//    short battery_percent;
//    unsigned char event_type[15];
//    unsigned char device_name[5];
//    unsigned char bdid[12]; /* we'll use this field as the key */
//    UT_hash_handle hh; /* makes this structure hashable */
//};
//
//struct device *deviceTable = NULL;
//
//static void add_user(struct device *s) {
////    struct device *s;
////
////    s = malloc(sizeof(struct device));
////    s->id = user_id;
////    strcpy(s->bdid, name);
//    HASH_ADD_STR( deviceTable,bdid, s );  /* id: name of key field */
//}
//
//static  struct device *find_user(char * bdid) {
//    struct device *s;
//
//    HASH_FIND_STR( deviceTable, bdid, s );
//    return s;
//}
//
//static  void delete_user(struct device *user) {
//    HASH_DEL( deviceTable, user);
//    free(user);
//}
//
//
//void hash_example(){
//
//
////	add_user(123,"hash1");
////	add_user(234,"hash2");
////	add_user(12356,"hash1");
////	struct device * dev = find_user("hash1");
////	if(dev != NULL)
////		printf("key : %d , value : %s \r\n", dev->id,dev->bdid);
////	else
////		printf("dev is null");
//////	printf("\nPlease start with %s \n", port);
////	dev = find_user("hash2");
////	if(dev != NULL)
////		printf("key : %d , value : %s \r\n", dev->id,dev->bdid);
////	else
////		printf("dev is null");
//
//
//}
//
