#include <Arduino.h>
#include <Adafruit_INA228.h>
#include "defines.h"

Adafruit_INA228 ina228 = Adafruit_INA228();

// 8S LiFePO4 Voltage to SOC Lookup Table (approximate values, at rest/no load)
// Format: {voltage, SOC_percent}
// Values interpolated between known data points
const float voltageSOC[][2] = {
  {29.20, 100},   // 100% - Full charge (3.65V/cell)
  {27.20, 90},    // 90%  - Resting full (3.40V/cell)
  {26.80, 80},    // 80%  (3.35V/cell)
  {26.56, 70},    // 70%  (3.32V/cell)
  {26.32, 60},    // 60%  (3.29V/cell)
  {26.16, 50},    // 50%  (3.27V/cell)
  {26.00, 40},    // 40%  (3.25V/cell)
  {25.76, 30},    // 30%  (3.22V/cell)
  {25.60, 20},    // 20%  (3.20V/cell)
  {24.00, 10},    // 10%  (3.00V/cell)
  {20.00, 0}      // 0%   - Cut-off (2.50V/cell)
};

#define TABLE_SIZE (sizeof(voltageSOC) / sizeof(voltageSOC[0]))

// Function: map voltage to SOC using piecewise linear interpolation
int voltageToSOC(float voltage) {
  // Clamp voltage to table range
  if (voltage >= voltageSOC[0][0]) return 100;
  if (voltage <= voltageSOC[TABLE_SIZE - 1][0]) return 0;

  // Find the two surrounding points and interpolate
  for (int i = 0; i < TABLE_SIZE - 1; i++) {
    if (voltage <= voltageSOC[i][0] && voltage >= voltageSOC[i + 1][0]) {
      float vHigh = voltageSOC[i][0];
      float vLow  = voltageSOC[i + 1][0];
      float socHigh = voltageSOC[i][1];
      float socLow  = voltageSOC[i + 1][1];
      
      // Linear interpolation between the two points
      float fraction = (voltage - vLow) / (vHigh - vLow);
      return (int)(socLow + fraction * (socHigh - socLow));
    }
  }
  return 0;
}

void ina228Loop() {
  static uint32_t lastPrint = 0;
  if (millis() - lastPrint < 5000) return;  // update every 5 seconds
  lastPrint = millis();

  // Read raw voltage from INA228 (no correction)
  float voltage = ina228.getBusVoltage_V();

  // Get SOC using the lookup table
  int batteryLevel = voltageToSOC(voltage);

  // Round down to nearest multiple of 5 (0, 5, 10, 15, ... 100)
  int batteryLevelRounded = (batteryLevel / 5) * 5;

  // Print for debugging
  Serial.print("Bus Voltage: ");
  Serial.print(voltage, 3);
  Serial.print(" V  |  SOC: ");
  Serial.print(batteryLevel);
  Serial.print("%  |  Blynk: ");
  Serial.println(batteryLevelRounded);

  // Send to Blynk
  Blynk.virtualWrite(BATTERY_LEVEL_VIRTUAL_PIN, batteryLevelRounded);
}

// =============================================
// setupIna228() - Called ONCE from main setup()
// =============================================
void setupIna228() {
  // Try to initialize the INA228 sensor
  if (!ina228.begin()) {
    // Print error but DO NOT LOCK the system.
    // The program will continue running.
    Serial.println("WARNING: INA228 not found! Check wiring.");
    // Optionally set a global flag here if you want to skip reading later.
  } else {
    Serial.println("INA228 initialized successfully.");
  }
  // Function always returns, never blocks.
}