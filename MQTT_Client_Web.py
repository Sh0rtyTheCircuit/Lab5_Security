#import os
import paho.mqtt.client as mqtt
import time
import socket
import select
import sys
import threading

#Channel Topic
sensors = "Sensors"
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
        if (str(msg.payload) == "close"):
				LED_color == "close"
		elif (str(msg.payload) == "open"):
				LED_color == "open"
		elif (str(msg.payload) == "on"):
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

## CREATE WEBSERVER ##

HOST, PORT = '', 9898 

listener = socket.socket(socket.AF_INET, socket.SOCK_STREAM)	#socket setup
listener.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
listener.bind((HOST,PORT))					#Where the server is now located
listener.listen(1)						#Listen for this many clients

print 'Listener is set up'
stopper = threading.Event()					#Handler for starting and stopping the thread

while True:
	client_connection, client_address = listener.accept()	#Accept connection from user 

	request = client_connection.recv(1024)			#Waiting for GET REQUEST(refresh/load website). Request is new request now.
	print request						#Request = whatever is Posted when btns are pressed
	check_status = request[0:13]					#Look at URL in the GET REQUEST
	print check_status
	if check_status.find("close")>0:			#Look for "Green" in request. If not found, will return -1 which breaks the code
		stopper.set()
		client.publish(sensors,"close")
	if check_status.find("open")>0:
		stopper.set()
		client.publish(sensors,"open")
		
	disp_body = """\
<html>
	<title> Choose Wisely </title>
	<body>

		<form action="http://localhost:9898/off" method= "post">
			<button>Close</button>
		</form>
	<br>
		<form action="http://localhost:9898/on" method= "post">
			<button> Open </button>
		</form>
	<br>
	</body>

</html>	
	"""
        display = """\
HTTP/1.1 302 OK
Content-Type: text/html
Content-Length: %d
Connection: close  
""" % len(disp_body)

	client_connection.sendall(display + disp_body)
	client_connection.close()

#Predefined functions
client.loop_forever()   #Client will keep itself alive
client.disconnect()     #Disconnect before dying (cntrl C or kill)