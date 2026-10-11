/*
  Combined Sketch: SCD30 Sensor + Arduino IoT Cloud

  Cloud Variables (configure these in your Arduino IoT Cloud Thing):
  - float co2Value  (READ ONLY from dashboard)
*/

#include <Adafruit_SCD30.h>
#include "thingProperties.h"

Adafruit_SCD30 scd30;

// --- Optional: Print system uptime in H:M:S ---
void printUptime() {
  unsigned long ms = millis();
  unsigned long seconds = ms / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;

  seconds = seconds % 60;
  minutes = minutes % 60;

  Serial.print("Uptime: ");
  Serial.print(hours);
  Serial.print(":");
  if (minutes < 10) Serial.print("0");
  Serial.print(minutes);
  Serial.print(":");
  if (seconds < 10) Serial.print("0");
  Serial.println(seconds);
}

void setup() {
  Serial.begin(115200);
  delay(1500); // wait for Serial Monitor

  Serial.println("SCD30 + Arduino IoT Cloud starting...");

  // --- Initialize IoT Cloud properties ---
  initProperties();

  // --- Connect to Arduino IoT Cloud ---
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  setDebugMessageLevel(4);
  ArduinoCloud.printDebugInfo();

  // --- Initialize SCD30 Sensor ---
  if (!scd30.begin()) {
    Serial.println("Failed to find SCD30 chip");
    while (1) { delay(10); }
  }
  Serial.println("SCD30 Found!");
  Serial.print("Measurement Interval: ");
  Serial.print(scd30.getMeasurementInterval());
  Serial.println(" seconds");
}

void loop() {
  // Keep IoT Cloud connection alive
  ArduinoCloud.update();

  // --- Read SCD30 (CO2 / Temp / Humidity) ---
  if (scd30.dataReady()) {
    Serial.println("SCD30 data available!");

    if (!scd30.read()) {
      Serial.println("Error reading SCD30 sensor data");
      delay(1000);
      return;
    }

    // Print uptime
    printUptime();

    // Update CO2 cloud variable
    co2Value = scd30.CO2;

    // Serial debug prints for environmental data
    Serial.print("Temperature: ");
    Serial.print(scd30.temperature);
    Serial.println(" °C");

    Serial.print("Humidity   : ");
    Serial.print(scd30.relative_humidity);
    Serial.println(" %");

    Serial.print("CO₂        : ");
    Serial.print(co2Value, 3);
    Serial.println(" ppm");
  } else {
    Serial.println("Waiting for SCD30 data...");
  }

  Serial.println("-----------------------------");

  // SCD30 default measurement interval is ~2–10s, you used 10s originally
  delay(10000); // 10 seconds between readings
}

/*
  Callback when Co2Value changes from IoT Cloud (if READ_WRITE).
*/
void onCo2ValueChange() {
  // Add your code here to act upon Co2Value change if needed
}
