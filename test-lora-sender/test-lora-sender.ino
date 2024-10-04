#include <SPI.h>
#include <LoRa.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Pin Definitions
#define SS_PIN 10             // Slave Select pin for LoRa
#define RST_PIN 9             // Reset pin for LoRa
#define DIO0_PIN 7            // DIO0 pin for LoRa

// LoRa Configurations
#define LORA_FREQUENCY 868.1E6  // Frequency for LoRa communication

// Function Prototypes
void sendSensorData(float temperature, float turbidity);

// Turbidity Sensor Pins and Constants
int turbidityPin = A0;
float volt;
float ntu;
float a = -1120.4;
float b = 5742.3;
float c = -4353.8;

// Temperature Sensor Pins and Libraries
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);


void setup() {
  // Start serial communication for debugging and data transmission
  Serial.begin(9600);

  // Start temperature sensor
  sensors.begin();

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
  // ---------- Turbidity Sensor Reading ----------
  volt = 0;
  // Take 800 analog readings and average them
  for (int i = 0; i < 800; i++) {
    volt += ((float)analogRead(turbidityPin) / 1023) * 5;
  }
  volt = volt / 800;
  volt = round_to_dp(volt, 2);  // Round the voltage value to 2 decimal places

  // Apply the polynomial equation to calculate NTU
  ntu = a * sq(volt) + b * volt + c;
  if (ntu < 0) ntu = 0;  // Ensure NTU doesn't go negative

  // ---------- Temperature Sensor Reading ----------
  sensors.requestTemperatures();  // Get temperatures from all devices
  float tempC = sensors.getTempCByIndex(0);  // Celsius

  // Print data to Serial for debugging
  Serial.print("Turbidity Voltage: ");
  Serial.print(volt);
  Serial.print(" V, NTU: ");
  Serial.println(ntu);
  
  Serial.print("Celsius: ");
  Serial.print(tempC);
  Serial.print(" C");  // Properly print degrees Celsius
  Serial.println();

  // Send the sensor data via LoRa
  sendSensorData(tempC, ntu);

  delay(2000);  // Wait for 2 seconds before the next reading
}

// Function to round the voltage to a specific number of decimal places
float round_to_dp(float in_value, int decimal_place) {
  float multiplier = pow(10.0f, decimal_place);
  in_value = round(in_value * multiplier) / multiplier;
  return in_value;
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
