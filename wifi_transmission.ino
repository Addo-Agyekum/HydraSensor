#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>  

const char* ssid = "VodafoneMobileWiFi-ACDF90";       
const char* password = "0933279580";           

const char* serverUrl = "https://shopify-webhook-sigma.vercel.app/test"; 

WiFiClient client;

void setup() {
  Serial.begin(9600);
  delay(100);

  // Connect to WiFi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Wait for data from MCU 1 (sensor data)
  if (Serial.available()) {
    String sensorData = Serial.readStringUntil('\n');  // Read the data from the serial input (NTU and Temp)

    // Initialize an HTTP client
    if (WiFi.status() == WL_CONNECTED) {  // Check if WiFi is still connected
      HTTPClient http;

      http.begin(client, serverUrl);              // Specify the URL for the HTTP request
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");  // Set content type to URL-encoded

      // Send the sensor data to the server
      String postData = "sensorData=" + sensorData;  // Data format: key=value

      int httpResponseCode = http.POST(postData);    // Send HTTP POST request

      if (httpResponseCode > 0) {
        String response = http.getString();          // Get the response from the server
        Serial.println("Server response: " + response);
      } else {
        Serial.println("Error on sending POST: " + String(httpResponseCode));
      }

      http.end();  // Close the connection
    } else {
      Serial.println("WiFi Disconnected.");
    }

    delay(3000);  // Wait for 3 seconds before the next transmission
  }
}