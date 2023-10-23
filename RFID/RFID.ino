/*
PINOUT:
RC522 MODULE    Uno/Nano     MEGA
SDA             D10          D9
SCK             D13          D52
MOSI            D11          D51
MISO            D12          D50
IRQ             N/A          N/A
GND             GND          GND
RST             D9           D8
3.3V            3.3V         3.3V
*/
/* Include the standard Arduino SPI library */
#include <SPI.h>
/* Include the RFID library */
#include <RFID.h>
#include <DHT.h>

/* Define the DIO used for the SDA (SS) and RST (reset) pins. */
#define SDA_DIO 5
#define RESET_DIO 53

#define DHTPIN 2
#define DHTTYPE DHT22
/* Create an instance of the RFID library */
RFID RC522(SDA_DIO, RESET_DIO); 

DHT dht(DHTPIN,DHTTYPE);

String numRef = "";

int led0 = A1;
int smoke = A5;
int sensorThres = 400;

int chk;
float hum;
float temp;

void setup()
{ 
  Serial.begin(9600);
  /* Enable the SPI interface */
  SPI.begin(); 
  /* Initialise the RFID reader */
  RC522.init();
  dht.begin();

  pinMode(led0, OUTPUT);
  pinMode(smoke, INPUT);
}

void loop()
{
  int smokeSensor = analogRead(smoke);
  Serial.print("Smoke: ");
  Serial.print(smokeSensor);
  Serial.println();
  hum = dht.readHumidity();
  temp = dht.readTemperature();

  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.print("%, Temperature: ");
  Serial.print(temp);
  Serial.print(" Celsius");
  Serial.println();
  /* Has a card been detected? */
  if (RC522.isCard())
  {
    /* If so then get its serial number */
    RC522.readCardSerial();
    Serial.println("Card detected:");
    for(int i=0;i<5;i++)
    {
    // Serial.print(RC522.serNum[i],DEC);
    Serial.print(RC522.serNum[i]); //to print card detail in Hexa Decimal format
    numRef = numRef + String(RC522.serNum[i],HEX);
    }
    Serial.println();
    Serial.println(numRef);
    
    if (numRef == "bab8e6896d") {
      numRef = "";
      Serial.println("Welcome Home");
    }
  }
  delay(1000);
}