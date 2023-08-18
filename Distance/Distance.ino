void blink(int a, int trg, int ech) {
  float duration;
  float distance;
  int dly;

  digitalWrite(trg, LOW);
  delayMicroseconds(2);
  digitalWrite(trg, HIGH);
  delayMicroseconds(10);
  digitalWrite(trg, LOW);

  duration = pulseIn(ech, HIGH);
  distance = (duration*.0343)/2;

  Serial.print("\nDistance: \n");
  Serial.println(distance);

  if (distance > 30) {
    dly = 1000;
  } else if ((distance >= 10) || (distance <= 29)) {
    dly = 500;
  } 
  if (distance <= 9) {
    dly = 50;
  }

  Serial.print("Delay: \n");
  Serial.print(dly);

  digitalWrite(a, HIGH);
  delay(dly);
  digitalWrite(a, LOW);
  delay(dly);
}

int led0 = 13;
int led1 = 12;
int led2 = 11;
int led3 = 10;
int led4 = 9;

int trg = 3;
int ech = 6;

void setup() {
  pinMode(led0, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);

  pinMode(trg, OUTPUT);
  pinMode(ech, INPUT);
  Serial.begin(9600);
}

void loop() {
  blink(led0, trg, ech);
  blink(led1, trg, ech);
  blink(led2, trg, ech);
  blink(led3, trg, ech);
  blink(led4, trg, ech);

  blink(led4, trg, ech);
  blink(led3, trg, ech);
  blink(led2, trg, ech);
  blink(led1, trg, ech);
  blink(led0, trg, ech);
}

