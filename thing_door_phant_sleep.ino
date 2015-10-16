// board config from http://arduino.esp8266.com/versions/1.6.5-947-g39819f0/package_esp8266com_index.json
// (not using the sparkfun one cuz EEPROM problems have been reported with it)
//  Board:      Generic ESP Module
//  Flashmode:  DIO
//  Flash Freq: 80MHz
//  CPU Freq:   80MHz
//  Flash Size: 512K (64K SPIFFS)

//ESP8266 board: Sparkfun 'Thing' from sparkfun.com
//Phant lib from sparkfun.com

// GPL 3
// 4 evar

#include <ESP8266WiFi.h>
#include <Phant.h>
#include <EEPROM.h>

// WiFi Definitions //
const char WiFiSSID[] = "___________";
const char WiFiPSK[] = "___________";

// Pin Definitions //
const int ESP8266_LED = 5;
const int DOOR_SENSOR = 4;

// Phant //
const char PhantHost[] = "data.sparkfun.com";
const char PublicKey[] = "OGzpoQN9bAir9gEj5NDE";      //chicken door
const char PrivateKey[] = "___________________";
Phant phant(PhantHost, PublicKey, PrivateKey);

// Door State  //
int last_door_state;
int new_door_state;

// Board Sleep //
int sleepTimeS = 15;

void setup() 
{
  initHardware();
  checkDoor();
}

void loop(){}                                         //Loop is sleep/wakeup/restart

void checkDoor(){
  new_door_state = digitalRead(DOOR_SENSOR);          // read door state
  EEPROM.begin(4);
  last_door_state = EEPROM.read(0);

  if( new_door_state != last_door_state ){            // check if state changed
    flashLED(ESP8266_LED,1);                          // one flash before connect starts
    connectWiFi();                                    // Connect & Post
    if( new_door_state == LOW ){                      // closing
      postToPhant(1);
    } else{                                           // opening
      postToPhant(0);
    }
    EEPROM.write(0, new_door_state);
    disconnectWiFi();
  }
  EEPROM.end();

  ESP.deepSleep(sleepTimeS * 1000000);
}

void initHardware()
{
  pinMode(ESP8266_LED, OUTPUT);
  pinMode(DOOR_SENSOR, INPUT_PULLUP);                 // LOW = closed; HIGH = open
}

int postToPhant(bool door_closed)
{
  phant.add("door_closed", door_closed);

  WiFiClient client;
  const int httpPort = 80;

  if (!client.connect(PhantHost, httpPort)) 
  {
    flashLED(ESP8266_LED,20);                         // 20 flashes = PROBLEM
    return 0;                                         // failed to connect
  }

  client.print(phant.post());                         // build post & send

  flashLED(ESP8266_LED,2);                            // two flash on success
  return 1;                                           // Return success
}

void connectWiFi()
{
  WiFi.mode(WIFI_STA);                                // Set WiFi mode to station
  WiFi.begin(WiFiSSID, WiFiPSK);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(10);                                        // Perform outside-sketch util tasks
  }
}

void disconnectWiFi()
{
  WiFi.disconnect();
}

void flashLED(int led, int times){
  for(int i = 0; i<times; i++){
    digitalWrite(led, HIGH);
    delay(100);
    digitalWrite(led, LOW);
    if(times - 1 != i){
      delay(100);
    }
  }
}
