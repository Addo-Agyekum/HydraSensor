#include <SPI.h>      // Required for SPI communication
#include <LoRa.h>     // LoRa library for Arduino
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>

// Define LoRa module pins
#define SS_PIN 15     // Slave Select pin
#define RST_PIN -1    // Reset pin (not used in this setup)
#define DIO0_PIN 5    // DIO0 pin (used for receive callback)

// Define LoRa frequency (868.1 MHz for Europe)
#define LORA_FREQUENCY 868.1E6  // Frequency in Hz

const char* ssid = "Scarlet";            // Replace with your Wi-Fi SSID
const char* password = "azizaziz";       // Replace with your Wi-Fi Password
const char* serverUrl = "https://shopify-webhook-sigma.vercel.app/test";  // Your HTTPS server URL

WiFiClientSecure client;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  while (!Serial);  // Wait for serial port to connect (only for native USB)

  Serial.println("LoRa Bidirectional Communication (868.1 MHz)");

  WiFi.begin(ssid, password);  // Connect to Wi-Fi
  Serial.println("Connecting to Wi-Fi...");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println("\nConnected to Wi-Fi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());  // Print the ESP8266 IP address
  
  client.setInsecure();  // Bypass SSL certificate validation for now

  // Initialize LoRa module
  LoRa.setPins(SS_PIN, RST_PIN, DIO0_PIN);

  if (!LoRa.begin(LORA_FREQUENCY)) {
    Serial.println("LoRa initialization failed. Check your connections.");
    while (1);  // Halt if initialization fails
  }

  // Set LoRa parameters
  LoRa.setSpreadingFactor(12);  // Spreading factor (6-12). Higher = more range, lower data rate
  LoRa.setSyncWord(0xF3);       // Ensure only nodes with the same sync word communicate

  Serial.println("LoRa initialization successful.");
}

void loop() {
  int packetSize = LoRa.parsePacket();  // Check if there's an incoming packet

  if (packetSize > 0) {
    Serial.println("Received LoRa packet");

    // Read the LoRa message
    String message = "";
    while (LoRa.available()) {
      message += (char)LoRa.read();  // Read incoming data
    }

    Serial.print("Received message: ");
    Serial.println(message);  // Print the received message

    // Send the message to the server
    if (WiFi.status() == WL_CONNECTED) {  // Check Wi-Fi connection
      HTTPClient http;
      http.begin(client, serverUrl);  // Initialize HTTP request
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");  // Set content type
      
      String postData = "receivedData=" + message;  // Prepare the data to send

      Serial.println("Sending data to server...");
      int httpResponseCode = http.POST(postData);  // Send POST request

      if (httpResponseCode > 0) {
        String response = http.getString();  // Get response from the server
        Serial.println("Server response: " + response);
      } else {
        Serial.println("Error on sending POST: " + String(httpResponseCode));  // Print error code
      }

      http.end();  // Close connection
    } else {
      Serial.println("WiFi disconnected.");
    }
  }

  delay(1000);  // Small delay to avoid spamming the server
}
