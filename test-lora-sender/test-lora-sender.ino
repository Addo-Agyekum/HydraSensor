#include <SPI.h>
#include <LoRa.h>

// Pin Definitions
#define SS_PIN 10             // Slave Select pin for LoRa
#define RST_PIN 9             // Reset pin for LoRa
#define DIO0_PIN 7            // DIO0 pin for LoRa

// LoRa Configurations
#define LORA_FREQUENCY 868.1E6  // Frequency for LoRa communication

// Function Prototypes
void sendSensorData(float temperature, float turbidity);

void setup() {
  // Initialize Serial
  Serial.begin(115200);

  // Initialize LoRa communication
  LoRa.setPins(SS_PIN, RST_PIN, DIO0_PIN);

  if (!LoRa.begin(LORA_FREQUENCY)) {
    Serial.println("LoRa initialization failed!");
    while (1);  // Stop execution if LoRa fails to initialize
  }

  // Set LoRa parameters
  LoRa.setSpreadingFactor(12);
  LoRa.setSyncWord(0xF3);

  Serial.println("LoRa Transmitter ready");
}

void loop() {
  // Placeholder sensor data, replace with actual sensor readings
  float temperature = 25.5;  // Replace with temperature sensor reading
  float turbidity = 3.8;     // Replace with turbidity sensor reading

  // Send the sensor data via LoRa
  sendSensorData(temperature, turbidity);

  // Wait before sending next data (adjust as necessary)
  delay(5000);
}

// Function to send sensor data via LoRa
void sendSensorData(float temperature, float turbidity) {
  // Format sensor data as a string: temperature, turbidity
  String sensorData = String(temperature, 2) + "," + String(turbidity, 2);

  // Send sensor data via LoRa
  LoRa.beginPacket();
  LoRa.print(sensorData);  // Send the formatted sensor data string
  LoRa.endPacket();

  Serial.println("Sensor Data sent: " + sensorData);
}
