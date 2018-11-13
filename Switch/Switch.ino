#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// #### LED Pin Setup #### //
int Switch = D7;
int State = 0;       

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
  pinMode (Switch, INPUT_PULLUP);               //INPUT_PULLUP, Pin is high (3.3V) until low      
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

  while(!client.connect("Switch")){          //LED_board is name of Wemos/arduino connected to code. Waiting to connect to Broker.
    Serial.println("Finding a Connection...");
  }
  client.subscribe(topic);
  Serial.println(topic);
}

void loop() {
  // put your main code here, to run repeatedly:
  client.loop();
  State = digitalRead(Switch);
  if (State == 1){
    client.publish(topic,"on");
  }
  else if (State == 0){
    client.publish(topic,"off");
  }
  Serial.print(topic);
  Serial.print(" ");
  Serial.println(State);
  delay (100);
}
