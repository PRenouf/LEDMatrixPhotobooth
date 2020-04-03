# LED MATIX PHOTOBOOTH
![Image of Assembly](/Images/ASM1.jpg)

The LED Matrix Photobooth Project adds a remote trigger and countdown screen to a DSLR camera. The Camera can also be connected to a TV Screen or monitor, where the DSLR camera supports this feature, to display the taken image.

A microcontroller, connected to the LED Matrix, triggers the camera by connecting different channels of the cameras trigger wire - a 1.5mm single or double ended headphone jack. 

The remote trigger communicates with the main microcontroller by connecting to its local AP and sending HTML requests. Although alternative communication over BLE or 433MHz RF would have perhaps been better, microcontrollers supporting WIFI were easily available. The chosen microcontroller comes complete with battery management systems for the 18650-cell powering the device.

## The Hub – Main Microcontroller
The main microcontroller, an ESP8266 based WEMIS D1 MINI, is configured as a local-AP and hosts a webserver.
During operation the microcontroller displays a flashing animation. When taking a photo, the numbers 3,2,1 followed by a smiley faced are overlaid onto the matrix.

![ExampleNumber](/Images/ExampleNumber.jpg)

###  Triggering Protocol
To avoid damaging the DSLR Camera there is no electrical connection between the main microcontroller and camera trigger wire. Signals are transferred utilising optocouplers.
The exact protocol used to trigger the camera may depend on brand and model – for Cannon cameras and this instance:
1.	To focus the camera connect Ring to Sleeve.
2.	To take a photo connect the Tip to Sleeve.  
When using a double ended headphone, the wire from the bottom ring is connected to the sleeve.

### Schematic
![Schematic](/Images/MatrixSch.jpg)

## Remote Trigger - Secondary Microcontroller
![Schematic](/Images/Trigger2.jpg)

The remote trigger is Adafruit feather HUZZAH ESP8266, it connects to the AP provided by the HUB, illuminating a red LED is there is a problem. Every 1 second a HTML request is sent to the server alerting it that the trigger is in range and connected, on a button press a different request is sent and the Hub is asked to take a photo. 
The button is connected directly to the microcontroller using an internal pullup resistor. 

 
