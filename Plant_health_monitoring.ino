#include <SoftwareSerial.h>
#include<Servo.h>
//const int sensor_pin = A1;
#include "DHT.h"
#define RX 2
#define TX 3
#define dht_apin 11 // Analog Pin sensor is connected to
DHT dhtObject;
String AP = "realme";       // AP NAME
String PASS = "0987654321"; // AP PASSWORD
String API = "E3C5QCT1IM9DDV8F";   // Write API KEY
String HOST = "api.thingspeak.com";
String PORT = "80";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
//int valSensor = 1;
  
Servo myservo;
SoftwareSerial esp8266(RX,TX); 

 void check_temperature();
 void check_moisture();
  
void setup() 
{
  Serial.begin(9600);
  esp8266.begin(115200);
  pinMode(8, OUTPUT);
  pinMode(A0, INPUT);
  dhtObject.setup(11);
  myservo.attach(9);
  digitalWrite(8,HIGH);
  myservo.write(180);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
  delay(500);
}

void loop()
{
  
  int moisture_output;
  moisture_output = analogRead(A0);
  moisture_output= map(moisture_output,550,10,0,100);
  check_temperature();
  check_moisture();
  String sendData = "GET /update?api_key="+ API +"&field1="+getTemperatureValue()+"&field2="+getHumidityValue()+"&field3="+ moisture_output;
  sendCommand("AT+CIPMUX=1",5,"OK");
  sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
  sendCommand("AT+CIPSEND=0," +String(sendData.length()+4),4,">");
  esp8266.println(sendData);
  //Serial.print( moisture_output);
  //Serial.print("%\n\n");
  //delay(800);
  countTrueCommand++;
  sendCommand("AT+CIPCLOSE=0",5,"OK");
 
}


String getTemperatureValue()
{

   //delay(dhtObject.getMinimumSamplingPeriod());
   int temperature = dhtObject.getTemperature();
   //Serial.print(" Temperature(C)= ");
   //Serial.println(temperature); 
   return String(temperature); 
  
}


String getHumidityValue()
{

   //delay(dhtObject.getMinimumSamplingPeriod());
   int humidity = dhtObject.getHumidity();
  // Serial.print(" Humidity in %= ");
   //Serial.println(humidity);
   return String(humidity); 
  
}

void sendCommand(String command, int maxTime, char readReplay[])
 {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }

void check_temperature()
{
  int temp = dhtObject.getTemperature();
  Serial.print("Temperature = ");
  Serial.print(temp);
  Serial.print("\n");
  if(temp > 35)
  {
   myservo.write(0); 
  }
  
  else
  {
    myservo.write(180);
    
  }
}


 void check_moisture()
{
 int moist = analogRead(A0);
  moist= map(moist,550,10,0,100);
  Serial.print("Moisture = ");
  Serial.print(moist);
  Serial.print("%");
  Serial.print("\n");
  if(moist < 20)
  {
    digitalWrite(8, LOW);
  
  }
  else
  {
    digitalWrite(8,HIGH);
    
  }
}
