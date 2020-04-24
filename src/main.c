#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <sys/byteorder.h>
#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#include "custom.h"

//Anything marked CONFIG is from the proj.conf file
//It allows us to define things and change settings,
//without going into the code.
#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

//GPIO Related things
struct device *dev;
//(Not actually currently using these)
#define LED_PORT	DT_ALIAS_LED0_GPIOS_CONTROLLER
#define LED		DT_ALIAS_LED0_GPIOS_PIN


//Default connection
struct bt_conn *default_conn;

//Advertising data
//I know theres a lot of random pre-defines, 
//use google to find out what they mean.
static const struct bt_data ad[] = {
		BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
		BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

//The connected callback, what happens when something attempts to connect to this device
static void connected(struct bt_conn *conn, u8_t err)
{
	//Turn on the LED
	//Strangely enough, 0 means on.
	gpio_pin_write(dev, 9, 0);
	if (err)
	{
		//Do nothing
	}
	else
	{
		//Establish our connection
		default_conn = bt_conn_ref(conn);
	}
}

//Our disconnection callback.
static void disconnected(struct bt_conn *conn, u8_t reason)
{
	//Turn off the LED
	gpio_pin_write(dev, 9, 1);

	if (default_conn){
		//Disconnect 
		bt_conn_unref(default_conn);
		default_conn = NULL;
	}
}

//The struct used to hold our callbacks
static struct bt_conn_cb conn_callbacks = {
		.connected = connected,
		.disconnected = disconnected,
};

static void bt_ready(int err)
{
	if (err){return;}

	//Initialize our custom service
	custom_init();

	//Make sure our advertising starts and starts correctly.
	err = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err){return;}
}

void main(void)
{
	//Establish the device
	//(I'm pretty sure pin 9 falls under GPIO 1, just check the device tree)
	dev = device_get_binding("GPIO_1"); 

	//Configure the pin
	//The pins are just numbered, nothing much fancier than that
	//In this case, I'm setting up pin 9 for output
	//Check the devicetree.conf in build/zephyr/include/generated/ for all your alias/gpio needs.
	gpio_pin_configure(dev, 9, GPIO_DIR_OUT); 
	
	//Enable bluetooth, passing the bt_ready function to it
	int err = bt_enable(bt_ready); 
	if (err){return;}

	//Register callbacks
	bt_conn_cb_register(&conn_callbacks); 
	//Turn off LED
	gpio_pin_write(dev, 9, 1);
}
