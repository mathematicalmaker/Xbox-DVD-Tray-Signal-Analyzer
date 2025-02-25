#include <Arduino.h>
#include <SoftwareSerial.h>

// Define Software Serial pins
const int rxPin = A0;
const int txPin = A1;
SoftwareSerial mySerial(rxPin, txPin);

// Define the input pins
const int statePin0 = 5;
const int statePin1 = 6;
const int statePin2 = 7;
const int ejectPin = 4; 

// Variables to store the current and previous states
int currentState = 0;
int previousState = -1; // Initialize to an impossible state
int currentEjectState = HIGH;
int previousEjectState = HIGH;

// Array to store state descriptions.  Make this const so it's in flash memory.
const char* stateDescriptions[] = {
  "000 - Closed - Detecting",
  "001 - Tray Open",
  "010 - Unloading",
  "011 - Opening",
  "100 - Empty - No Media",
  "101 - Closing",
  "110 - Media Detected",
  "111 - ???"
};

// put function declarations here:
// int myFunction(int, int);

void setup() {
  // Initialize Software Serial communication
  mySerial.begin(19200); // Or whatever baud rate your device uses

  // Set the input pins as inputs - NO PULLUPS
  pinMode(statePin0, INPUT);
  pinMode(statePin1, INPUT);
  pinMode(statePin2, INPUT);
  pinMode(ejectPin, INPUT); 

  mySerial.println("DVD Tray State Monitor"); // Print to SoftwareSerial}
}

void loop() {
  // Read the state pins and combine them into a single integer
  currentState = 0;
  if (digitalRead(statePin0) == HIGH) { // Check for HIGH now
    currentState |= 0b001;
  }
  if (digitalRead(statePin1) == HIGH) {
    currentState |= 0b010;
  }
  if (digitalRead(statePin2) == HIGH) {
    currentState |= 0b100;
  }

  // Check if the state has changed
  if (currentState != previousState) {
    // Print the state in binary
    mySerial.print("State: ");
    for (int i = 2; i >= 0; i--) {  // Iterate through the bits (2, 1, 0)
      mySerial.print((currentState >> i) & 1); // Extract and print each bit
    }
    mySerial.print(" - ");
    mySerial.println(stateDescriptions[currentState]); // Access the description array

    previousState = currentState; // Update the previous state
  }

  // Check the eject pin
  currentEjectState = digitalRead(ejectPin);

  // Check for the falling edge (HIGH to LOW transition)
  if (currentEjectState == LOW && previousEjectState == HIGH) {
    mySerial.println("Eject button pressed."); // Print the message only once
  }

  // Update the previous control state
  previousEjectState = currentEjectState;

  delay(10); // Small delay for stability (optional)
}

