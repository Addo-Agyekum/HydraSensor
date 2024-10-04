// Include necessary libraries
#include <SPI.h>      // Required for SPI communication
#include <LoRa.h>     // LoRa library for Arduino

// Define LoRa module pins
#define SS_PIN 15     // Slave Select pin
#define RST_PIN -1    // Reset pin (not used in this setup)
#define DIO0_PIN 5    // DIO0 pin (used for receive callback)

// Define LoRa frequency (868.1 MHz for Europe)
#define LORA_FREQUENCY 868.1E6  // Frequency in Hz

// Variables for handling user input
String inputMessage = "";   // Stores the message input by the user
bool messageReady = false;  // Flag to indicate when a complete message is ready to send

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  while (!Serial);  // Wait for serial port to connect. Needed for native USB port only

  Serial.println("LoRa Bidirectional Communication (868.1 MHz)");

  // Configure LoRa transceiver module
  LoRa.setPins(SS_PIN, RST_PIN, DIO0_PIN);

  // Initialize LoRa
  if (!LoRa.begin(LORA_FREQUENCY)) {
    Serial.println("LoRa initialization failed. Check your connections.");
    while (1);  // If failed, do nothing
  }

  // Set LoRa parameters
  LoRa.setSpreadingFactor(12);  // Set spreading factor (6-12). Higher SF increases range but decreases data rate
  LoRa.setSyncWord(0xF3);       // Set sync word to ensure communication only between nodes with the same sync word

  Serial.println("LoRa initialization successful.");
  Serial.println("Type a message and press enter to send via LoRa.");
}

void loop() {
  // Check for user input from Serial monitor
  while (Serial.available()) {
    char c = Serial.read();  // Read a character from the serial buffer
    if (c == '\n') {  // If the character is a newline (Enter key pressed)
      messageReady = true;  // Set flag to indicate a complete message is ready
    } else {
      inputMessage += c;  // Add the character to the input message
    }
  }

  // If a complete message is ready, send it
  if (messageReady) {
    sendMessage(inputMessage);  // Call function to send the message
    inputMessage = "";  // Clear the input message buffer
    messageReady = false;  // Reset the message ready flag
  }

  // Check for any incoming LoRa data
  int packetSize = LoRa.parsePacket();
  if (packetSize) {  // If a packet is received
    receiveMessage();  // Call function to handle the received message
  }
}

// Function to send a LoRa message
void sendMessage(String message) {
  Serial.print("Sending message: ");
  Serial.println(message);

  LoRa.beginPacket();  // Start the LoRa packet
  LoRa.print(message);  // Add the message to the packet
  LoRa.endPacket();  // Finish the packet and send it
}

// Function to receive and process a LoRa message
void receiveMessage() {
  String message = "";  // Variable to store the received message

  // Read the packet
  while (LoRa.available()) {
    message += (char)LoRa.read();  // Read each byte and add it to the message string
  }

  // Print the received message
  Serial.print("Received message: ");
  Serial.println(message);

  // Print the RSSI (Received Signal Strength Indicator)
  Serial.print("RSSI: ");
  Serial.println(LoRa.packetRssi());  // Get and print the RSSI of the received packet
}