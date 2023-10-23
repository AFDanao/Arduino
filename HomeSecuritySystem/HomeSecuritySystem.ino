#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Keypad.h>
#include <DHT.h>
#include <Wire.h>
#include <BH1750.h>

#define RST_PIN 5
#define SS_PIN 53

#define DHTPIN 2
#define DHTTYPE DHT22

Servo servoLeft;
Servo servoRight;
BH1750 lightMeter;
int fire = A0;
int water = A1;
int bzz = A2;
int smoke = A5;
int servoLeftPin = 13;   //Smaller Servo
int servoRightPin = 12;  //Larger Servo
int trg = 11;
int ech = 10;
int led = 9;

MFRC522 mfrc522(SS_PIN, RST_PIN);
DHT dht(DHTPIN, DHTTYPE);

String key = "";
int atHome = 0;
int fireValue;
int waterValue;
int smokeValue;
long duration;
int distance;
int humidityValue;
int temperatureValue;
int stopperClose = 0;
int stopperOpen = 1;
int luxValue;

const byte ROWS = 4;
const byte COLS = 3;

char hexaKeys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};

byte rowPins[ROWS] = { 22, 24, 26, 28 };
byte colPins[COLS] = { 30, 32, 34 };

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void setup() {
  // Set the output pins
  pinMode(trg, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(bzz, OUTPUT);

  // Set the input pins
  pinMode(ech, INPUT);
  pinMode(fire, INPUT);
  pinMode(smoke, INPUT);

  Serial.begin(9600);  // Initialize serial
  SPI.begin();         // Init SPI bus
  mfrc522.PCD_Init();  // Init MFRC522 card
  Wire.begin();
  lightMeter.begin();
  dht.begin();
  servoLeft.attach(servoLeftPin);
}

void loop() {
  selectSystem();

  checkSmoke(smoke);

  checkFire(fire);

  checkWater(water);

  checkHumTem();

  checkLight();

  // Reset the loop, This saves the entire process when idle.
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return NULL;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return NULL;
  }

  printDec(mfrc522.uid.uidByte, mfrc522.uid.size);
  delay(1000);
}

void selectSystem() {
  char customKey = customKeypad.getKey();
  if ((checkDist(trg, ech) <= 10) || (atHome == 1)) {
    if (customKey) {
      if (atHome == 1) {
        switch (customKey) {
          case '1':
            {
              Serial.println("Selected Ultrasonic Sensor.");
              Serial.println("Distance Value is: " + String(distance));
              break;
            }
          case '2':
            {
              Serial.println("Selected Fire Sensor.");
              Serial.println("Fire Value Is: " + String(fireValue));
              break;
            }
          case '3':
            {
              Serial.println("Selected Water Sensor.");
              Serial.println("Water Value Is: " + String(waterValue));
              break;
            }
          case '4':
            {
              Serial.println("Selected Smoke Sensor.");
              Serial.println("Smoke Value Is: " + String(smokeValue));
              break;
            }
          case '5':
            {
              Serial.println("Selected Humidity & Temperature Sensor.");
              Serial.println("Humidity Value Is: " + String(humidityValue) + "%. Temperature Value Is: " + String(temperatureValue) + "°C");
              break;
            }
          case '6':
            {
              Serial.println("Selected Light Sensor.");
              Serial.println("Light Value Is: " + String(luxValue));
              break;
            }
          default:
            {
              Serial.println("Unknown System Selected! Please Choose From 1-6 Only.");
              break;
            }
        }
      } else if (atHome == 0) {
        Serial.println("Owner Is Not Home. Intruder Alert!");
      }
    }
  }
}

void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    key = key + buffer[i];
  }
  if (String(key) == "186184230137") {
    atHome++;
    if (atHome == 1) {
      Serial.println("Welcome Home! Inserted ID: " + String(key));
    } else if (atHome > 1) {
      Serial.println("Good bye!");
      atHome = 0;
    }
  } else {
    Serial.println("Unknown Inserted ID: " + String(key));
  }
  key = "";
}

int checkDist(int trg, int ech) {
  digitalWrite(trg, LOW);
  delayMicroseconds(2);
  digitalWrite(trg, HIGH);
  delayMicroseconds(10);
  digitalWrite(trg, LOW);

  duration = pulseIn(ech, HIGH);
  distance = duration * 0.034 / 2;

  return distance;
}

void checkFire(int fire) {
  fireValue = analogRead(fire);
  if (fireValue <= 100) {
    Serial.println("WARNING: Fire Detected! Please Evacuate Safely. Value: " + String(fireValue));
    motor("Door");
  }
}

void checkSmoke(int smoke) {
  smokeValue = analogRead(smoke);
  if (smokeValue > 400) {
    Serial.println("WARNING: Smoke Detected! Check To See If There Is Fire. Value: " + String(smokeValue));
    motor("Window");
  }
}

void checkWater(int water) {
  waterValue = analogRead(water);

  if (waterValue > 500) {
    Serial.println("WARNING: Rising Water Level! Seek Higher Ground. Water Value: " + String(waterValue));
    motor("Drain");
  }
}

void checkHumTem(){
  humidityValue = dht.readHumidity();
  temperatureValue = dht.readTemperature();
}

void checkLight(){
  luxValue = lightMeter.readLightLevel();
  if (luxValue < 40) {
    Serial.println("It's Turning Night Time. Good Night! Value: " + String(luxValue));
  }
}

void motor(String component) {
  Serial.println("Opening " + component);
  alert();
  digitalWrite(led, HIGH);
  delay(500);
  digitalWrite(led, LOW);
  servoLeft.write(10);
  servoRight.attach(servoRightPin);

  if (stopperClose == 1) {
    servoRight.write(145);
    stopperClose = 0;
  } else {
    servoRight.detach();
  }

  stopperOpen = 1;
  delay(1000);
  servoRight.attach(servoRightPin);

  if (stopperOpen == 1) {
    servoRight.write(0);
    stopperOpen = 1;
  } else {
    servoRight.detach();
  }
  servoLeft.write(120);
  delay(1000);
  stopperClose = 1;
  servoRight.detach();
  delay(2000);

  return;
}

void alert() {
  tone(bzz, 200);
  delay(200);
  noTone(bzz);
  delay(200);
  tone(bzz, 200);
  delay(200);
  noTone(bzz);
}
