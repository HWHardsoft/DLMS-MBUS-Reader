# DLMS-MBUS-Reader

Arduino MBUS Reader demo for electricity meters with DLMS COSEM interface (as they are common in Austria, for example). Demo code for our  MBUS slave Arduino MKR boards and Feather Boards (including ESP32, RP2040, nRF52840, Cortex M0 ....).


![Example Data output of an connected electricity meter](https://user-images.githubusercontent.com/3049858/263453247-d6c13182-0374-48ee-8505-16257cd2addd.jpg)



## Hardware 

Electricity meters with an MBUS interface include a rudimentary MBUS master. In order to be able to read the meter, an MBUS slave is required. I've used our MBUS Slave wing for feather borads and/or MBUS slave shield for Arduino MKR boards

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


## Remarks

The code was tested with Kaifa MA309M and Sagemcom T210-D (both data Lower Austria). In other countries and regions (e.g. Vorarlberg, Austria) the structure of the user data may differ and an adjustment of the program may be necessary.



## Libraries

Install the following library through Arduino Library Manager

[Arduino Crypto Library](https://rweather.github.io/arduinolibs/crypto.html) by Rhys Weatherley 
                         


## Credits 

Based on [ESP32/8266 Smartmeter decoder for Kaifa MA309M](https://github.com/FKW9/esp-smartmeter-netznoe/) code by FKW9, EIKSEUand others
   


# License

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.
