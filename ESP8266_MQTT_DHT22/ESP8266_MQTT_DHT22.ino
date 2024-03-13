#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

// Wifi & MQTT variable
const char* ssid = "Redmi";
const char* password = "qwerty098";
const char* mqtt_server = "192.168.61.114";

// Initialize Wifi
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

// Allocate the JSON document
JsonDocument doc;

// DHT & Relay PIN
#define DHT_SENSOR_PIN D3
#define DHT_SENSOR_TYPE DHT22
#define RELAY_PIN D2

// Initialize DHT22 & Relay
DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);

  dht_sensor.begin();
  pinMode(RELAY_PIN, OUTPUT);

  delay(100);
  digitalWrite(RELAY_PIN, LOW);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // wait 10 seconds between readings & sendings
  unsigned long now = millis();
  if (now - lastMsg > 10000) {
    lastMsg = now;

    // read humidity
    float humidity = dht_sensor.readHumidity();
    // read temperature in Celsius
    float temperature_C = dht_sensor.readTemperature();
    // read temperature in Fahrenheit
    float temperature_F = dht_sensor.readTemperature(true);

    // check whether the reading is successful or not
    if (isnan(temperature_C) || isnan(temperature_F) || isnan(humidity)) {
      Serial.println("Failed to read from DHT sensor!");
    } else {
      Serial.print("Humidity: ");
      Serial.print(humidity);
      Serial.print("%");

      Serial.print("  |  ");

      Serial.print("Temperature: ");
      Serial.print(temperature_C);
      Serial.print("°C  ~  ");
      Serial.print(temperature_F);
      Serial.println("°F");

      // Turn on Relay if the temperature reached 35 Celcius
      if (temperature_C >= 35) {
        digitalWrite(RELAY_PIN, HIGH);
      } else {
        digitalWrite(RELAY_PIN, LOW);
      }
    }

    // Add values in the document
    doc["humidity"] = humidity;
    doc["temperature_c"] = temperature_C;
    doc["temperature_f"] = temperature_F;

    // Send to MQTT Broker
    char buffer[96];
    serializeJson(doc, buffer);
    size_t n = serializeJson(doc, buffer);
    client.publish("dht22", buffer, n);
  }
}
