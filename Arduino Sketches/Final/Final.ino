//libraries
#include <Servo.h>
#include "NewPing.h"

#include "Pins.h"


enum MOTOR_STATE {
  OFF,
  ON,
  REV
};

enum FISH_CONCENTRATION {
  LOWW,
  HIGHH,
  MEDIUM
};

typedef int Pin;
typedef MOTOR_STATE State;
typedef int cm;
typedef FISH_CONCENTRATION Concentration;

Servo usMotor;
NewPing ultrasonic(US_TRIG, US_ECHO, US_MAX_DIST);

int pins[] {LOW_PIN, MID_PIN, HIGH_PIN};
int clicks {0}; //Fish Count
int servo_degrees[US_STEPS];
cm distances[US_STEPS];

/*//SD/sine wave variables/defines//*/
#include <SD.h>                           //include SD module library
#include <TMRpcm.h>                       //include speaker control library
#define SD_ChipSelectPin 10                //define CS pin

TMRpcm tmrpcm;
char file[] {"music.wav"};
/*////*/


void controlMotor(State state, Pin pos_pin, Pin neg_pin) {
  if (state == MOTOR_STATE::OFF) {
    digitalWrite(pos_pin, LOW);
    digitalWrite(neg_pin, LOW);
  } else if (state == MOTOR_STATE::ON) {
    digitalWrite(pos_pin, HIGH);
    digitalWrite(neg_pin, LOW);
  } else if (state == MOTOR_STATE::REV) {
    digitalWrite(pos_pin, LOW);
    digitalWrite(neg_pin, HIGH);
  }
}

void oscillator(bool play) {
  if (!play) {
    tmrpcm.stopPlayback();
    return;
  Serial.println("Enabled");
  }
  if (!tmrpcm.isPlaying()) {
    tmrpcm.play(file);
  }
}

void ledFlasher(bool play) {
  static unsigned long millis_last_flash;
  if (!play) {
    digitalWrite(LED, LOW);
    return;
  }

  if(millis() - millis_last_flash < 3000) {
    return;
  }

  millis_last_flash = millis();
  Serial.println("LED");
  digitalWrite(LED, HIGH);
  delay(2000);   //make this "asynchronous"
  digitalWrite(LED, LOW);
}

void ultrasonicSensor() {
  static int previous_distance_cm {0};
  static int loops {0};
  static bool may_dumaan {true};  //setting this initially to true prevents an automatic fishy count on first ping (from the initial 0 to whatever value)
  static unsigned long millis_last_ping {0};

  if (millis() - millis_last_ping < 50) {                     // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
    return;
  }

  if (millis() - millis_last_ping < US_DELAY) {
    return;
  }

  millis_last_ping = millis();
  int distance_cm {ultrasonic.ping_cm()};
  if (distance_cm < 15) {   //Underwater Ultrasonic sensor has a blind spot of 20cm, so any reading below that would be invalid
    Serial.println("Got less than 15");
    return;
  }

  if ((abs(distance_cm - previous_distance_cm) > 20)) {
    if (may_dumaan) {
      may_dumaan = false;
    } else {
      clicks++;
      may_dumaan = true;

      Serial.print("Fishy counter: ");
      Serial.println(clicks);
      Serial.print("Loops: ");
      Serial.println(loops);
      Serial.println();
    }
  }
  previous_distance_cm = distance_cm;
  Serial.print("Ping: ");
  Serial.print(distance_cm); // Send ping, get distance in cm and print result (0 = outside set distance range)
  Serial.println("cm");

  loops++;
}
void sendFishConcentration() {
  Concentration concentration {fishConcentration()};

  Pin active {LOW_PIN};
  switch (concentration) {
    case FISH_CONCENTRATION::LOWW : {active = LOW_PIN; break;}
    case FISH_CONCENTRATION::MEDIUM : {active = MID_PIN; break;}
    case FISH_CONCENTRATION::HIGHH : {active = HIGH_PIN; break;}
    default: {active = LOW_PIN;}
  }

  for (Pin pin : pins) {
    digitalWrite(pin, pin==active);
  }
}


Concentration fishConcentration() {
  if (clicks >= 15) {
    return FISH_CONCENTRATION::HIGHH;
  } else if (clicks >= 5) {
    return FISH_CONCENTRATION::MEDIUM;
  } else if (clicks >= 2) {
    return FISH_CONCENTRATION::LOWW;
  }
}

void setup() {
  int out_pins[] {US_SERVO, LOW_PIN, MID_PIN, HIGH_PIN, US_TRIG, LED};
  for (int i=0; i<sizeof(out_pins)/sizeof(int); ++i) {
    pinMode(out_pins[i], OUTPUT);
  }

  int in_pins[] {US_ECHO, ATTRACTOR_ENABLE};
  for (int i=0; i<sizeof(in_pins)/sizeof(int); ++i) {
    pinMode(in_pins[i], INPUT);
  }

  randomSeed(analogRead(0));
  usMotor.attach(US_SERVO);
  usMotor.write(0);
  int i = 0; 
  for (int deg=0; deg <= 180; deg += 180 / (US_STEPS - 1)) {
    servo_degrees[i] = deg;
    i++;
  }
  servo_degrees[US_STEPS - 1] -= 2;

  Serial.begin(9600);
  while (!Serial);
  Serial.println("Config: ");
  Serial.print("Ultrasonic delay: ");
  Serial.println(US_DELAY);
  Serial.print("Servo degrees: ");
  for (int i=0; i < US_STEPS; ++i) {
    Serial.print(servo_degrees[i]);
    Serial.print(" ");
  }
  Serial.println();
  Serial.print("Int size: ");
  Serial.println(sizeof(int));
  Serial.print("Long size: ");
  Serial.println(sizeof(long));
  Serial.print("Long long size: ");
  Serial.println(sizeof(long long));
  
  /*//SD/sine wave setup//*/
  pinMode(SD_ChipSelectPin, OUTPUT);
  digitalWrite(SD_ChipSelectPin, LOW);
  while (!Serial);
  tmrpcm.speakerPin = 9;                  //define speaker pin. 
                                          //you must use pin 9 of the Arduino Uno and Nano
                                          //the library is using this pin
  Serial.println("Reading SD");
  if (!SD.begin(SD_ChipSelectPin)) {      //see if the card is present and can be initialized
    Serial.print("SD failed.");
    return;
  }
  Serial.println("SD");

  tmrpcm.setVolume(5);                    //0 to 7. Set volume level
  tmrpcm.play(file);
  tmrpcm.stopPlayback();
  /*////*/
}

void loop() {
  ultrasonicSensor();
  sendFishConcentration();
  oscillator(digitalRead(ATTRACTOR_ENABLE));
  ledFlasher(digitalRead(ATTRACTOR_ENABLE));
  ledFlasher(digitalRead(ATTRACTOR_ENABLE));
  Serial.print("Attractor:");
  Serial.println(digitalRead(ATTRACTOR_ENABLE));
}
