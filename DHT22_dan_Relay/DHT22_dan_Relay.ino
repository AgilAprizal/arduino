#include <DHT.h>
#define DHT_SENSOR_PIN  D3 // The ESP8266 pin D7 connected to DHT22 sensor
#define DHT_SENSOR_TYPE DHT22
#define RELAY_PIN D2

DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

void setup() {
  Serial.begin(115200);
  dht_sensor.begin(); // initialize the DHT sensor
  pinMode(RELAY_PIN, OUTPUT);

  delay(100);
  digitalWrite(RELAY_PIN, LOW);
}

void loop() {
  // read humidity
  float humi  = dht_sensor.readHumidity();
  // read temperature in Celsius
  float temperature_C = dht_sensor.readTemperature();
  // read temperature in Fahrenheit
  float temperature_F = dht_sensor.readTemperature(true);

  // check whether the reading is successful or not
  if ( isnan(temperature_C) || isnan(temperature_F) || isnan(humi)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    Serial.print("Humidity: ");
    Serial.print(humi);
    Serial.print("%");

    Serial.print("  |  ");

    Serial.print("Temperature: ");
    Serial.print(temperature_C);
    Serial.print("°C  ~  ");
    Serial.print(temperature_F);
    Serial.println("°F");

    if (temperature_C >= 35) {
      digitalWrite(RELAY_PIN, HIGH);
    } else {
      digitalWrite(RELAY_PIN, LOW);
    }
  }

  // digitalWrite(RELAY_PIN, LOW);

  // wait a 1 seconds between readings
  // delay(1000);

  // digitalWrite(RELAY_PIN, HIGH);

  // wait a 2 seconds between readings
  delay(2000);
}