#include <WiFi.h>
#include <SPI.h>
#include <MFRC522.h>

static const int saidareleporta = 13;
static const int botao = 12;
static const int buzzer = 26;
static const int amarelo = 27;
static const int verde = 14;

WiFiServer server(80);
MFRC522 mfrc522(5, 21);

String header;
bool portaEstaFechada = true;

unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

bool rfid_tag_present_prev = false;
bool rfid_tag_present = false;
int _rfid_error_counter = 0;
bool _tag_found = false;



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(saidareleporta, OUTPUT);
  pinMode(botao, INPUT_PULLUP);
  pinMode(amarelo, OUTPUT);
  pinMode(verde, OUTPUT);
  digitalWrite(amarelo, HIGH);
  WiFi.softAP("portafoda", "toctocqueme");
  server.begin();
  SPI.begin();
	mfrc522.PCD_Init();	
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!digitalRead(botao)){
    mudarEstadoPorta();
    while (!digitalRead(botao)){
      delay(250);
      }
  }

  wifiLoop();

    rfid_tag_present_prev = rfid_tag_present;

  _rfid_error_counter += 1;
  if(_rfid_error_counter > 2){
    _tag_found = false;
  }

  // Detect Tag without looking for collisions
  byte bufferATQA[2];
  byte bufferSize = sizeof(bufferATQA);

  // Reset baud rates
  mfrc522.PCD_WriteRegister(mfrc522.TxModeReg, 0x00);
  mfrc522.PCD_WriteRegister(mfrc522.RxModeReg, 0x00);
  // Reset ModWidthReg
  mfrc522.PCD_WriteRegister(mfrc522.ModWidthReg, 0x26);

  MFRC522::StatusCode result = mfrc522.PICC_RequestA(bufferATQA, &bufferSize);

  if(result == mfrc522.STATUS_OK){
    if ( ! mfrc522.PICC_ReadCardSerial()) { //Since a PICC placed get Serial and continue   
      return;
    }
    _rfid_error_counter = 0;
    _tag_found = true;        
  }
  
  rfid_tag_present = _tag_found;
  
  // rising edge
  if (rfid_tag_present && !rfid_tag_present_prev){
    Serial.println("Tag found");
  }
  
  // falling edge
  if (!rfid_tag_present && rfid_tag_present_prev){
    Serial.println("Tag gone");
    mudarEstadoPorta();

  }
}

void mudarEstadoPorta(){
  if (portaEstaFechada){
    digitalWrite(saidareleporta, HIGH);
    digitalWrite(amarelo, LOW);
    digitalWrite(verde, HIGH);
    portaEstaFechada = false;
  }
  else{
    digitalWrite(saidareleporta, LOW);
    digitalWrite(amarelo, HIGH);
    digitalWrite(verde, LOW);
    portaEstaFechada = true;
  }
  tone(buzzer, 500, 500);
}


void wifiLoop(){
  WiFiClient client = server.available();
  if (client) {                             // If a new client connects,
      currentTime = millis();
      previousTime = currentTime;
      Serial.println("New Client.");          // print a message out in the serial port
      String currentLine = "";                // make a String to hold incoming data from the client
      while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
        currentTime = millis();
        if (client.available()) {             // if there's bytes to read from the client,
          char c = client.read();             // read a byte, then
          Serial.write(c);                    // print it out the serial monitor
          header += c;
          if (c == '\n') {                    // if the byte is a newline character
            // if the current line is blank, you got two newline characters in a row.
            // that's the end of the client HTTP request, so send a response:
            if (currentLine.length() == 0) {
              // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
              // and a content-type so the client knows what's coming, then a blank line:
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println();
              
              // turns the GPIOs on and off
              if (header.indexOf("GET /mudarestadoporta") >= 0) {
                mudarEstadoPorta();
              }
              
              // Display the HTML web page
              client.println("<!DOCTYPE html><html>");
              client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
              client.println("<link rel=\"icon\" href=\"data:,\">");
              // CSS to style the on/off buttons 
              // Feel free to change the background-color and font-size attributes to fit your preferences
              client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
              client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
              client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
              client.println(".button2 {background-color: #555555;}</style></head>");
              
              // Web Page Heading
              client.println("<body><h1>ESP32 Web Server</h1>");
              if (portaEstaFechada){
                client.println("<p>Porta fechada</p>");
              }
              else{
                client.println("<p>Porta aberta</p>");
              }
              client.println("<a href=\"/mudarestadoporta\">Acionar porta</a>");
              client.println("</body></html>");
              
              // The HTTP response ends with another blank line
              client.println();
              // Break out of the while loop
              break;
            } else { // if you got a newline, then clear currentLine
              currentLine = "";
            }
          } else if (c != '\r') {  // if you got anything else but a carriage return character,
            currentLine += c;      // add it to the end of the currentLine
          }
        }
      }
      // Clear the header variable
      header = "";
      // Close the connection
      client.stop();
      Serial.println("Client disconnected.");
      Serial.println("");
    }
}