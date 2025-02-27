/*  Code by mathmaticalmaker
    This is to read the DVD tray state lines from an Original Xbox and
    write the states to a serial monitor when the status changes.  
    The available documentation for these states is incomplete and incorrect
    from what I've been able to find, to this is an attempt to document them
    correctly.

    This has been tested using an Arduino Leonardo clone and an FTDI adapter
    connected to a PC running a terminal program.  Connections to the DVD header
    on the Xbox were made by splicing into the wires of an original yellow DVD
    cable, specifically pins 2-7.  
    
    Connections, DVD Header to Leonardo:
    2 -- +5V    -- Vin
    3 -- GND    -- GND
    4 -- Eject  -- 4
    5 -- State0 -- 5
    6 -- State1 -- 6
    7 -- State2 -- 7

    Connections, Leonardo to FTDI
    A0  -- TX
    A1  -- RX
    GND -- GND

    Note the Leonardo is powered by the +5V from the Xbox itself and that 
    common ground is used for the Leonardo and the FTDI adapter.  It is neither
    necessary nor advisable to connect to VCC on the FTDI adapter.
*/ 
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
  "Detecting",       //000
  "Tray Open",       //001
  "Unloading",       //010
  "Opening",         //011
  "No Media",        //100
  "Closing",         //101  
  "Media Detected",  //110
  "Initializing"     //111
};

void setup() {
  // Initialize Software Serial communication
  mySerial.begin(9600); 

  // Set the input pins as inputs - NO PULLUPS
  pinMode(statePin0, INPUT);
  pinMode(statePin1, INPUT);
  pinMode(statePin2, INPUT);
  pinMode(ejectPin, INPUT); 

  mySerial.println("DVD Tray State Monitor"); // Print to SoftwareSerial
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

  // delay(10); // Small delay for stability (optional)
}

