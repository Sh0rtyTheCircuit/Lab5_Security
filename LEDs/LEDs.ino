#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>


// #### LED Pin Setup #### //
int GREEN = D5; 
int YELLOW = D7;
int RED = D6;

// ### Sensor Variables sent ### //
char* LED_pwr = "On";
char* LED_color = "Green";

// #### MQTT Server connection Setup - Raspberry Pi Broker #### //
char* mqtt_server = "192.168.43.40";  
int mqtt_port = 1883;  

WiFiClient Wifi;            //Setup Wifi object 
PubSubClient client(Wifi);  //Object that gives you all the MQTT functionality, access objects in PubSubClient Library

// ##### Wifi Connection Setup #### //
char WifiName[] = "Verizon-SM-G935V";            //SSID
char Password[] = "password";

// #### LED Functions Setup #### //
void TurnGREEN(){
  AllClear();
  digitalWrite(GREEN,HIGH);
  Serial.println("GREEN");
}

void Flash(){
  digitalWrite(GREEN,HIGH);
  delay (200);
  digitalWrite(GREEN,LOW);
  delay (200);
  digitalWrite(GREEN,HIGH);
  delay (200);
  digitalWrite(GREEN,LOW);
  delay (200);
  digitalWrite(GREEN,HIGH);
  delay (200);
  digitalWrite(GREEN,LOW);
  delay (200);
}

void TurnYELLOW(){
  //TurnOFF();
  AllClear();
  digitalWrite(YELLOW,HIGH);
  Serial.println("YELLOW");
}

void TurnRED(){
  AllClear();
  digitalWrite(RED,HIGH);
  Serial.println("RED");
}

void AllClear(){
  digitalWrite(GREEN,LOW);
  digitalWrite(YELLOW,LOW);
  digitalWrite(RED,LOW);
}

// ### Look through payload (Message Sent) for LED instructions ### //

void Msg_rcv(char* topic, byte* payload, unsigned int length){     //Unsigned int = Positive numbers (more range)
  Serial.println((char)payload[1]);
  if ((char) payload[0] == 'o'){
    if ((char) payload[1] == 'n'){
      LED_pwr = "On";
    }
    else{
      LED_pwr = "Off";
      AllClear();
    }
  }
  else if ((char) payload[0] == 'g' && LED_pwr == "On"){
    //LED_color = "Green";
    TurnGREEN();
  }
  else if ((char) payload[0] == 'y' && LED_pwr == "On"){
    //LED_color = "Yellow";
    TurnYELLOW();
  }
  else if ((char) payload[0] == 'r' && LED_pwr == "On"){
    LED_color = "Red";
    TurnRED();
  }
  //Turn_color();
}

// ### Call the functions according to the payload and LED power being on or off ### //

void Turn_color(){
  if (LED_pwr == "On"){
    if (LED_color == "Green"){
      TurnGREEN();
    }
    else if (LED_color == "Red"){
      TurnRED();
    }
    else if (LED_color == "Yellow"){
      TurnYELLOW();
    }
    Serial.print (LED_color);
    Serial.println (" is on");
    }
   else{
    AllClear();
    Serial.println ("Power is off");  
   }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(GREEN,OUTPUT);
  pinMode(YELLOW,OUTPUT);
  pinMode(RED,OUTPUT);
  Serial.begin(9600);                          //Starts the Serial Monitor (Input printed on screen)
  
  client.setServer(mqtt_server, mqtt_port);           
  client.setCallback(Msg_rcv);                   //Send payload to function (Msg_rcv)
  
  WiFi.begin(WifiName,Password);
  while (WiFi.status() !=WL_CONNECTED){          //If not connected to Wifi, delay until connected
    delay (2000);
    Serial.println("Finding a Connection...");
  }

  Serial.println("Connection Started");         //Begin Connection to Wifi
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());               //IP assigned to Server by host wifi

  while(!client.connect("LED_board")){          //LED_board is name of Wemos/arduino connected to code. Waiting to connect to Broker.
    Serial.println("Finding a Connection...");
  }
  client.subscribe("LED");
  Serial.println("Subscribed to LED");
}

void loop() {                                // put your main code here, to run repeatedly:
  client.loop();                             // pre-made function. Connection to MQTT run continuosly (Constantly listening)
  if(!client.connected()){
    client.connect("LED_board");
  }
  //Turn_color();
}


/// ## SOURCES ## ///
//https://www.instructables.com/id/How-to-Use-MQTT-With-the-Raspberry-Pi-and-ESP8266/
//Andrew Thomas, friend, M.S. candidate
