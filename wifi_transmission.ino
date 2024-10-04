#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "Scarlet";            // Replace with your Wi-Fi SSID
const char* password = "azizaziz";       // Replace with your Wi-Fi Password
const char* serverUrl = "https://shopify-webhook-sigma.vercel.app/test";  // Your HTTPS server URL

WiFiClientSecure client;

void setup() {
  Serial.begin(9600);  // Initialize serial communication at 9600 baud
  WiFi.begin(ssid, password);  // Connect to Wi-Fi
  
  Serial.println("Connecting to Wi-Fi...");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println("\nConnected to Wi-Fi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());  // Print the ESP8266 IP address
  
  // Bypass SSL certificate validation (NOT recommended for production)
  client.setInsecure();  // Bypass SSL cert validation for now
}

void loop() {
  // Wait for input from the serial monitor
  if (Serial.available()) {
    Serial.println("Serial available");

    String customData = Serial.readStringUntil('\n');  // Read the custom data input from Serial Monitor
    Serial.println("Sending custom data: " + customData);  // Debugging line to verify the input

    // Initialize an HTTP client
    if (WiFi.status() == WL_CONNECTED) {  // Check if Wi-Fi is still connected
      HTTPClient http;

      // Try connecting to the server
      Serial.println("Attempting to connect to the server...");
      if (client.connect("shopify-webhook-sigma.vercel.app", 443)) {
        Serial.println("Connected to server.");

        http.begin(client, serverUrl);  // Specify the HTTPS server URL
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");  // Set content type to URL-encoded

        // Send the custom data to the server
        String postData = "customData=" + customData;  // Data format: key=value (e.g., customData=my message)

        int httpResponseCode = http.POST(postData);  // Send HTTP POST request
        Serial.println("HTTP Response code: " + String(httpResponseCode));  // Print response code

        // Check server response
        if (httpResponseCode > 0) {
          String response = http.getString();  // Get the response from the server
          Serial.println("Server response: " + response);
        } else {
          Serial.println("Error on sending POST: " + String(httpResponseCode));  // If POST failed
        }

        http.end();  // Close the connection
      } else {
        Serial.println("Failed to connect to server.");
      }

    } else {
      Serial.println("WiFi Disconnected.");
    }

    delay(3000);  // Wait for 3 seconds before the next input
  }
}
