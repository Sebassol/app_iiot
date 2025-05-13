#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

//conexion a internet en donde se va a instalar el sensor debe estar a una red de internet de 2.4hz porque es la unica frecuencia aceptable para ESP-32
const char* ssid = "Sebastian";
const char* password = "sebasTsol";


const char* mqtt_server = "52.224.5.206"; // IP de tu PC con Mosquitto esta seria la de la maquina virtual 


#define DHTPIN 4       // Pin donde conectaste el DHT11
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(100);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado ✅");
  Serial.print("Conectado al WiFi con IP: ");
  Serial.println(WiFi.localIP());

}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Intentando conexión MQTT... ");
    if (client.connect("ESP32Client")) {
      Serial.println("Conectado ✅");
    } else {
      Serial.print("Fallo, rc=");
      Serial.print(client.state());
      Serial.print(" - ");
      switch (client.state()) {
        case -4: Serial.println("Timeout"); break;
        case -2: Serial.println("Conexión rechazada"); break;
        case -1: Serial.println("Cliente desconectado"); break;
        default: Serial.println("Error desconocido");
      }
      delay(5000);
    }
  }
}

void setup() {
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  dht.begin();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // 🌡️ Leer sensores
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    Serial.println("❌ Error al leer el DHT11");
    return;
  }

  // 📤 Publicar datos reales
  client.publish("Global_Tech_Measures/Colombia/Bucaramanga/Invernaderos/sensor/Temperatura", String(temp).c_str());
  client.publish("Global_Tech_Measures/Colombia/Bucaramanga/Invernaderos/sensor/Humedad", String(hum).c_str());

  Serial.printf("Temp: %.1f°C | Hum: %.1f%%\n", temp, hum);
  delay(5000); // Esperar 5s
}