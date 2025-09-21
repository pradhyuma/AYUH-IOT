// Authorization token from Blynk app
#define BLYNK_TEMPLATE_ID "TMPL59RQclluO"
#define BLYNK_TEMPLATE_NAME "LEDControl"
#define BLYNK_AUTH_TOKEN "3g3q7jgjB6rZVOSqiij2CCq45inwpS_V"

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <ESP8266Wifi.h>

// Define SSR control pins
#define LIGHT_PIN D6     // Channel 1 for lights
#define MOTOR_PIN D7     // Channel 2 for motor


// WiFi credentials
char ssid[] = "YOUR_WIFI_SSID";
char pass[] = "YOUR_WIFI_PASSWORD";

// NTP settings
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);

// Timezone offset (in seconds)
int timezoneOffset = 0;

// Control variables
int motorRunTime = 15;    // Default: 15 minutes every hour
int lightDuration = 12;   // Default: 12 hours
int lightStartHour = 8;   // Default: 8:00 AM
bool motorAlwaysOn = false;
bool lightAlwaysOn = false;
bool systemEnabled = true;

// State tracking
bool motorState = false;
bool lightState = false;
unsigned long motorStartTime = 0;
unsigned long lightStartTime = 0;

BlynkTimer scheduler;

BLYNK_CONNECTED() {
  // Sync time on connection
  timeClient.update();
  setTime(timeClient.getEpochTime());
  
  // Request timezone from app
  Blynk.syncVirtual(V10);
  
  // Send current settings to app
  Blynk.virtualWrite(V11, lightStartHour);
  Blynk.virtualWrite(V12, lightDuration);
  Blynk.virtualWrite(V13, motorRunTime);
}

// System control
BLYNK_WRITE(V0) { // System on/off
  systemEnabled = param.asInt();
  updateOutputs();
}

// Light control
BLYNK_WRITE(V1) { // Light always on
  lightAlwaysOn = param.asInt();
  if (lightAlwaysOn) {
    digitalWrite(LIGHT_PIN, HIGH);
    lightState = true;
    Blynk.virtualWrite(V2, "ON (Always)");
  } else {
    updateLightSchedule();
  }
}

BLYNK_WRITE(V11) { // Light start hour
  lightStartHour = param.asInt();
  updateLightSchedule();
}

BLYNK_WRITE(V12) { // Light duration
  lightDuration = param.asInt();
  updateLightSchedule();
}

// Light duration buttons
BLYNK_WRITE(V20) { if (param.asInt()) { lightDuration = 4; updateLightSettings(); } }
BLYNK_WRITE(V21) { if (param.asInt()) { lightDuration = 6; updateLightSettings(); } }
BLYNK_WRITE(V22) { if (param.asInt()) { lightDuration = 8; updateLightSettings(); } }
BLYNK_WRITE(V23) { if (param.asInt()) { lightDuration = 10; updateLightSettings(); } }
BLYNK_WRITE(V24) { if (param.asInt()) { lightDuration = 12; updateLightSettings(); } }

// Motor control
BLYNK_WRITE(V3) { // Motor always on
  motorAlwaysOn = param.asInt();
  if (motorAlwaysOn) {
    digitalWrite(MOTOR_PIN, HIGH);
    motorState = true;
    Blynk.virtualWrite(V4, "ON (Always)");
  } else {
    updateMotorSchedule();
  }
}

BLYNK_WRITE(V13) { // Motor run time
  motorRunTime = param.asInt();
  updateMotorSchedule();
}

// Motor run time buttons
BLYNK_WRITE(V30) { if (param.asInt()) { motorRunTime = 5; updateMotorSettings(); } }
BLYNK_WRITE(V31) { if (param.asInt()) { motorRunTime = 10; updateMotorSettings(); } }
BLYNK_WRITE(V32) { if (param.asInt()) { motorRunTime = 15; updateMotorSettings(); } }
BLYNK_WRITE(V33) { if (param.asInt()) { motorRunTime = 20; updateMotorSettings(); } }
BLYNK_WRITE(V34) { if (param.asInt()) { motorRunTime = 25; updateMotorSettings(); } }

// Timezone
BLYNK_WRITE(V10) {
  timezoneOffset = param.asInt() * 3600;
  timeClient.setTimeOffset(timezoneOffset);
  timeClient.update();
  setTime(timeClient.getEpochTime());
}

// Update light settings in app
void updateLightSettings() {
  Blynk.virtualWrite(V12, lightDuration);
  lightAlwaysOn = false;
  Blynk.virtualWrite(V1, 0);
  updateLightSchedule();
}

// Update motor settings in app
void updateMotorSettings() {
  Blynk.virtualWrite(V13, motorRunTime);
  motorAlwaysOn = false;
  Blynk.virtualWrite(V3, 0);
  updateMotorSchedule();
}

// Update light schedule based on current time
void updateLightSchedule() {
  if (!systemEnabled || lightAlwaysOn) return;
  
  timeClient.update();
  setTime(timeClient.getEpochTime());
  int currentHour = hour();
  int currentMinute = minute();
  int currentTimeInMinutes = currentHour * 60 + currentMinute;
  int lightStartInMinutes = lightStartHour * 60;
  int lightEndInMinutes = lightStartInMinutes + (lightDuration * 60);
  
  // Handle overnight schedule
  if (lightEndInMinutes > 1440) {
    lightEndInMinutes -= 1440;
    if (currentTimeInMinutes >= lightStartInMinutes || currentTimeInMinutes < lightEndInMinutes) {
      digitalWrite(LIGHT_PIN, HIGH);
      lightState = true;
      Blynk.virtualWrite(V2, "ON (Scheduled)");
    } else {
      digitalWrite(LIGHT_PIN, LOW);
      lightState = false;
      Blynk.virtualWrite(V2, "OFF");
    }
  } else {
    if (currentTimeInMinutes >= lightStartInMinutes && currentTimeInMinutes < lightEndInMinutes) {
      digitalWrite(LIGHT_PIN, HIGH);
      lightState = true;
      Blynk.virtualWrite(V2, "ON (Scheduled)");
    } else {
      digitalWrite(LIGHT_PIN, LOW);
      lightState = false;
      Blynk.virtualWrite(V2, "OFF");
    }
  }
}

// Update motor schedule based on current time
void updateMotorSchedule() {
  if (!systemEnabled || motorAlwaysOn) return;
  
  timeClient.update();
  setTime(timeClient.getEpochTime());
  int currentMinute = minute();
  
  // Motor runs for specified minutes at the start of each hour
  if (currentMinute < motorRunTime) {
    digitalWrite(MOTOR_PIN, HIGH);
    motorState = true;
    Blynk.virtualWrite(V4, "ON (" + String(motorRunTime - currentMinute) + "m left)");
  } else {
    digitalWrite(MOTOR_PIN, LOW);
    motorState = false;
    int minutesLeft = 60 - currentMinute;
    Blynk.virtualWrite(V4, "OFF (" + String(minutesLeft) + "m to start)");
  }
}

// Update all outputs
void updateOutputs() {
  if (!systemEnabled) {
    digitalWrite(LIGHT_PIN, LOW);
    digitalWrite(MOTOR_PIN, LOW);
    Blynk.virtualWrite(V2, "OFF (System Off)");
    Blynk.virtualWrite(V4, "OFF (System Off)");
    return;
  }
  
  if (lightAlwaysOn) {
    digitalWrite(LIGHT_PIN, HIGH);
    Blynk.virtualWrite(V2, "ON (Always)");
  } else {
    updateLightSchedule();
  }
  
  if (motorAlwaysOn) {
    digitalWrite(MOTOR_PIN, HIGH);
    Blynk.virtualWrite(V4, "ON (Always)");
  } else {
    updateMotorSchedule();
  }
}

void setup() {
  Serial.begin(115200);
  
  // Initialize pins
  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(MOTOR_PIN, OUTPUT);
  digitalWrite(LIGHT_PIN, LOW);
  digitalWrite(MOTOR_PIN, LOW);
  
  // Connect to WiFi and Blynk
  Blynk.begin(auth, ssid, pass);
  
  // Initialize NTP client
  timeClient.begin();
  timeClient.update();
  setTime(timeClient.getEpochTime());
  
  // Set up scheduler
  scheduler.setInterval(30000L, []() { // Update every 30 seconds
    timeClient.update();
    setTime(timeClient.getEpochTime());
    updateOutputs();
  });
  
  Serial.println("System started");
}

void loop() {
  Blynk.run();
  scheduler.run();
}
