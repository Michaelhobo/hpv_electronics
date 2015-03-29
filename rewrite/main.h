/* RF24 Handlers. They must all take in a char* parameter. */
void receiver_handler(char *data);
void speed_handler(char *data);
void cadence_handler(char *data);
void rear_lights_handler(char *data);
void front_lights_handler(char *data);
void shifter_handler(char *data);

uint8_t send_sensor(uint8_t id, char *data);
void process_rf_input();
void process_connection();

void init();
void buttons_init();
void telemetry_init();
void rf24_init();

uint8_t find_id(char *name);
void send_sensor_name(char *name, char *data);
