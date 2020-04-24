#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr.h>
#include <sys/byteorder.h>


#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#include "custom.h"

//Every service and characteristic gets a UUID.
//The UUIDs you give it can be arbitrary to be honest.
//The bluetooth standard already has a bunch defined though,
//so double check to make sure youre setting a UUID that isn't
//way out of line.
#define BT_UUID_LINK_LOSS BT_UUID_DECLARE_16(0x1802)
#define BT_UUID_ALERT_LEVEL BT_UUID_DECLARE_16(0x2A06)

//The data this service is holding.
//It'll stick around.
u8_t link_loss_alert_level = 0;

//This defines our service.
BT_GATT_SERVICE_DEFINE(custom_service,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_LINK_LOSS),

    //You define the read and write callbacks + permissions here, in the characteristic.
	BT_GATT_CHARACTERISTIC(BT_UUID_ALERT_LEVEL, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
											    BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, read_level, write_level, NULL),
);

void custom_init()
{
}

//READ
ssize_t read_level(struct bt_conn *conn,
																	 const struct bt_gatt_attr *attr, void *buf,
																	 u16_t len, u16_t offset)
{
	return bt_gatt_attr_read(conn, attr, buf, len, offset, &link_loss_alert_level, sizeof(link_loss_alert_level));
}

//WRITE
ssize_t write_level(struct bt_conn *conn,
																		const struct bt_gatt_attr *attr,
																		const void *buf, u16_t len, u16_t offset,
																		u8_t flags)
{
	//Mainly all left over from the project I dissected.
	const u8_t *new_alert_level = buf;

	if (!len)
	{
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
	}
	if (*new_alert_level >= 0 && *new_alert_level <= 2)
	{
		link_loss_alert_level = *new_alert_level;
	}
	else
	{
		return BT_GATT_ERR(BT_ATT_ERR_VALUE_NOT_ALLOWED);
	}

	return len;
}

u8_t get_level()
{
	return link_loss_alert_level;
}