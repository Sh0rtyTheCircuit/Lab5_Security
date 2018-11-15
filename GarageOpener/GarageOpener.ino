#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

// #### LED Pin Setup #### //
int NO = D8;
int COM = D5;
int NC = D1;       

// #### MQTT Server connection Setup - Raspberry Pi Broker #### //
char* mqtt_server = "192.168.43.40";  
int mqtt_port = 1883;
char* topic = "GarageOpener";  
char* state = " ";

WiFiClient Wifi;            //Setup Wifi object 
PubSubClient client(Wifi);  //Object that gives you all the MQTT functionality, access objects in PubSubClient Library

// ##### Wifi Connection Setup #### //
char WifiName[] = "Verizon-SM-G935V";            //SSID
char Password[] = "password";

void Msg_rcv(char* topic, byte* payload, unsigned int length){     //Unsigned int = Positive numbers (more range)
  Serial.print ("payload is ");
  Serial.println((char)payload[0]);
  if ((char) payload[0] == 'o'){
    if ((char) payload[1] == 'p'){
      digitalWrite(COM,LOW);
      Serial.println("COM port is LOW");
     }
    }
  if ((char) payload[0] == 'c'){
    digitalWrite(COM,HIGH);
    Serial.println("COM port is HIGH");
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode (COM, OUTPUT);               //INPUT_PULLUP, Pin is high (3.3V) until low
  pinMode (NO, INPUT);
  pinMode (NC, INPUT);      
  digitalWrite(COM,LOW);
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

  while(!client.connect("Garage_Opener")){          //LED_board is name of Wemos/arduino connected to code. Waiting to connect to Broker.
    Serial.println("Finding a Connection...");
  }
  client.subscribe(topic);
}

void loop() {
  // put your main code here, to run repeatedly:
  client.loop();
  if(!client.connected()){
    client.connect("GarageOpener");
  }
   if (digitalRead(NC) == HIGH){
    state = "close";
    Serial.println(state);
  }
  else if (digitalRead(NC) == LOW){
    state = "open";
    Serial.println(state);
    
  }
  //client.publish(topic, state);
  Serial.println(state);
  delay(1000);
}
