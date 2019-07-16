#include <NewPing.h>

#define TRIGGER_PIN_1  14  // A0  "Left"
#define ECHO_PIN_1     14  // A0  "Left"
#define TRIGGER_PIN_2  15  // A1  "Middle"
#define ECHO_PIN_2     15  // A1  "Middle"
#define TRIGGER_PIN_3  16  // A2  "Right"
#define ECHO_PIN_3     16  // A2  "Right"

#define SONAR_NUM      3 
#define MAX_DISTANCE 150 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define PING_INTERVAL 33

unsigned long pingTimer[SONAR_NUM];
uint8_t currentSensor = 0;
int resultDistSensor[SONAR_NUM] = {0,0,0};

char logChars[22];

NewPing sonar[SONAR_NUM] = {
  NewPing(TRIGGER_PIN_1, ECHO_PIN_1, MAX_DISTANCE),
  NewPing(TRIGGER_PIN_2, ECHO_PIN_2, MAX_DISTANCE),
  NewPing(TRIGGER_PIN_3, ECHO_PIN_3, MAX_DISTANCE)
};

void distSensorSetup() {
  pingTimer[0] = millis() + 75;
  for (uint8_t i = 1; i < SONAR_NUM; ++i)
    pingTimer[i] = pingTimer[i - 1] + PING_INTERVAL;
}

void distSensorUpdate() {
  for (uint8_t i = 0; i < SONAR_NUM; ++i) {
    if (millis() >= pingTimer[i]) {
      pingTimer[i] += PING_INTERVAL * SONAR_NUM;
      sonar[currentSensor].timer_stop();
      currentSensor = i;
      sonar[currentSensor].ping_timer(echoCheck);
    }
  }
}

void echoCheck() {
  if (sonar[currentSensor].check_timer())
    publishResult(currentSensor, sonar[currentSensor].ping_result * 10 / US_ROUNDTRIP_CM);
}

void publishResult(uint8_t sensor, int16_t dist) {
  String logString = "id: " + String(sensor) + ", dist: " + String(dist) + " mm";
  logString.toCharArray(logChars, 22);
  nh.loginfo(logChars);
}
