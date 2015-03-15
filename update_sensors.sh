SENSORS=sensor_*
for sensor in $SENSORS; do cp sensor_template/sensor_template.ino $sensor/$sensor.ino; git add $sensor/$sensor.ino; done
