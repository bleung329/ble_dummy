/* main.c - Application main entry point */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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

//Anything marked CONFIG is put into the conf file
#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

//GPIO Related things
struct device *dev;
#define LED_PORT	DT_ALIAS_LED0_GPIOS_CONTROLLER
#define LED		DT_ALIAS_LED0_GPIOS_PIN

struct bt_conn *default_conn;

//Theres a lot of random pre-defines, use google to find out what they mean.
static const struct bt_data ad[] = {
		BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
		BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

static void connected(struct bt_conn *conn, u8_t err)
{
	gpio_pin_write(dev, 9, 0);
	if (err)
	{
		//Do nothing
	}
	else
	{
		default_conn = bt_conn_ref(conn);
	}
}

static void disconnected(struct bt_conn *conn, u8_t reason)
{
	gpio_pin_write(dev, 9, 1);
	if (get_level() > 0)
	{
		// handle link loss per the alert level
	}

	if (default_conn){
		bt_conn_unref(default_conn);
		default_conn = NULL;
	}
}

static struct bt_conn_cb conn_callbacks = {
		.connected = connected,
		.disconnected = disconnected,
};

static void bt_ready(int err)
{
	if (err){return;}

	custom_init();

	err = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err){return;}
}

void main(void)
{
	//Check the devicetree.conf in build/zephyr/include/generated/ for all your alias/gpio needs.
	dev = device_get_binding("GPIO_1"); //Establish the device
	gpio_pin_configure(dev, 9, GPIO_DIR_OUT); //Configure the pin

	int err = bt_enable(bt_ready); 
	if (err){return;}

	bt_conn_cb_register(&conn_callbacks); 
	gpio_pin_write(dev, 9, 1);
}
