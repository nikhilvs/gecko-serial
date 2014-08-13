/*
 * config.h
 *
 *  Created on: Aug 13, 2014
 *      Author: nikhilvs9999
 */

#ifndef CONFIG_H_
#define CONFIG_H_





#define  GPIO_EXPORT_FILE  "/sys/class/gpio/export"
#define  GPIO_UNEXPORT_FILE  "/sys/class/gpio/unexport"
#define  GPIO_DIRECTION_FILE  "/sys/class/gpio/gpio3_pg4/direction"
#define  GPIO_VALUE_FILE  "/sys/class/gpio/gpio3_pg4/value"
#define  RESTART_SLEEP     2*1000*1000
#define  SET_LOW_SLEEP     2*1000*1000
#define  GPIO_PORT   3
#define  MAX_BUF  15
#define  OUT  1
#define  IN   0
#define  LOW  0
#define  HIGH  1


int do_power_reset();
int initialize_gecko_gpio();

#endif /* CONFIG_H_ */
