#include <Wire.h>
#include <ESP8266WiFi.h>

const char* ssid = "ASUS275";
const char* password = "valeriya140912";

// Create an instance of the server specify the port to listen on as an argument
WiFiServer server(80);
// Create an instance of the client
WiFiClient client;

void setup() {
  Serial.begin(9600); /* begin serial for debug */
  Wire.begin(D1, D2); /* join i2c bus with SDA=D1 and SCL=D2 of NodeMCU */

  connectToWiFiAndStartServer();
}

void loop() {
  
// Check if a client has connected
  client = server.available();
  if (!client) {

    /* request & read data of size 6 from slave */
    Wire.requestFrom(8, 6); 
    while(Wire.available()){
      char c = Wire.read();
      Serial.print(c);
    }
    // here could be the code for calling POST method
    // of specific endpoind and sending data obtained from slave

    Serial.println();
    delay(300);
    
  return;
  }
  
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
  // Match the request
  char command;
  if (req.indexOf("/control/f") != -1)
    command = 'f';  
  else if (req.indexOf("/control/b") != -1)
    command = 'b'; 
  else if (req.indexOf("/control/r") != -1)
    command = 'r'; 
  else if (req.indexOf("/control/l") != -1)
    command = 'l'; 
  else if (req.indexOf("/control/s") != -1)
    command = 's';   
  else if (req.indexOf("/sensor/d") != -1)
    command = 'd';     
  else {
    Serial.println("invalid request");
    client.stop();
    return;
  }
  sentCommandToSlave(command);
  String message = "last command: ";
  message += command;
  sentResponseToClient(message);
  
  delay(1);
  Serial.println("Client disonnected");
}

void connectToWiFiAndStartServer(){
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void sentCommandToSlave(char command){
  Wire.beginTransmission(8); /* begin with device address 8 */
  Wire.write(command);
  Wire.endTransmission();    /* stop transmitting */
}

void sentResponseToClient(String message){
// Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\ ";
  s += message;
  s += "</html>\n";
  client.flush();
  client.print(s); /* Send the response to the client */
}
