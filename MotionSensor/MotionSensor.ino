#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <PubSubClient.h>


const int Ping = D7; //trigger - sends sound out to measure distance
const int Receive = D5; //Echo - Receives sound from trigger to measure travel time (microsec)
long TimeTravel;   //2 bit integer
int distance;      //1 bit integer


// #### MQTT Server connection Setup - Raspberry Pi Broker #### //
char* mqtt_server = "192.168.43.40";  
int mqtt_port = 1883;  
char* topic = "Distance Sensor and Reed Switch";

WiFiClient Wifi;            //Setup Wifi object 
PubSubClient client(Wifi);  //Object that gives you all the MQTT functionality, access objects in PubSubClient Library


// ##### Wifi Connection Setup #### //
char WifiName[] = "Verizon-SM-G935V";            //SSID
char Password[] = "password";

void Msg_rcv(char* topic, byte* payload, unsigned int length){     //Unsigned int = Positive numbers (more range)
  Serial.println ("Message Received");
}

void setup() {
  // put your setup code here, to run once:
  pinMode (Ping, OUTPUT);  
  pinMode(Receive, INPUT);    
  Serial.begin(9600);       //Begin serial monitor
  
  client.setServer(mqtt_server, mqtt_port);           
  client.setCallback(Msg_rcv);                   //Send payload to function (Msg_rcv)

  // ### Begin Connection to Wifi ### //
  WiFi.begin(WifiName,Password);
  while (WiFi.status() !=WL_CONNECTED){          //If not connected to Wifi, delay until connected
    delay (2000);
    Serial.println("Finding a Connection...");
  }

  Serial.println("Connection Started");         //Begin Connection to Wifi
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());               //IP assigned to Server by host wifi

  while(!client.connect("Motion_Sensor")){          //LED_board is name of Wemos/arduino connected to code. Waiting to connect to Broker.
    Serial.println("Finding a Connection...");
  }
  //client.subscribe(topic);
  Serial.println(topic);
}

void loop() {
  // put your main code here, to run repeatedly:
  client.loop();

  if(!client.connected()){
    client.connect("Motion_Sensor");
    client.publish(topic, "Reconnected");
  }

  // ### Activate Sensor ### //
  
  digitalWrite(Ping,LOW);    //Clear
  delayMicroseconds (2);
  digitalWrite(Ping,HIGH);    //Send out sound
  delayMicroseconds (10);       
  digitalWrite(Ping,LOW);     //Clear again

  // ### Convert time to cm ### //
  
  TimeTravel = pulseIn (Receive,HIGH);       //Get the input of Receive
  distance = (TimeTravel*0.034)/2;          //converts time to cm. If want a more precise reading, do an average of 3 readings.
  if (distance > 0){                        // in cm
    if (distance > 10){
      Serial.println("Green - Too far");
      client.publish(topic,"green");  //Server is 192.168.43.177. Setup GET request to send to this
      delay (100);
    }
    else if (distance < 10 && distance > 5){
      Serial.println("Yellow - Getting Close");
      client.publish(topic,"yellow");
      delay (100);
    }
    else if (distance == 5) {
      Serial.println ("Red - Perfect");
      client.publish(topic,"red");
      delay (100);
    }
    else if (distance < 5){
      Serial.println ("Flash Green and Yellow - Too close");
      client.publish(topic,"green");
      delay (300);
      client.publish(topic,"yellow");
      delay (300);
      //client.publish(topic,"green");
      //delay (100);
    }
  }
  //delay(500);
}

/// ## SOURCES ## ///
//https://www.instructables.com/id/How-to-Use-MQTT-With-the-Raspberry-Pi-and-ESP8266/
//Andrew Thomas, friend, M.S. candidate
