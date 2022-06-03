#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_CCS811.h>

// Commenter si debug off
//#define DEBUG

// Pins
#define DHTPIN 8
#define DHTTYPE DHT22     
#define SMOKE A1

#define redLed 2
#define greenLed 3
#define blueLed 4

// Instances
DHT_Unified dht(DHTPIN, DHTTYPE);
Adafruit_CCS811 ccs;
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display


// Prototypes
void displayAt(int col, int row, String text);
void initDHT();
void initCSS();
void initSMOKE();
void initLed();
double* getTempHumi();
int getSMOKE();

// Variables
uint32_t delayReadDHT;
unsigned long realTime;
unsigned long oldRealTime;

int smokeThreshold = 800;
float co2Threshold = 750;
float tvocThreshold = 10;

void setup() 
{
  #ifdef DEBUG
    while(!Serial)
    Serial.begin(9600);
  #endif
  //lcd
  lcd.init();
  lcd.backlight();
  //DHT22 (Temperature, Humidity)
  initLed();
  initDHT();
  initCSS();
  initSMOKE();
}

void loop() 
{
  // Read DHT
  realTime = millis();
  if (realTime - oldRealTime > delayReadDHT)
  {
    oldRealTime = realTime;
    double temp, humidity;
    getTempHumi(temp, humidity);
    if (temp != -1)
    {
      displayAt(0, 0, "Temp:");
      displayAt(6, 0, String(temp));
      displayAt(11, 0, "C");
    }
    if (humidity != -1)
    {
      displayAt(0, 1, "Humi:");
      displayAt(6, 1, String(humidity));
      displayAt(11, 1, "%");
    }
  }

  // Read CSS
  if(ccs.available())
  {
    double eCO2, TVOC;
    readCSS(eCO2, TVOC);
    if (eCO2 != -1)
    {
      displayAt(0, 2, "eCO2:");
      displayAt(5, 2, String(eCO2));
      displayAt(0, 3, "TVOC:");
      displayAt(5, 3, String(TVOC));
    }

    // Threshold
    if (eCO2 > co2Threshold)
      digitalWrite(blueLed, HIGH);
    else
      digitalWrite(blueLed, LOW);

    if (TVOC > tvocThreshold)
      digitalWrite(greenLed, HIGH);
    else
      digitalWrite(greenLed, LOW);
  }

  // Read Smoke
  int smoke = getSMOKE();
  if (smoke > smokeThreshold)
  {
    digitalWrite(redLed, HIGH);
    delay(3000);
    lcd.clear();
    displayAt(2, 1,"Attention fumee");
    displayAt(2, 2, "detectee");
    delay(3000);
    lcd.clear();
  }
  else
  {
    digitalWrite(redLed, LOW);
  }
}

void initDHT()
{
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  #ifdef DEBUG
    Serial.println(F("------------------------------------"));
    Serial.println(F("Temperature Sensor"));
    Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
    Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
    Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
    Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
    Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
    Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
    Serial.println(F("------------------------------------"));
  #endif
  dht.humidity().getSensor(&sensor);
  #ifdef DEBUG
    Serial.println(F("Humidity Sensor"));
    Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
    Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
    Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
    Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
    Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
    Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
    Serial.println(F("------------------------------------"));
  #endif
  // Set delay between sensor readings based on sensor details.
  delayReadDHT = sensor.min_delay / 1000;
}

void initCSS()
{
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  pinMode(11, OUTPUT);
  digitalWrite(11, LOW);
  if(!ccs.begin())
  {
    #ifdef DEBUG
      Serial.println("Failed to start sensor! Please check your wiring.");
    #endif
    while(1);
  }
  while(!ccs.available());
}

void initSMOKE()
{
  pinMode(SMOKE, INPUT);
}

void initLed()
{
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(blueLed, OUTPUT);

  digitalWrite(redLed, HIGH);
  digitalWrite(greenLed, HIGH);
  digitalWrite(blueLed, HIGH);
}

void getTempHumi(double &temp, double &humidity)
{
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature))
  {
    #ifdef DEBUG
      Serial.println(F("Error reading temperature!"));
    #endif
    temp = -1;
  }
  else 
  {
    temp = event.temperature;
    #ifdef DEBUG
      Serial.print(F("Temperature: "));
      Serial.print(temp);
      Serial.println(F("°C"));
    #endif
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) 
  {
    #ifdef DEBUG
      Serial.println(F("Error reading humidity!"));
    #endif
    humidity = -1;
  }
  else 
  {
    humidity = event.relative_humidity;
    #ifdef DEBUG
      Serial.print(F("Humidity: "));
      Serial.print(humidity);
      Serial.println(F("%"));
    #endif
  }
}

void readCSS(double &eCO2, double &TVOC)
{
  if(!ccs.readData())
  {
    eCO2 = ccs.geteCO2();
    TVOC = ccs.getTVOC();

    #ifdef DEBUG
      Serial.print("CO2: ");
      Serial.print(eCO2);
      Serial.print("ppm, TVOC: ");
      Serial.println(TVOC);
    #endif
  }
  else
  {
    eCO2 = -1;
    TVOC = -1;
    #ifdef DEBUG
      Serial.println("ERROR!");
      while(1);
    #endif
  }
}

int getSMOKE()
{
  int smoke = analogRead(SMOKE);
  #ifdef DEBUG
    Serial.print("Smoke: ");
    Serial.println(smoke);
  #endif
  return smoke;
}

void displayAt(int col, int row, String text)
{
  lcd.setCursor(col, row);
  lcd.print(text);
}
