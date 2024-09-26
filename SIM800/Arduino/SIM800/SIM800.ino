#include <SoftwareSerial.h>

// Define a SoftwareSerial object for communication with SIM800L module
SoftwareSerial mySerial(3, 2); // Rx pin: 3, Tx pin: 2

void setup() {
  // Start serial communication with the monitor
  Serial.begin(9600);
  // Start serial communication with the SIM800L module
  mySerial.begin(9600);

  Serial.println("Initializing..."); // Display "Initializing..." message on the serial monitor
  delay(1000);

  // Send AT command to test communication with the SIM800L module
  sendATCommand("AT");

  // Set SMS text mode
  sendATCommand("AT+CMGF=1");

  // Set SMS notification to immediate format
  sendATCommand("AT+CNMI=1,2,0,0,0");
}

void loop() {
  updateSerial(); // Update serial communication
}

// Function to send AT command and wait for "OK" response
bool sendATCommand(String command) {
  mySerial.println(command); // Send AT command to SIM800L module
  return waitForResponse("OK", 1000); // Wait for "OK" response within 1 second timeout
}

// Function to wait for and receive response from SIM800L module
bool waitForResponse(String response, unsigned long timeout) {
  unsigned long startTime = millis(); // Start time for waiting
  String reply; // Variable to store received response from module
  
  while (millis() - startTime < timeout) {
    while (mySerial.available()) {
      char c = mySerial.read();
      reply += c; // Append each character to the reply string
    }
    if (reply.indexOf(response) != -1) {
      return true; // Desired response received
    }
  }
  
  return false; // Timeout occurred
}

// Function to update serial communication between Arduino and SIM800L module
void updateSerial() {
  if (Serial.available()) {
    while (mySerial.availableForWrite() && Serial.available()) {
      mySerial.write(Serial.read());
    }
  }

  if (mySerial.available()) {
    while (Serial.availableForWrite() && mySerial.available()) {
      Serial.write(mySerial.read());
    }
  }
}
