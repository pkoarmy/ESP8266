#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Arduino.h>

// https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/
int measurePin = A0; // Connect dust sensor to 8266 A0
int ledPower = 14;   // Connect led driver pins of dust sensor to 8266 D5
int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;

const int interval = 3000;
float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

float lastMeasureTime = 0;

int i = 0;
float ppm=0;
float voltage = 0;

float h, t, p, pin, dp;
char temperatureFString[6];
char dpString[6];
char humidityString[6];
char pressureString[7];
char pressureInchString[6];
 
Adafruit_BME280 bme; // I2C

void setup()
{
    Serial.begin(9600);
  
    // Printing the ESP IP address
    if (!bme.begin(0x76)) 
    {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);
    }
    pinMode(ledPower,OUTPUT);
    digitalWrite(ledPower,HIGH);  
}
 
void loop()
{
    i += 1;
    digitalWrite(ledPower,LOW); // power on the LED
    delayMicroseconds(samplingTime);
    voMeasured = analogRead(measurePin); // read the dust value
    delayMicroseconds(deltaTime);
    digitalWrite(ledPower,HIGH); // turn the LED off
    delayMicroseconds(sleepTime);

    ppm += voMeasured;
  
    static unsigned long OledTimer=millis();
    if (millis() - OledTimer >=1000) {
        OledTimer=millis();
//          voltage = (ppm/i) * 0.0049; // ( 5.0 / 1024 )
        voltage = (ppm/i) * 0.0032; // ( 3.3 / 1024 )
        dustDensity = 0.17 * voltage - 0.1;
        float dustDensity2 = (0.17 * voltage - 0.1 ) * 1000; // ug/m3
        float dustDensity1 = 170 * voltage - 0.1;
        float ppmpercf = (voltage-0.0256)*120000;
        Serial.println(voMeasured);
        Serial.println(voltage);
        Serial.println(dustDensity);  // unit: mg/m3
        Serial.println(dustDensity1);
        Serial.println(dustDensity2);  // unit: mg/m3
        Serial.println(ppmpercf);  // unit: mg/m3

        h = bme.readHumidity();
        t = bme.readTemperature();
        t = t*1.8+32.0;
        dp = t-0.36*(100.0-h);
        
        p = bme.readPressure()/100.0F;
        pin = 0.02953*p;
        dtostrf(t, 5, 1, temperatureFString);
        dtostrf(h, 5, 1, humidityString);
        dtostrf(p, 6, 1, pressureString);
        dtostrf(pin, 5, 2, pressureInchString);
        dtostrf(dp, 5, 1, dpString);
    
        Serial.print("Temperature = ");
        Serial.println(temperatureFString);
        Serial.print("Humidity = ");
        Serial.println(humidityString);
        Serial.print("Pressure = ");
        Serial.println(pressureString);
        Serial.print("Pressure Inch = ");
        Serial.println(pressureInchString);
        Serial.print("Dew Point = ");
        Serial.println(dpString);
    
        Serial.println("...............................................");
    }
}
