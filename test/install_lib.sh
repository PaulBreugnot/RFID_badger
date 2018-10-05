# This shell script allows you to hard link firmware files in your Arduino library repository in order to use them in test sketchs.
# Hard link usage allow you to keep the "library" synchronized with the actual code files in firmware/ if you modify them.
# Your Arduino Sketchbook repository must be specified in input.
# You can find its path in the Arduino IDE, under file -> preferences -> Sketchbook location.

arduino_libraries_path=$1/libraries
mkdir $arduino_libraries_path/RFID_badger
ln -f ../firmware/* $arduino_libraries_path/RFID_badger
