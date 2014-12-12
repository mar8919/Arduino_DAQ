Arduino_DAQ
Written By: Christopher Mar
===========
The files in this repository are being used for a data aquisition system running on an Arduino Yun.  It contains two
seperate parts.  The first is an Arduino sketch uploaded to the Yun's processor.  The second is an HTML website which displays a
user interface via a web browser.

AJAX_SERVER:

- Folder containing the Arduino sketch
- Reads data from analog inputs
- Control digital outputs
- Responds to client REST requests used to control the Arduino
	
www:

 - Folder must be placed in the root directory of the SD card used in the Yun
 - Contains files for the website interface
		
In order to access the website your computer must be connected to the same local network as the Arduino (either WiFi or ethernet).
The default URL for the website is:	http://arduino.local/sd/

For our needs we are using the Yun as a ethernet access point so it has a static IP.
Its website URL is: http://192.168.240.1/sd/
