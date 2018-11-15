import paho.mqtt.client as mqtt
import time
import socket
import select
import sys
import threading

#Channel Topic
sensors = "Sensors"
LED = "LED"
GarageOpener = "GarageOpener"

#ip of localhost
mqtt_broker= "192.168.43.40"
mqtt_port = 1883

Accept_IP = "192.168.43."

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
        print "Subscribed to " + topic
        client.subscribe(GarageOpener)

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

#httpd = BaseHTTPServer.HTTPServer((HOST, PORT), SimpleHTTPServer.SimpleHTTPRequestHandler)
#httpd.socket = ssl.wrap_socket (httpd.socket, keyfile= './key.pem', certfile='./server.pem', server_side=True)
#httpd.serve_forever()

listener = socket.socket(socket.AF_INET, socket.SOCK_STREAM)    #socket setup
listener.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
listener.bind((HOST,PORT))                                      #Where the server is now located
listener.listen(1)                                              #Listen for this many clients

print 'Listener is set up'
stopper = threading.Event()                                     #Handler for starting and stopping the thread

while True:
        client.loop()
        client_connection, client_address = listener.accept()   #Accept connection from user

        request = client_connection.recv(1024)                  #Waiting for GET REQUEST(refresh/load website). Request is new request now.
        print request                                           #Request = whatever is Posted when btns are pressed
        check_status = request[0:13]                                    #Look at URL in the GET REQUEST
        print check_status
        if check_status.find("close")>0:                        #Look for "Green" in request. If not found, will return -1 which breaks the code
                stopper.set()
                print ("closed")
                client.publish(GarageOpener,payload ="close")
        elif check_status.find("open")>0:
                stopper.set()
                print ("open")
                client.publish(GarageOpener,payload ="open")
        if client_address [0][0:11] == Accept_IP:               #If the client IP is in the same network
                disp_body = """\
<html>
        <title> Choose Wisely </title>
        <body>

                <form action="/close" method= "post">
                        <button>Close</button>
                </form>
				<form action="/open" method= "post">
                        <button> Open </button>
                </form>
        <br>
        </body>

</html>
        """
        else:                                                   #If the client IP is in a different network, do not show the page
                disp_body = """\
<html>
        <title> Choose Wisely </title>
        <body>
                <h1> Nice Try </h2>
        <br>
        </body>

</html> """
        display = """\
HTTP/1.1 302 OK
Content-Type: text/html
Connection: close \n
"""
        print (display)
        client_connection.sendall(display + disp_body)
        client_connection.close()
        print ("sent")
#Predefined functions
client.loop_forever()   #Client will keep itself alive
client.disconnect()     #Disconnect before dying (cntrl C or kill)
