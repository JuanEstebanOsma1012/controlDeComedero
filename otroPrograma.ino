#include <Stepper.h>
#include <SoftwareSerial.h>

int stepsPerRevolution = 0;
int motSpeed = 12;
int dt = 1500;

SoftwareSerial esp8266(10, 11);
Stepper pasoAPaso(stepsPerRevolution, 4, 6, 5, 7);

#define serialCommunicationSpeed 9600
#define DEBUG true

void setup(){

  pasoAPaso.setSpeed(motSpeed);

  Serial.begin(serialCommunicationSpeed);
  esp8266.begin(serialCommunicationSpeed);
  InitWifiModule();

}

void loop() {

  if (esp8266.available()) {
    if (esp8266.find("+IPD,")) {
      delay(1000);

      int connectionId = esp8266.read() - 48;
      String webpage = "<!DOCTYPE html><html lang=\"es\"><head><title>Fish feeder</title></head><body><form action=\"#\" method=\"get\"><button type=\"submit\" name=\"step\" value=\"1\">dar de comer</button></form></body></html>";

      if (esp8266.find("step=")) {

        int lectura = esp8266.read() - 48;

        if (lectura == 1) {

          //webpage = "<!DOCTYPE html><html lang=\"es\"><head><title>Fish feeder</title></head><body><form action=\"#\" method=\"get\"><button type=\"submit\" name=\"led\" value=\"0\">apagar</button></form></body></html>";
          //numeroClics++;

          stepsPerRevolution += 256;
          pasoAPaso.step(stepsPerRevolution);
          delay(dt);

        }
      }

      String cipSend = "AT+CIPSEND=";
      cipSend += connectionId;
      cipSend += ",";
      cipSend += webpage.length();
      cipSend += "\r\n";

      sendData(cipSend, 1000, DEBUG);
      sendData(webpage, 1000, DEBUG);

      String closeCommand = "AT+CIPCLOSE=";
      closeCommand += connectionId;  // append connection id
      closeCommand += "\r\n";
      sendData(closeCommand, 3000, DEBUG);
    }
  }
}

String sendData(String command, const int timeout, boolean debug) {
  String response = "";
  esp8266.print(command);
  long int time = millis();
  while ((time + timeout) > millis()) {
    while (esp8266.available()) {
      char c = esp8266.read();
      response += c;
    }
  }
  if (debug) {
    Serial.print(response);
  }
  return response;
}

void InitWifiModule() {
  sendData("AT+RST\r\n", 2000, DEBUG);
  sendData("AT+CWJAP=\"A30\",\"12345678\"\r\n", 2000, DEBUG);
  delay(10000);
  sendData("AT+CWMODE=1\r\n", 1500, DEBUG);
  delay(1500);
  sendData("AT+CIFSR\r\n", 1500, DEBUG);
  delay(1500);
  sendData("AT+CIPMUX=1\r\n", 1500, DEBUG);
  delay(1500);
  sendData("AT+CIPSERVER=1,80\r\n", 1500, DEBUG);
}