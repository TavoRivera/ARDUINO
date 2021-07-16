#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"
#include <LiquidCrystal.h>
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);
MAX30105 particleSensor;
#define MAX_BRIGHTNESS 255
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
//Arduino Uno doesn't have enough SRAM to
store 50 samples of IR led data and red led
data in 32-bit format
//To solve this problem, 16-bit MSB of the
sampled data will be truncated. Samples
become 16-bit data.
uint16_t irBuffer[50]; //infrared LED
sensor data
uint16_t redBuffer[50]; //red LED sensor
data
#else
uint32_t irBuffer[50]; //infrared LED
sensor data
uint32_t redBuffer[50]; //red LED sensor
data
#endif
int32_t spo2; //SPO2 value
int8_t validSPO2; //indicator to show if
the SPO2 calculation is valid
int32_t heartRate; //heart rate value
int8_t validHeartRate; //indicator to show
if the heart rate calculation is valid
void setup()
{
 lcd.begin(16, 2);
 Serial.begin(115200);
 Serial.println("Initializing...");
 // Initialize sensor
 if (!particleSensor.begin(Wire,
I2C_SPEED_FAST)) //Use default I2C port,
400kHz speed
 {
 lcd.setCursor(0,0);
 lcd.print("sensor not found");
 Serial.println("MAX30105 was not
found. Please check wiring/power. ");
 while (1);
 }
 lcd.setCursor(3,0);
 lcd.print("OXIMETRO");
 lcd.setCursor(6,1);
 lcd.print("UNI");
 delay(3000);
 Serial.println("Place your index
finger on the sensor with steady
pressure.");
 lcd.setCursor(0,0);
 lcd.print("Ponga su dedo");
 lcd.setCursor(0,1);
 lcd.print("estable ");
 delay(3000);
 particleSensor.setup(55, 4, 2, 200, 411,
4096); //Configure sensor with these
settings
}
void loop()
{
 lcd.clear();
 lcd.setCursor(4,0);
 lcd.print("BPM:");
 lcd.setCursor(3,1);
 lcd.print("SPO2:");
 //read the first 50 samples, and
determine the signal range
 for (byte i = 0 ; i < 50 ; i++)
 {
 while (particleSensor.available() ==
false) //do we have new data?
 particleSensor.check(); //Check the
sensor for new data
 redBuffer[i] =
particleSensor.getRed();
 irBuffer[i] = particleSensor.getIR();
 particleSensor.nextSample(); //We're
finished with this sample so move to next
sample
 Serial.print(F("red="));
 Serial.print(redBuffer[i], DEC);
 Serial.print(F(", ir="));
 Serial.println(irBuffer[i], DEC);
 }
 //calculate heart rate and SpO2 after
first 50 samples (first 4 seconds of
samples)

maxim_heart_rate_and_oxygen_saturation(irB
uffer, 50, redBuffer, &spo2, &validSPO2,
&heartRate, &validHeartRate);
 //Continuously taking samples from
MAX30102. Heart rate and SpO2 are
calculated every 1 second
 while (1)
 {
 //dumping the first 25 sets of samples
in the memory and shift the last 25 sets of
samples to the top
 for (byte i = 25; i < 50; i++)
 {
 redBuffer[i - 25] = redBuffer[i];
 irBuffer[i - 25] = irBuffer[i];
 }
 //take 25 sets of samples before
calculating the heart rate.
 for (byte i = 25; i < 50; i++)
 {
 while (particleSensor.available() ==
false) //do we have new data?
 particleSensor.check(); //Check
the sensor for new data
 redBuffer[i] =
particleSensor.getRed();
 irBuffer[i] =
particleSensor.getIR();
 particleSensor.nextSample(); //We're
finished with this sample so move to next
sample
 Serial.print(F("red="));
 Serial.print(redBuffer[i], DEC);
 Serial.print(F(", ir="));
 Serial.print(irBuffer[i], DEC);
 Serial.print(F(", HR="));
 Serial.print(heartRate, DEC);
 Serial.print(F(", HRvalid="));
 Serial.print(validHeartRate, DEC);
 Serial.print(F(", SPO2="));
 Serial.print(spo2, DEC);
 Serial.print(F(", SPO2Valid="));
 Serial.println(validSPO2, DEC);
 }
 lcd.setCursor(9,0);
 lcd.print(heartRate);
 lcd.print(" ");
 lcd.setCursor(9,1);
 lcd.print(spo2);
 lcd.print("%");
 lcd.print(" ");


 //After gathering 25 new samples
recalculate HR and SP02

maxim_heart_rate_and_oxygen_saturation(irB
uffer, 50, redBuffer, &spo2, &validSPO2,
&heartRate, &validHeartRate);
 }
}
