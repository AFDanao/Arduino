#include <Keypad.h>
#include <Servo.h>
#include <Wire.h>
#include <BH1750.h>

Servo myservoL;
Servo myservoR;
BH1750 lightMeter;

int count = 0;
int wrong = 0;

int bzz = A0;
int rain = A1;
int led1 = 2;
int led2 = 22;
int pir = 23;
int ir = 11;
int servoR = 12;
int servoL = 13;
int stopperClose = 0;
int stopperOpen = 1;

const byte ROWS = 4;
const byte COLS = 3;

char hexaKeys [ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte rowPins [ROWS] = {3, 4, 5, 6};
byte colPins [COLS] = {7, 8, 9};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(pir, INPUT);
  pinMode(ir, INPUT);
  pinMode(rain, INPUT);

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(bzz, OUTPUT);

  myservoR.attach(servoR);
  Wire.begin();
  lightMeter.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  int pirValue = digitalRead(pir);
  Serial.println(pirValue);
  char customKey = customKeypad.getKey();
  float lux = lightMeter.readLightLevel();
  int rainLevel = analogRead(rain);

  myservoR.write(0);

  if (pirValue == HIGH) {
    digitalWrite(led2, HIGH);
    delay(100);
  } else {
    digitalWrite(led2, LOW);
    delay(100);
  }

  if ((lux <= 1000) && (lux > 200)) {
    Serial.println("Time: Midday");
  } else if ((lux <= 200) && (lux > 40)) {
    Serial.println("Time: Sunrise / Sunset");
  } else if (lux <= 40) {
    Serial.println("Weather: Thick Clouds");
  }

  if (rainLevel < 500) {
    Serial.println("Weather Condition: Raining");
    tone(bzz, 200);
    delay(200);
    noTone(bzz);
    delay(200);
    tone(bzz, 200);
    delay(200);
    noTone(bzz);
  }

  if (digitalRead(ir) == 0) {
    // Serial.println(digitalRead(ir));
    if (customKey) {
      incKey(customKey);
    }
  } else {
    // Serial.println(digitalRead(ir));
    if (customKey) {
      Serial.println("You cannot enter the password!");
    }
  }
}

String pass = "";
void incKey(char customKey) {
  Serial.println(customKey);

  pass = pass + customKey;

  char str = '123';

  count = count + 1;

  if (count == 3) {
    Serial.println("Inputted Password: " + pass);
    if (pass == "123") {
      Serial.println("Password is correct. Welcome Home!");

      myservoR.write(10);
      myservoL.attach(servoL);
      
      if (stopperClose == 1) {
        myservoL.write(145);
        stopperClose = 0;
      } else {
        myservoL.detach();
      }

      stopperOpen = 1;
      delay(1000);

      myservoL.attach(servoL);
      if (stopperOpen == 1) {
        myservoL.write(0);
        stopperOpen = 1;
      } else {
        myservoL.detach();
      }
      myservoR.write(120);
      delay(1000);
      stopperClose = 1;
      myservoL.detach();
      delay(2000);

    } else {
      Serial.println("Wrong Password!");
      wrong++;
      digitalWrite(led1, HIGH);
      tone(bzz, 500);
      delay(500);
      digitalWrite(led1, LOW);
      noTone(bzz);
      delay(500);
      digitalWrite(led1, HIGH);
      tone(bzz, 500);
      delay(500);
      digitalWrite(led1, LOW);
      noTone(bzz);

      if (wrong == 3) {
        lockDownSequence();
      }
    }
    noTone(bzz);
    count = 0;
    pass = "";
  }
}

void lockDownSequence() {
  Serial.println("You have entered the wrong password 3 times! Lock down begin.");
  for (int i = 10; i > 0; i--) {
    tone(bzz, 200);
    Serial.println("System on lock down! Please wait " + String(i) + " second(s).");
    delay(1000);
    noTone(bzz);
  }
  Serial.println("Lock down sequence lifted!");
  wrong = 0;
}
