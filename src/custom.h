//Initialize the service
void custom_init();
//Read callback
ssize_t read_level(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, u16_t len, u16_t offset);
//Write callback
ssize_t write_level(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, u16_t len, u16_t offset, u8_t flags);

u8_t get_level();
