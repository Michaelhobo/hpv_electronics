#!/bin/bash
# Update the base for each sensor
# Usage: update_base.sh <sensor_folder>
# Note: <sensor_folder> must contain a sensor_template.ino.bak file
# and the ino file must be the same as the sensor_folder name.
# Example: ./update_base.sh sensor_speed
# In the above example, the sensor_speed folder must have sensor_speed.ino as the main program file
#
# Note: You may need to manually fix any git merge issues
# Note: make sure you commit your changes before you update_base!
git merge-file ../$1/$1.ino ../$1/sensor_template.ino.bak sensor_template.ino
cp sensor_template.ino ../$1/sensor_template.ino.bak
