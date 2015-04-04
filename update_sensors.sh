SENSORS=sensor_*
for sensor in $SENSORS; do cp sensor_template/sensor_template.ino $sensor/$sensor.ino; git add $sensor/$sensor.ino; cp sensor_template/constants.h $sensor/constants.h; git add $sensor/constants.h; done
