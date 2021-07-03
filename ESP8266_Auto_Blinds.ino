#include <Servo.h>

#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include <ArduinoOTA.h>

#include "index.h" //Our HTML webpage contents with javascripts
#include "configParams.h"  //secret properties

// Replace with your network credentials
const char* ssid     = SSID;
const char* password = PASSWORD;

// gmail 
const char* user_base64 = USER_BASE64; // https://www.base64encode.org/
const char* user_password_base64 = USER_PASSWORD_BASE64; 
const char* from_email = FROM_EMAIL;
const char* to_email = TO_EMAIL;

ESP8266WebServer server(80); // Set web server port number to 80
String header = ""; // Variable to store the HTTP request


const int servoCnt = 3;
Servo myservo[servoCnt];             // create servo object to control a servo
const int servoPin[] = {5,4,0};      // GPIO pins used to connect the servo control (digital out)

const int buttonPin = 2;

bool isServoAttached[servoCnt];
int currPos[servoCnt];

const int moveDelay = 2000;

int moves[] = {10, 90, 170, 90}; // 90 Start Position
int lastpos = 1;

unsigned long ms = 0;
unsigned long lastMoveMillis = 0;

const unsigned int DetachTime = 10000; //detach after 10 seconds

int buttonState;             // the current reading from the input pin
// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 100;    // the debounce time; increase if the output flickers


bool debug = false;

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
void setup()
{
  Serial.begin(115200);

  // initialize servos
  moveAll( moves[1] );

  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);

  // connect to wiFi
  wifiConnect();

  // send email with URL for web page
  sendEmail(WiFi.localIP().toString());
}
///////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
void loop() {

  ms = millis();
  
  doWifi();  //Handle client requests
 
  if ( debounce() ) {     //sample the state of the button - is it pressed or not?
  //if ( digitalRead(buttonPin) == HIGH ){
    nextpos();
    moveAll( moves[lastpos] );
    //delay(moveDelay);    
  }

  //checkDetach();  //ignore, always detach after move
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
void nextpos() {
  lastpos = lastpos + 1;
  if ( lastpos >= ( sizeof(moves) / sizeof(moves[0]) ) ) // sizeof = bytes, so total / single = units
    lastpos = 0;

  if ( debug )
    Serial.println("Button Press");
}

void moveServo(int srv, int myval) {
  noInterrupts();                //block other interrupt from distorting signal
  //Serial.println(myval);
  myservo[srv].write(myval);     // set the servo position degrees
  
  if ( !isServoAttached[srv] ) {
    // using SG90 servo min/max of 500us and 2400us for MG995 large servo, use 1000us and 2000us,
    //myservo[srv].setPeriodHertz(50);
    myservo[srv].attach(servoPin[srv], 500, 2500);
    isServoAttached[srv] = true;
  }
  interrupts();               //restore interrupt ability
  
  delay(moveDelay);           // wait a bit extra for the servo to get there then detach
  detachServo(srv);  
  
  currPos[srv] = myval;
  lastMoveMillis = ms;
}

void moveAll(int myval) {
  for (int i = 0; i < servoCnt; i++){
    moveServo(i, myval);
  }  
}

void detachServo(int srv) {
    myservo[srv].detach();
    isServoAttached[srv] = false;
}
void detachServos() {
  for (int i = 0; i < servoCnt; i++){
    detachServo[i];    
  }  
  if ( debug )   
    Serial.println("**Detached Servos");
}
void checkDetach() {
  if ( isServoAttached && ms - lastMoveMillis > DetachTime )
    detachServos();
}

////////////////////////////////////////////////////////////////////////////////////
bool debounce(){
  //sample the state of the button - is it pressed or not?
  buttonState = digitalRead(buttonPin);

  //filter out any noise by setting a time buffer
  //So lets say previousMillis is 4,294,967,290 (5 ms before rollover), and currentMillis is 10 (10ms after rollover). 
  //Then currentMillis - previousMillis is actual 16 (not -4,294,967,280) since the result will be calculated as an UNSIGNED long (which can't be negative, so itself will roll around).
  if ( (ms - lastDebounceTime) > debounceDelay ) {  

    //if the button has been pressed, lets toggle the state from "off to on" or "on to off"
    if ( buttonState == HIGH ) {
      lastDebounceTime = ms; //set the current time
      return true; 
    }
    return false;
  } 
  return false;
}
////////////////////////////////////////////////////////////////////////////////////
void wifiConnect() {
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  OTASetup();

  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");  Serial.println(WiFi.localIP());
  //server.begin();

  /* Set up mDNS *
  if (!MDNS.begin("blinds")) {
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  */

  server.on("/", handleRoot);      //Which routine to handle
  //at root location. This is display page
  server.on("/setTILT", handleTILT);
  server.on("/customTILT", customTILT);

  /* Start Web Server server */
  server.begin();
  Serial.println("Web server started");

  /* Add HTTP service to MDNS-SD */
  MDNS.addService("http", "tcp", 80);

}


void OTASetup() {
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)          Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)    Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR)  Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)  Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)      Serial.println("End Failed");
  });
  ArduinoOTA.begin();
}

//////////////////////////////////////////////////////////////////////////////////
//===============================================================
// This routine is executed when you open its IP in browser
//===============================================================
void handleRoot() {
  String page = MAIN_page; //Read HTML contents
  server.send(200, "text/html", page); //Send web page
}

void handleTILT() {
  String tiltSt = "OPEN";
  String t_state = server.arg("TILTstate"); //Refer  xhttp.open("GET", "setTILT?TILTstate="+val, true);
  Serial.println(t_state);

  // turns blinds
  if (t_state == "180") {
    Serial.println("Tilt UP");
    tiltSt = "UP";
    lastpos = 2;
    moveAll(moves[lastpos]);
  } else if (t_state == "0") {
    Serial.println("Tilt DOWN");
    tiltSt = "DOWN";
    lastpos = 0;
    moveAll(moves[lastpos]);
  } else {
    Serial.println("Tilt OPEN");
    tiltSt = "OPEN";
    lastpos = 1;
    moveAll(moves[lastpos]);
  }

  server.send(200, "text/plane", tiltSt); //Send web page
}

void customTILT(){
  String tiltSt = "CUSTOM";
  String tservo = server.arg("Tnum");
  String t_state = server.arg("Tstate"); //Refer  xhttp.open("GET", "customTILT?Tstate="+val, true);
  Serial.print(tservo);
  Serial.println(t_state);

  if ( tservo == "TiltA" ){
    moveServo(0, t_state.toInt());
  } else if ( tservo == "TiltB" ){
    moveServo(1, t_state.toInt());
  } else if ( tservo == "TiltC" ){
    moveServo(2, t_state.toInt());
  }
  
  server.send(200, "text/plane", tiltSt); //Send web page
}
/////////////////////////////////////////////////////////////////////////////////////////

void doWifi() {
  server.handleClient();          //Handle client requests
  ArduinoOTA.handle();
}

//////////////////////////////////////////////////////////////////////////////////////////

byte sendEmail(String IP) {
  WiFiClientSecure client;
  client.setInsecure();
  if (client.connect("smtp.gmail.com", 465) == 1) //645 = SSL, 587 = TLS
    Serial.println(F("connected"));
  else {
    Serial.println(F("connection failed"));
    return 0;
  }

  if (!eRcv(client)) return 0;
  Serial.println(F("--- Sending EHLO"));              client.println("EHLO 1.2.3.4");         if (!eRcv(client)) return 0;
  Serial.println(F("--- Sending login"));             client.println("AUTH LOGIN");           if (!eRcv(client)) return 0;
  Serial.println(F("--- Sending User base64"));       client.println(user_base64);            if (!eRcv(client)) return 0;
  Serial.println(F("--- Sending Password base64"));   client.println(user_password_base64);   if (!eRcv(client)) return 0;
  Serial.println(F("--- Sending From"));              client.println(from_email);             if (!eRcv(client)) return 0;
  Serial.println(F("--- Sending To"));                client.println(to_email);               if (!eRcv(client)) return 0;
  Serial.println(F("--- Sending DATA"));              client.println(F("DATA"));              if (!eRcv(client)) return 0;
  client.println(F("Subject: ESP8266 Blinds IP\r\n"));
  client.println(F("******Play with the blinds at:\n"));
  String IPLink = "http://" + IP + "  Make sure it's HTTP, no S";
  client.println(IPLink);
  client.println(F("."));
  if (!eRcv(client)) return 0;
  Serial.println(F("--- Sending QUIT"));              client.println(F("QUIT"));
  if (!eRcv(client)) return 0;

  client.stop();
  return 1;
}

byte eRcv(WiFiClientSecure client)
{
  byte respCode;
  byte thisByte;
  int loopCount = 0;

  while (!client.available()) {
    delay(1);
    loopCount++;
    if (loopCount > 10000) {
      client.stop();
      Serial.println(F("\r\nTimeout"));
      return 0;
    }
  }

  respCode = client.peek();
  while (client.available())
  {
    thisByte = client.read();
    Serial.write(thisByte);
  }
  if (respCode >= '4')return 0;
  return 1;
}
