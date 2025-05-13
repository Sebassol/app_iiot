#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHT.h>

// WiFi
const char* ssid = "Sebastian";
const char* password = "sebasTsol";

// HiveMQ Cloud Config
const char* mqtt_server = "97e7a492de2b4a02a4c3692b1b640f50.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_user = "ADMIN";
const char* mqtt_pass = "Admin1234";

// DHT11 Config
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Cliente seguro (TLS)
WiFiClientSecure secureClient;
PubSubClient client(secureClient);

void setup_wifi() {
  delay(100);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi conectado");
  Serial.print("IP local: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Intentando conectar a HiveMQ... ");
    if (client.connect("ESP32Client", mqtt_user, mqtt_pass)) {
      Serial.println("✅ Conectado a HiveMQ Cloud");
    } else {
      Serial.print("❌ Falló, rc=");
      Serial.print(client.state());
      Serial.println(" - Reintentando en 5s");
      delay(5000);
    }
  }
}

void setup() {
  setup_wifi();

  // ⚠️ Esto permite conexiones sin verificar certificados (solo para pruebas)
  secureClient.setInsecure(); 

  client.setServer(mqtt_server, mqtt_port);
  dht.begin();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    Serial.println("❌ Error al leer el DHT11");
    return;
  }

  // Publicar en HiveMQ
  client.publish("Global_Tech_Measures/Colombia/Bucaramanga/Invernaderos/sensor/Temperatura", String(temp).c_str());
  client.publish("Global_Tech_Measures/Colombia/Bucaramanga/Invernaderos/sensor/Humedad", String(hum).c_str());

  Serial.printf("📡 Temp: %.1f°C | Hum: %.1f%%\n", temp, hum);
  delay(15000);
}
