//Thanks to Adafruit for providing PM sensor code, which was modified and used in this program
#include "DHT.h"
#include <Wire.h>
#include "MutichannelGasSensor.h"
#include <SoftwareSerial.h>
SoftwareSerial pmsSerial(2, 3); //PM sensor
#define DHTPIN 7 
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE);
char test1[1];
int mq2value=0;
int mq5value=0;
int mq9value=0;
int airqualvalue=0;
float temp=0.0;
float humidity=0.0;
float MCNO2=0.0;
float MCCO=0.0;
float MCCH4=0.0;
float MCNH3=0.0;
float MCH2=0.0;
int pm10env=0;
int pm25env=0;
int pm100env=0;
int count03=0;
int count05=0;
int count10=0;
int count25=0;
String combinedDataString;
char sensorData[110];
int test2;
void setup() 
{
  Serial.begin(9600);
  pmsSerial.begin(9600);
  dht.begin();
  gas.begin(0x04);//the default I2C address of the slave is 0x04(multichannel gas sensor)
  gas.powerOn();
  delay(1000);
  pinMode(9,OUTPUT);  //fan
  pinMode(8,OUTPUT);   //PM sensor fan (set to input in order to float the output)
  pinMode(A0,INPUT);  //MQ2
  pinMode(A1,INPUT);  //MQ5
  pinMode(A2,INPUT);  //MQ9
  pinMode(A3,INPUT);  //airqual
  analogWrite(9,255);
  digitalWrite(8,LOW);
}

struct pms5003data 
{
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t unused;
  uint16_t checksum;
};

struct pms5003data data;

void loop() 
{
  if(Serial.available()>0)
  {
      test2=Serial.read();
      switch(test2)
      {
          case 'f':
            fanOn();
            break;
          case 'n':
            fanOff();
            break;
          case 's':
            readSensors();
            break;
      }
  }
  
  if (readPMSdata(&pmsSerial))
  {
    pm10env=data.pm10_env;
    pm25env=data.pm25_env;
    pm100env=data.pm100_env;
    count03=data.particles_03um;
    count05=data.particles_05um;
    count10=data.particles_10um;
    count25=data.particles_25um;
  }
}

boolean readPMSdata(Stream *s) 
{
  if (! s->available()) 
  {
    return false;
  }
 
  if (s->peek() != 0x42) 
  {
    s->read();
    return false;
  }
 
  if (s->available() < 32) 
  {
    return false;
  }
    
  uint8_t buffer[32];    
  uint16_t sum = 0;
  s->readBytes(buffer, 32);
 
  for (uint8_t i=0; i<30; i++) 
  {
    sum += buffer[i];
  }
 
  /* debugging
  for (uint8_t i=2; i<32; i++) 
  {
    Serial.print("0x"); Serial.print(buffer[i], HEX); Serial.print(", ");
  }
  Serial.println();
  */
 
  uint16_t buffer_u16[15];
  for (uint8_t i=0; i<15; i++) 
  {
    buffer_u16[i] = buffer[2 + i*2 + 1];
    buffer_u16[i] += (buffer[2 + i*2] << 8);
  }

  memcpy((void *)&data, (void *)buffer_u16, 30);
 
  if (sum != data.checksum) 
  {
    return false;
  }

  return true;
}

void fanOn()
{
  analogWrite(9,30);
  pinMode(8,INPUT);
}

void fanOff()
{
  analogWrite(9,255); 
  pinMode(8,OUTPUT);
  digitalWrite(8,LOW);
}

void readSensors()
{
   mq2value=analogRead(A0);
   mq5value=analogRead(A1);
   mq9value=analogRead(A2);
   airqualvalue=analogRead(A3);
   temp=dht.readTemperature(true);
   humidity=dht.readHumidity();
   MCNO2=gas.measure_NO2();
   MCCO=gas.measure_CO();
   MCCH4=gas.measure_CH4();
   MCNH3=gas.measure_NH3();
   MCH2=gas.measure_H2();
   combinedDataString=String(mq2value)+","+String(mq5value)+","+String(mq9value)+","+String(airqualvalue)+","+String(temp)+","+String(humidity)+","+String(MCNO2)+","+String(MCCO)+","+String(MCCH4)+","+String(MCNH3)+","+String(MCH2)+","+String(pm10env)+","+String(pm25env)+","+String(pm100env)+","+String(count03)+","+String(count05)+","+String(count10)+","+String(count25);

combinedDataString.toCharArray(sensorData,110);
 
Serial.write(sensorData,110);
}
