# ESP8266_Auto_Blinds  Arduino project.  

ESP8266 Node MCU - Blinds tilt controller for a wall with three large windows.

ESP8266 has three MG995 servos hooked up to some 3D printed gears attached to the tilt bar inside the blinds.

There is also a single button on the case that increments the blinds tilt position [Up, Center/Open, Down]

ESP8266 connects to wifi and hosts a simple webpage that allows for manual control of positions via three buttons and individual sliders

On startup, the ESP8266 also checks its IP address and sends an email with a URL to the webpage (my router was occasionally changing the IP address handed out).  

```

#include <Servo.h> 
#include <ESP8266mDNS.h> 
#include <ESP8266WiFi.h> 
#include <WiFiClient.h> 
#include <ESP8266WebServer.h> 
#include <ArduinoOTA.h> 
 
#include "index.h" //Our HTML webpage contents with javascripts 
#include "configParams.h"  //secret properties 

```
