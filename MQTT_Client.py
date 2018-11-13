#import os
import paho.mqtt.client as mqtt
import time

#Channel Topic
sensors = "Distance Sensor and Reed Switch"
LED = "LED"

#ip of localhost
mqtt_broker= "192.168.43.40"
mqtt_port = 1883

#Set up MQTT Client object (access to MQTT functions in the library)
#client = mqtt.Client()
#print "MQTT client object is set up"

#Define functions

def msg_rcv(sensors, user_data, msg):   #Interpret Msgs (Loops)
        print "Payload is " + str(msg.payload)
        if (str(msg.payload) == "on"):
                LED_color = "on"
        elif (str(msg.payload) == "off"):
                LED_color = "off"
        elif (str(msg.payload) == "red"): # or str(msg.payload) == "green" or str(msg.payload) == "yellow"):
                LED_color = "red"
#               LED_color = str(msg.payload)
        elif (str(msg.payload) == "yellow"):
                LED_color = "yellow"
        else:
                LED_color = "green"
        sensors.publish (LED, payload = LED_color, qos=0, retain=False) #(channel, msg to publish)
        print (LED_color)

def run_broker(client, user_data, flags, rc):                   #Subscribe to topics (Once)
        print "In the broker function"
        client.subscribe(sensors)                       #Listen to the Sensors channel
        print "Subscribed to "
        print (sensors)

client = mqtt.Client()

#When message is received, run msg_rcv function
client.on_message = msg_rcv

#When connected to Broker, run run_broker function
client.on_connect = run_broker

#Begin connection to MQTT Broker
client.connect(mqtt_broker,mqtt_port)
print "connection to broker started"

#while not client.connect(mqtt_broker,mqtt_port):
#       print "Finding a connection"
#       time.sleep(.5)

#Predefined functions
client.loop_forever()   #Client will keep itself alive
client.disconnect()     #Disconnect before dying (cntrl C or kill)