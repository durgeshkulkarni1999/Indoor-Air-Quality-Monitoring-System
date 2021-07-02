#define BLYNK_PRINT Serial    
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include "MQ135.h"

#include<ThingSpeak.h>

WiFiClient client;

long myChannelNumber= ; // write your channel number provided by thingspeak
const char myWriteAPIKey[]=""; // write your writeapikey provided by thingspeak

char auth[] = "";// Write your auth code provided by blynk
char ssid[] = "";  //Enter your WIFI Name
char pass[] = "";  //Enter your WIFI Password

 Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */

 WiFiServer server(80);

void setup(void) 
{
  Serial.begin(9600);
  Wire.begin(D2,D1);
  Blynk.begin(auth, ssid, pass);
  WiFi.begin(ssid,pass);
  //Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV/ADS1015, 0.1875mV/ADS1115)");
  ThingSpeak.begin(client);

  //Serial.println("Connecting to ");
  //lcd.print("Connecting.... ");
  //Serial.println(ssid);
  //WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
    //lcd.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP Address: ");
  delay(5000);
  Serial.println(WiFi.localIP());
  delay(5000);
  server.begin();
  
  ads.begin();
}

void loop(void) 
{
  int16_t adc0, adc1, adc2, adc3;
  float g0,g1,g2,g3,sum,AQI;
  Blynk.run();
  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  adc2 = ads.readADC_SingleEnded(2);
  adc3 = ads.readADC_SingleEnded(3);

  Serial.println("************************");
  float m = -0.44; //Slope 
  float b = 0.53;//0.53 Y-Intercept 
  float R0 = 0.2; //Sensor Resistance kiloohm in fresh air from previous code

  float MD=(log(R0)*m)*15*b;
  
  Serial.print("Combustible_steam_MQ2= ");
  g0=adc0/MD;
  if(g0<0)
  {
    Serial.println(0);
    Blynk.virtualWrite(V0,g0);
    ThingSpeak.writeField(myChannelNumber,1,g0,myWriteAPIKey);
  }
  else
  {
    Serial.println(g0);
    Blynk.virtualWrite(V0,g0);
    ThingSpeak.writeField(myChannelNumber,1,g0,myWriteAPIKey);
  }
  

  
  float m1 = -0.6527; //Slope 
  float b1 = 1.30; //Y-Intercept 
  float R01 = 0.2; //Sensor Resistance

  float MD1=(log(R01)*m1)*b1*17;

    //Serial.print("CO_MQ_7= ");
  Serial.println("adc1=");
  Serial.println(adc1);
  if(adc1<0)
  {
    adc1=0;
    g1=10;
    Serial.println(g1);
    Blynk.virtualWrite(V1,g1);
    ThingSpeak.writeField(myChannelNumber,2,g1,myWriteAPIKey);
    
  }
  else
  {
     g1=adc1/MD1;
     Serial.println(g1);
     Blynk.virtualWrite(V1,g1);
     ThingSpeak.writeField(myChannelNumber,2,g1,myWriteAPIKey);
     
  }
    
  float m2 = -0.754; //Slope 
  float b2 = 1.996; //Y-Intercept 
  float R02 = 0.2; //Sensor Resistance 

  float MD2=(log(R02)*m2)*b2*15;
   
  
  Serial.print("Alcohol_MQ_3= ");
  g2=adc2/MD2;
  if(g2<0)
  {
    g2=0;
    Serial.println(g2);
    Blynk.virtualWrite(V2,g2);
    ThingSpeak.writeField(myChannelNumber,3,g2,myWriteAPIKey);
  }
  else
  {
    Serial.println(g2);
    Blynk.virtualWrite(V2,g2);
    ThingSpeak.writeField(myChannelNumber,3,g2,myWriteAPIKey);
  }
  

    


  float m3 = -0.461; //Slope 
  float b3 = 1.38; //Y-Intercept 
  float R03 = 0.1; //Sensor Resistance

  float MD3=(log(R03)*m3)*b3*16;
  
  Serial.print("LPG_MQ_9= ");
  g3=0;
  if(adc3<0)
  {
    Serial.println(g3);
    Blynk.virtualWrite(V3,g3);
    ThingSpeak.writeField(myChannelNumber,4,g3,myWriteAPIKey);
  }
  else
  {
    g3=adc3/MD3;
    if(g3>50)
    {
      String body=String("LPG gas outage detected with PPM= ")+g3+"\nPlease be careful open all windows ";
      Blynk.email("abhijeet2thorat@gmail.com","LPG Leakage ALERT",body);
      Serial.println(g3);
      Blynk.virtualWrite(V3,g3);
      ThingSpeak.writeField(myChannelNumber,4,g3,myWriteAPIKey);
    }
    else
    {
    Serial.println(g3);
    Blynk.virtualWrite(V3,g3);
    ThingSpeak.writeField(myChannelNumber,4,g3,myWriteAPIKey);
    }
  }
  //Serial.println(adc3/3);
  Serial.println(" ");

  MQ135 gasSensor = MQ135(A0);
  double ppm4 = gasSensor.getPPM();
  float g4 = ppm4+15;             
  Serial.print ("CO2 PPM: ");
  if(g4<0)
  {
    g4=0;
    Serial.println(g4);
    Blynk.virtualWrite(V4,g4);
    ThingSpeak.writeField(myChannelNumber,5,g4,myWriteAPIKey);
  }
  else
  {
    Serial.println(g4);
    Blynk.virtualWrite(V4,g4);
    ThingSpeak.writeField(myChannelNumber,5,g4,myWriteAPIKey);
  }
 

  sum=g0+g1+g2+g3+g4;
  AQI=sum/20;
  Blynk.virtualWrite(V5,AQI);
  ThingSpeak.writeField(myChannelNumber,6,AQI,myWriteAPIKey);

  Serial.print("AQI=");
  Serial.println(AQI);

  Serial.println("************************************************************************************************************************************");
  
// Here we have not included html code you can watch tutorial from youtube for interfacing of html page with nodemcu.
