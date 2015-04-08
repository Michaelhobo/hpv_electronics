SENSORS=sensor_*
for sensor in $SENSORS; do cp sensor_template/sensor_template.ino $sensor/$sensor.ino; git add $sensor/$sensor.ino; cp sensor_template/constants.h $sensor/constants.h; git add $sensor/constants.h; cp sensor_template/user_functions.cpp $sensor/user_functions.cpp; done
ACTUATORS=actuator_*
for actuator in $ACTUATORS; do cp actuator_template/actuator_template.ino $actuator/$actuator.ino; git add $actuator/$actuator.ino; cp actuator_template/constants.h $actuator/constants.h; git add $actuator/constants.h; cp actuator_template/user_functions.cpp $actuator/user_functions.cpp; done
