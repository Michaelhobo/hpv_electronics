/* RF24 Handlers. They must all take in a char* parameter. */
uint8_t send_sensor(uint8_t id, char *data);

void init();
void buttons_init();
void telemetry_init();

uint8_t find_id(char *name);
void send_sensor_name(char *name, char *data);

