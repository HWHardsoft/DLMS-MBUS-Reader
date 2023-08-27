# DLMS-MBUS-Reader

Arduino MBUS Reader demo for electricity meters with DLMS COSEM interface (as they are common in Austria, for example). Demo code for our  MBUS slave Arduino MKR boards and Feather Boards (including ESP32, RP2040, nRF52840, Cortex M0 ....).


![Example Data output of an connected electricity meter](https://user-images.githubusercontent.com/3049858/263540720-a5ca355f-043a-423e-a036-43d0d01d43e5.jpg)



## Hardware 

Electricity meters with an MBUS interface include a rudimentary MBUS master. In order to be able to read the meter, an MBUS slave is required. I've used our MBUS Slave wing for feather boards and/or MBUS slave shield for Arduino MKR boards

![MBUS Slave Wing](https://user-images.githubusercontent.com/3049858/263453697-6a00bde5-259d-4733-a12a-3dff900e32d1.jpg)  ![MBUS Slave shield for Arduino MKR](https://user-images.githubusercontent.com/3049858/263453696-eaf3f158-7afa-4ac2-a786-6002ce8581bb.jpg)

You can find all information about the hardware here:
https://www.hwhardsoft.de/english/projects/

The code was tested with the following boards:

* Arduino MKR Wifi 1010
* Adafruit Feather Huzzah32
* Adafruit Feather nRF52840
* Adafruit Feather M0 Express
* Sparkfun ESP32 Thing Plus
* Sparkfun Thing Plus RP2040



## Connection 

On many electricity meters, the MBUS is provided via an RJ12 Jack. The MBUS is brought out on pins 3 and 4. The two MBUS signals must be connected to the terminal block of the MBUS-Slave wing or shield. The polarity does not have to be taken into account since the MBUS on the slave has no polarity.

![RJ12 Pinout](https://user-images.githubusercontent.com/3049858/263541267-3450de44-6f11-47fe-b2e2-5701e3a5a49e.png)



## Key

The user data in the MBUS protocol is encrypted. For decryption you need a corresponding key from your energy supplier. The 16-byte key is usually provided as a hex string. This string must be stored as a byte array in the key.h file. 

For example

key from supplier: 36C66639E48A8CA4D6BC8B282A793BBB

in key.h:

static const unsigned char KEY[] = {0x36, 0xC6, 0x66, 0x39, 0xE4, 0x8A, 0x8C, 0xA4, 0xD6, 0xBC, 0x8B, 0x28, 0x2A, 0x79, 0x3B, 0xBB};



## Remarks

The code was tested with Kaifa MA309M and Sagemcom T210-D (both data Lower Austria). In other countries and regions (e.g. Vorarlberg, Austria) the structure of the user data may differ and an adjustment of the program may be necessary.



## Libraries

Install the following library through Arduino Library Manager

[Arduino Crypto Library](https://rweather.github.io/arduinolibs/crypto.html) by Rhys Weatherley 
                         


## Credits 

Based on [ESP32/8266 Smartmeter decoder for Kaifa MA309M](https://github.com/FKW9/esp-smartmeter-netznoe/) code by FKW9, EIKSEU and others
   


# License

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.
