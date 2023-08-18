int ir0 = 13;
int ir1 = 12;
int ir2 = 11;

int bzz = 10;

int led0 = 9;
int led1 = 8;
int led2 = 7;
int led3 = 6;

bool ob0;
bool ob1;
bool ob2;

boolean detect(int ir, int led) {
  if (digitalRead(ir) == 0) {
    digitalWrite(led, HIGH);
    Serial.print("\nIR Reading: ");
    Serial.print(digitalRead(ir));
    return true;
  } else {
    digitalWrite(led, LOW);
    Serial.print("\nIR Reading: ");
    Serial.print(digitalRead(ir));
    return false;
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(ir0, INPUT);
  pinMode(ir1, INPUT);
  pinMode(ir2, INPUT);

  pinMode(bzz, OUTPUT);

  pinMode(led0, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  ob0 = detect(ir0, led0);
  ob1 = detect(ir1, led1);
  ob2 = detect(ir2, led2);

  if ((ob0 == true) && (ob1 == true) && (ob2 == true)) {
    digitalWrite(led3, HIGH);
    tone(bzz, 1000);
    delay(50);
    noTone(bzz);
    delay(50);
    tone(bzz, 1000);
  } else {
    digitalWrite(led3, LOW);
    noTone(bzz);
  }
}
