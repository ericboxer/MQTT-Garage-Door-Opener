
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>


WiFiUDP udp;

// Update these with values suitable for your network.


// .:: Setup the internets ::.
const char* ssid = "YOUR SSID HERE";
const char* password = "YOUR PASSWORD";
IPAddress broadcast(255, 255, 255, 255);
IPAddress lappy(10, 0 , 1, 101);
IPAddress thisIP(10, 0, 1, 125);
IPAddress thisGateway(10, 0, 1, 1);
IPAddress thisSubnet(255, 255, 255, 0);
IPAddress thisDNS(10, 0, 1, 1);


// .:: Setup the MQTT stuffs ::.
//const char* mqtt_server = "";
//const char* mqtt_user = "";    
//const char* mqtt_password = "";
//int mqtt_port = 1883;
const char* mqtt_clied_id = "Garage Door";
const char* mqtt_server = "SERVER ADDRESS";
const char* mqtt_user = "SERVER USER";    
const char* mqtt_password = "SERVER PASSWORD";
int mqtt_port = 19214;
const char* garageCommand = "boxhouse/garage/door/set"; // Where does the garage listen?
const char* garageState = "boxhouse/garage/door"; // Where does the door post it's status to... Facebook is so yesterday 
const char* generalStats = "boxhose/stats"; // Uptime, RSSI, all that fun stuff

// .:: Open Sesame ::.
String openPassword = "NOT SHARING THIS"; // Randomly generated thanks to 1Password

// .:: Use DHCP or a Static Address ::.
#define useDHCP true // Decisions, decisions...

// .:: Pin Assignments! ::.
#define relayPin 4
#define ledPin 0
#define doorPin 5

// .:: Random Global Variables ::.
const char* theState = "closed";
const char* theLastState = "";
long lastMsg = 0;
char msg[50];
int value = 0;

// .:: Create a MQTT Client Thingy ::.
WiFiClient espClient;
PubSubClient client(espClient);



// .:: MARK: WiFi Setup ::.

void setup_wifi() {
  delay(10);

// .:: Display stuff to pretend we are connecting the Wifi ::.
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

// .:: DHCP selection ::.
  if (useDHCP != true) {
    WiFi.config(thisIP, thisSubnet, thisGateway, thisDNS, thisDNS);
  }
  
  // .:: Actually connect ot the Wifi ::.
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

// .:: Print stuff that says we've connected to the Wifi ::.
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


// .:: MARK: Toggle Relay. Really useful for opening doors and such ::.
// .:: TODO: Use an interrupt ::.
void toggleRelay(int thePin) {
  digitalWrite(thePin, HIGH);
  delay(200);
  digitalWrite(thePin, LOW);
}


// .:: Rack em up... ::.
void setup() {
  // .:: Initialize the relay ::.
  pinMode(relayPin, OUTPUT);
  // .:: Initialize the status LED ::.
  pinMode(ledPin, OUTPUT);
  // .:: Initialize the door stats reed switch ::.
  pinMode(doorPin, INPUT_PULLUP); // Pullup keeps a constant reading on the pin. conntect the othe end to ground. 

  // .:: Old school modem sounds ::.
  Serial.begin(115200);
  
  // .:: Make Wifi happen ::.
  setup_wifi();

  // .:: Make MQTT happen ::.
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}


// .:: MARK: Callback (Do something when a topic changes) ::.
// .:: TODO: Make it more gooder. It's a mess ::.
void callback(char* topic, byte* payload, unsigned int length) {
  // .:: Mail time! ::.
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String fullPayload ="";
  // .:: It's a Little Orphan Annie secrect messge... lets string it together! ::.
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    fullPayload = fullPayload+(char)payload[i];
  }
  // .:: Was the password correct? ::.
  if(fullPayload == openPassword){
      Serial.println("The door was triggered");
      toggleRelay(relayPin);
      Serial.println("trigger complete");
      // .:: Avoid accidental infinite loop ::.
      client.publish(garageCommand, "null", true);
    } // else { make lots of sirens to deter people; }

  Serial.println(fullPayload);
  Serial.println();
}


// .:: MARK: Do something if the connection to the MQTT server is lost ::.
// .:: TODO: Clean this up before supper. ::.
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(mqtt_clied_id, mqtt_user, mqtt_password)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(generalStats, "I'm back");
      // ... and resubscribe
      client.subscribe(garageCommand);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// .:: Send a message to a MQTT Topic. Retain always. ::.
char sendMQTT(const char* topic, const char* message){
    client.publish(topic, message, true);
}



// .:: MARK: Loop - Make it go! ::.
void loop() {
  // .:: Keep the connection alive ::.
  if (!client.connected()) {
    reconnect();
  }

  // .:: Do your MQTT thing. ::.
  client.loop();

  // .:: Send an update to the MQTT server every second ::.
  // .:: TODO: Make more gooder ::.
  // .:: 1. Don't flood the network if you dont have to. And you don't have to. ::.
  long now = millis();
  if (now - lastMsg > 1000) {
    lastMsg = now;
    ++value;
    Serial.println(digitalRead(doorPin));
    switch (digitalRead(doorPin)) {
      case 0:
        theState = "closed";
        break;
      case 1:
        theState = "open";
        break;
    }
    sendMQTT(garageState, theState);
  }

    
  }
