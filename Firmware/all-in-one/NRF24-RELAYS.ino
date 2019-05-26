#define MY_DEBUG

/*Raido*/
#define MY_RADIO_RF24 // Enable and select radio type attached 
#define MY_RF24_PA_LEVEL RF24_PA_MAX //MINI LOW HIGH MAX
//#define MY_RF24_DATARATE RF24_250KBPS
//#define MY_REPEATER_FEATURE
//#define MY_RF24_CHANNEL 2
//#define MY_RADIO_RFM69
#define MY_BAUD_RATE 9600 //115200 19200 9600 =8MHZ  4800 =1MHZ

/*Node ID & NAME*/
//#define MY_NODE_ID 10  //Fallback NodeId
//#define MY_TRANSPORT_UPLINK_CHECK_DISABLED
//#define MY_PARENT_NODE_IS_STATIC //only for clone si24r1
//#define MY_PARENT_NODE_ID 0 //only for clone si24r1
char SKETCH_NAME[] = "Relay Actuator";
char SKETCH_VERSION[] = "1.0";

/*OTA Featuer*/
#define MY_OTA_FIRMWARE_FEATURE
//#define MY_OTA_FLASH_JDECID 0  //0x2020


// Enabled repeater feature for this node  //注释下列启用中继功能
//#define MY_REPEATER_FEATURE

/*Authentication*/
//#define MY_SIGNING_ATSHA204
//#define MY_SIGNING_ATSHA204_PIN 17
//#define  MY_SIGNING_REQUEST_SIGNATURES

/*Child ID*/
/*
  #define CHILD_ID_DOOR 1
  #define CHILD_ID_LIGHT 2 //Light Sensor
  #define CHILD_ID_TEMP 3 //Temperature Sensor
  #define CHILD_ID_HUM 4 //Humidity Sensor
  #define CHILD_ID_BARO 5 //Pressure Sensor
  #define CHILD_ID_UV 6 //UV sensor
  #define CHILD_ID_MOTION 7 //Motion Sensor
*/
#define CHILD_ID_RELAY1 81 //Relay
#define CHILD_ID_RELAY2 82 //Relay
#define CHILD_ID_RELAY3 83 //Relay
/*
  #define CHILD_ID_IR 100 //Lock Sensor
  #define CHILD_ID_LOCK 110 //Lock Sensor
  #define CHILD_ID_HVAC 200 //HVAC Sensor
  #define CHILD_ID_VBAT 254 //BATTERY
*/

/*BATTERY&PIN*/
#define  BATTERY_SENSE_PIN  A1 // battery sensor
#define  LED_PIN  8  //battery  sensor
#define VMIN 1.8
#define VMAX 3.44
#define RELAY1_PIN 7
#define RELAY2_PIN 6
#define RELAY3_PIN 5 // Arduino Digital I/O pin number for first relay (second on pin+1 etc)
#define SWITCH1_PIN A2  // Arduino Digital I/O pin number for first relay (second on pin+1 etc)
#define SWITCH2_PIN A3
#define SWITCH3_PIN A4  // Arduino Digital I/O pin number for first relay (second on pin+1 etc)
#define RELAY_ON 1  // GPIO value to write to turn on attached relay
#define RELAY_OFF 0 // GPIO value to write to turn off attached relay

#define EEPROM_VAR1 11 // SAVE relays num
#define EEPROM_VAR2 21 // SAVE sw/button mode

/*Time Period */
static int16_t battreport = 1;
static int16_t lastVcc = 3.44; //Batteryv
bool state1;
bool state2;
bool state3;
static int16_t var1 = 1 ;
static int16_t var2;
static int16_t lastvalue;
static int16_t value1;
static int16_t value2;
static int16_t value3;
static int16_t oldValue1;
static int16_t oldValue2;
static int16_t oldValue3;
long timeout;
long timewait;
long acttime;
long rfvalue;

#include <SPI.h>
#include <Wire.h>
#include <MySensors.h>
#include <TimeLib.h>
#include <Bounce2.h>

Bounce debouncer1 = Bounce();
Bounce debouncer2 = Bounce();
Bounce debouncer3 = Bounce();

///*Message Instance */
MyMessage msgrelay1(CHILD_ID_RELAY1, V_STATUS);
MyMessage msgrelay2(CHILD_ID_RELAY2, V_STATUS);
MyMessage msgrelay3(CHILD_ID_RELAY3, V_STATUS);
MyMessage msgvar1(S_CUSTOM, V_VAR1); //relays数量
MyMessage msgvar2(S_CUSTOM, V_VAR2); //外接按键switch/button

void blinkity(uint8_t pulses, uint8_t repetitions) {
  for (int x = 0; x < repetitions; x++) {
    for (int i = 0; i < pulses; i++) {
      digitalWrite(LED_PIN, HIGH);
      delay(20);
      digitalWrite(LED_PIN, LOW);
      delay(100);
    }
    delay(100);
  }
}

void before() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  pinMode(SWITCH1_PIN, INPUT);
  pinMode(SWITCH2_PIN, INPUT);
  pinMode(SWITCH3_PIN, INPUT);
  digitalWrite(RELAY1_PIN, loadState(CHILD_ID_RELAY1) ? RELAY_ON : RELAY_OFF);
  digitalWrite(RELAY2_PIN, loadState(CHILD_ID_RELAY2) ? RELAY_ON : RELAY_OFF);
  digitalWrite(RELAY3_PIN, loadState(CHILD_ID_RELAY3) ? RELAY_ON : RELAY_OFF);

  debouncer1.attach(SWITCH1_PIN);
  debouncer1.interval(5);
  debouncer2.attach(SWITCH2_PIN);
  debouncer2.interval(5);
  debouncer3.attach(SWITCH3_PIN);
  debouncer3.interval(5);
}

void setup() {
  analogReference(INTERNAL);
  if (transportCheckUplink() == false) {
    blinkity(4, 2);
  }
  if (isTransportReady() == true)  {
    blinkity(2, 1);
  }

  var1 = loadState(EEPROM_VAR1); //读取var1值为motion[2]/switch[1]
  var2 = loadState(EEPROM_VAR2); //读取var1值为motion[2]/switch[1]

  loadState(CHILD_ID_RELAY1);
  loadState(CHILD_ID_RELAY2);
  loadState(CHILD_ID_RELAY3);
  send(msgrelay1.set(loadState(CHILD_ID_RELAY1) ? RELAY_ON : RELAY_OFF));
  send(msgrelay2.set(loadState(CHILD_ID_RELAY2) ? RELAY_ON : RELAY_OFF));
  send(msgrelay3.set(loadState(CHILD_ID_RELAY3) ? RELAY_ON : RELAY_OFF));
  send(msgvar1.set(var1));
  send(msgvar2.set(var2));
  requestTime();
}

void presentation() {
  sendSketchInfo(SKETCH_NAME, SKETCH_VERSION);
  wait(50);
  present(CHILD_ID_RELAY1, S_BINARY, "Relay1" );
  if (var1 == 2) {
    present(CHILD_ID_RELAY2, S_BINARY, "Relay2" );
  }
  if (var1 == 3) {
    present(CHILD_ID_RELAY3, S_BINARY, "Relay3" );
  }
  present(S_CUSTOM, V_VAR1, "Relays");
  present(S_CUSTOM, V_VAR2, "Mode");
}

void loop()
{
  //SWitch模式操作
  if (var2 == 0) {
    debouncer1.update();
    int value1 = debouncer1.read();
    if (var1 == 2) {
      debouncer2.update();
      int value2 = debouncer2.read();
    }
    if (var1 == 3) {
      debouncer3.update();
      int value3 = debouncer3.read();
    }
    // Get the update value


    if (value1 != oldValue1 ) {
      changeState(CHILD_ID_RELAY1, state1 ? RELAY_OFF : RELAY_ON);
    }
    oldValue1 = value1;

    if (var1 == 2) {
      if (value2 != oldValue2 ) {
        changeState(CHILD_ID_RELAY2, state2 ? RELAY_OFF : RELAY_ON);
      }
      oldValue2 = value2;
    }

    if (var1 == 3) {
      if (value3 != oldValue3 ) {
        changeState(CHILD_ID_RELAY3, state3 ? RELAY_OFF : RELAY_ON);
      }
      oldValue3 = value3;
    }
  }

  //button模式操作
  if (var2 == 1 ) {
    debouncer1.update();
    int value1 = debouncer1.read();
    if (var1 == 2) {
      debouncer2.update();
      int value2 = debouncer2.read();
    }
    if (var1 == 3) {
      debouncer3.update();
      int value3 = debouncer3.read();
    }
    // Get the update value


    if (value1 != oldValue1 && value1 == 0) {
      changeState(CHILD_ID_RELAY1, state1 ? RELAY_OFF : RELAY_ON);
    }
    oldValue1 = value1;

    if (var1 == 2) {
      if (value2 != oldValue2 && value2 == 0 ) {
        changeState(CHILD_ID_RELAY2, state2 ? RELAY_OFF : RELAY_ON);
      }
      oldValue2 = value2;
    }

    if (var1 == 3) {
      if (value3 != oldValue3 && value3 == 0) {
        changeState(CHILD_ID_RELAY3, state3 ? RELAY_OFF : RELAY_ON);
      }
      oldValue3 = value3;
    }
  }
  timeupdate();
}


void changeState(int childId, int newState) {
  switch (childId) {
    case CHILD_ID_RELAY1:
      digitalWrite(RELAY1_PIN, newState);
      state1 = newState;
      saveState(CHILD_ID_RELAY1, state1);
      wait(20);
      send(msgrelay1.set(newState));
      break;
    case CHILD_ID_RELAY2:
      digitalWrite(RELAY2_PIN, newState);
      state2 = newState;
      saveState(CHILD_ID_RELAY2, state2);
      wait(20);
      send(msgrelay2.set(newState));
      break;
    case CHILD_ID_RELAY3:
      digitalWrite(RELAY3_PIN, newState);
      state2 = newState;
      saveState(CHILD_ID_RELAY3, state3);
      wait(20);
      send(msgrelay3.set(newState));
      break;
    default:
      break;
  }
}

void receive(const MyMessage &message) {
  // We only expect one type of message from controller. But we better check anyway.
  if (message.type == V_STATUS && message.sensor != 7  && !mGetAck(message)) {
    changeState(message.sensor, message.getBool() ? RELAY_ON : RELAY_OFF);
  }
  if (message.type == V_VAR1  && message.sensor == 23) {
    var1 = atoi(message.data);
    saveState(EEPROM_VAR1, var1);
    send(msgvar1.set(var1));
  }
  if (message.type == V_VAR2  && message.sensor == 23) {
    var2 = atoi(message.data);
    saveState(EEPROM_VAR1, var2);
    send(msgvar2.set(var2));
  }
}

void receiveTime(unsigned long ts) {
  setTime(ts);
}

void timeupdate() {
  if ((hour() == 0 && minute() == 0 && second() == 0  ) | (hour() == 12 && minute() == 0 && second() == 0  )) {
    wait(500);
    requestTime();
  }
}
