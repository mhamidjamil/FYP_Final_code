#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

SoftwareSerial ESP32_listener(16, 5);  // for 9600 baud rate
SoftwareSerial ESP32_listener_2(4, 0); // for 115200 baud rate

// Configuration parameters for Access Point
char *ssid_ap = "Edge_network_2";
char *password_ap = "123456789";
IPAddress ip(192, 168, 11,
             4); // arbitrary IP address (doesn't conflict w/ local network)
IPAddress gateway(192, 168, 11, 1);
IPAddress subnet(255, 255, 255, 0);

// Set up the server object
ESP8266WebServer server;

// Keep track of the sensor data that's going to be sent by the client
String sensor_value;

void blynk(int times_);
// 3a2v3Vlb5UD6RFx0Eyg3@35.154!Spine_position!2.151,450670.54,2.12!2.79!#
bool Data_validator(String tempstr);
#define USER_ID_LENGTH 20
#define NO_OF_EXCLAMATION 4
#define NO_OF_COMMAS 2
int count_presence(String testStr, char toBeFound);
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(ip, gateway, subnet);
  WiFi.softAP(ssid_ap, password_ap);
  // Print IP Address as a sanity check
  Serial.begin(9600);
  ESP32_listener.begin(9600);
  ESP32_listener_2.begin(115200);
  // Serial.println();
  // Serial.print("IP Address: ");
  // Serial.println(WiFi.localIP());
  // Configure the server's routes
  server.on(
      "/",
      handleIndex); // use the top root path to report the last sensor value
  server.on("/update",
            handleUpdate); // use this route to update the sensor value
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
}

void handleIndex() {
  server.send(200, "text/plain",
              String(sensor_value)); // we'll need to refresh the page for
                                     // getting the latest value
}

void handleUpdate() {
  // The value will be passed as a URL argument
  sensor_value = server.arg("value");
  Serial.println(sensor_value);
  if (Data_validator(sensor_value)) {
    Serial.println("Data is valid (sent to server)");
    ESP32_listener.println(sensor_value);
    ESP32_listener_2.println(sensor_value);
    server.send(200, "text/plain", "Updated");
  } else {
    Serial.println("Data is invalid");
    server.send(200, "text/plain", "Invalid");
  }
  blynk(3);
}
void blynk(int times_) {
  for (; times_ > 0; times_--) {
    pinMode(LED_BUILTIN, HIGH);
    delay(50);
    pinMode(LED_BUILTIN, LOW);
    delay(50);
  }
}

bool Data_validator(String tempstr) {
  // Serial.println("Working on : " + tempstr);
  String user_id = "";
  int first_at = tempstr.indexOf('@');
  if (first_at == USER_ID_LENGTH) {
    user_id = tempstr.substring(0, first_at);
    if (USER_ID_LENGTH == user_id.length()) {
      if (count_presence(tempstr, '!') == NO_OF_EXCLAMATION) {
        if (count_presence(tempstr, ',') == NO_OF_COMMAS) {
          if (count_presence(tempstr, '#') == 1) {
            if (count_presence(tempstr, '@') == 1) {
              return true;
            } else {
              Serial.println("NO_OF_AT : 1, Found in input : " +
                             String(count_presence(tempstr, '@')));
              return false;
            }
          } else {
            Serial.println("NO_OF_HASH : 1, Found in input : " +
                           String(count_presence(tempstr, '#')));
            return false;
          }
        } else {
          Serial.println(
              "NO_OF_COMMAS : " + String(NO_OF_COMMAS) +
              " Found in input : " + String(count_presence(tempstr, ',')));
          return false;
        }
      } else {
        Serial.println(
            "NO_OF_EXCLAMATION : " + String(NO_OF_EXCLAMATION) +
            " Found in input : " + String(count_presence(tempstr, '!')));
        return false;
      }

    } else {
      Serial.println("USER_ID_LENGTH : " + String(USER_ID_LENGTH) +
                     " strlen(user_id) : " + String(user_id.length()));
      return false;
    }
  } else {
    Serial.println("first at : " + String(first_at) +
                   " USER_ID_LENGTH : " + String(USER_ID_LENGTH));
    return false;
  }
  //   return
}
int count_presence(String testStr, char toBeFound) {
  int count = 0;
  for (int i = 0; i < testStr.length(); i++) {
    if (testStr[i] == toBeFound) {
      count++;
    }
  }
  return count;
}
