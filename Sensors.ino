// Turbidity Sensor Pins and Constants
int turbidityPin = A0;
float volt;
float ntu;
float a = -1120.4;
float b = 5742.3;
float c = -4353.8;

// Temperature Sensor Pins and Libraries
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  // Start serial communication for debugging and data transmission
  Serial.begin(9600);
  
  // Start temperature sensor
  sensors.begin();
}

void loop() {
  // ---------- Turbidity Sensor Reading ----------
  volt = 0;
  // Take 800 analog readings and average them
  for(int i = 0; i < 800; i++) {
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
  Serial.println(" °C");

  // Send data over Serial to the WiFi module (Board 2)
  Serial.print(ntu);  // Send NTU value
  Serial.print(",");
  Serial.println(tempC);  // Send temperature value (Celsius)

  delay(2000);  // Wait for 3 seconds before the next reading
}

// Function to round the voltage to a specific number of decimal places
float round_to_dp(float in_value, int decimal_place) {
  float multiplier = pow(10.0f, decimal_place);
  in_value = round(in_value * multiplier) / multiplier;
  return in_value;
}
