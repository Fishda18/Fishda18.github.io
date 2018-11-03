// ---------------------------------------------------------------------------
// Example NewPing library sketch that does a ping about 20 times per second.
// ---------------------------------------------------------------------------

#include "NewPing.h"

//Sonar-related defines
#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     13  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 400 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

#define LED_INDICATOR 7
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
int previous_distance_cm{0};
int clicks{0};
int loops{0};
bool may_dumaan{true};  //setting this initially to true prevents an automatic fishy count on first ping (from the initial 0 to whatever value)

void setup() {
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
}

void loop() {
  delay(30);                     // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  int distance_cm{sonar.ping_cm()};
  if (distance_cm < 15) {   //Underwater Ultrasonic sensor has a blind spot of 20cm, so any reading below that would be invalid
    //Serial.println("Got less than 15");
    return;
  }
  if ((abs(distance_cm - previous_distance_cm) > 10)) {
    if (may_dumaan) {
      may_dumaan = false;
    } else {
      clicks++;
      may_dumaan = true;
      //Light a led for half a second to signify detection of fish movement
      digitalWrite(LED_INDICATOR, HIGH);
      delay(500);
      digitalWrite(LED_INDICATOR, LOW);

      Serial.println("May dumaan na fishy");
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
