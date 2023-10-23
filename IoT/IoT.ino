#define BLYNK_TEMPLATE_ID "TMPL6CFDPZX6i"
#define BLYNK_TEMPLATE_NAME "IoT Based Health Monitoring"
#define BLYNK_AUTH_TOKEN "Pvp5baC_nmmlKioVU1WpaP9eVGSyZeuk"

#include <WiFi.h>
#include <Wire.h>
#include <WiFiClient.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <BlynkSimpleEsp32.h>
#include <DFRobot_Heartrate.h>

MAX30105 particleSensor;
DFRobot_Heartrate heartrate(DIGITAL_MODE);

char ssid[] = "";
char pass[] = "";

int pulsePin = 39;
int alcoholPin = 34;
int LED = 12;
int PULSE_FADE = 5;

int Threshold = 3450;
const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;

float beatsPerMinute;
float alcoholValue;
int beatAvg;

void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(115200);

  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30102 was not found. Please check wiring/power. ");
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup();                     //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A);  //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0);

  delay(20000);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
}

void loop() {
  long irValue = particleSensor.getIR();
  Serial.print(String(checkForBeat(irValue)));
  if (checkForBeat(irValue) == true) {
    long delta = millis() - lastBeat;
    lastBeat = millis();
    Serial.println("Delta: " + String(delta));

    beatsPerMinute = 60 / (delta / 1000.0);
  }

  int pulseValue = analogRead(pulsePin);

  alcoholValue = analogRead(alcoholPin);

  Serial.println("IR: " + String(irValue));
  Serial.println("BPM: " + String(beatsPerMinute));
  Serial.println("Gas: " + String(alcoholValue));

  Blynk.virtualWrite(V0, pulseValue);
  Blynk.virtualWrite(V1, beatsPerMinute);
  Blynk.virtualWrite(V2, alcoholValue);

  Blynk.run();
}