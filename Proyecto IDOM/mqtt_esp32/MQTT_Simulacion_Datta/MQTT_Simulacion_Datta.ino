#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <time.h>


const char* ssid = "Sebastian";
const char* password = "sebasTsol";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -5 * 3600; // Colombia UTC -5
const int   daylightOffset_sec = 0;

const char* mqtt_server = "192.168.215.184";

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
  Serial.println("\n✅ WiFi conectado");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.print("Esperando hora NTP");
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\n🕒 Hora sincronizada con NTP");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Intentando conexión MQTT... ");
    if (client.connect("ESP32Client", "Usuario", "Admin1234")) {
      Serial.println("Conectado ✅");
    } else {
      Serial.print("Fallo, rc=");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

float getRandom(float minVal, float maxVal) {
  return minVal + ((float)random(0, 1000) / 1000.0) * (maxVal - minVal);
}

int randomLowFrequency() {
  return random(0, 10) < 2 ? 1 : 0; // Solo 20% probabilidad de ser 1
}

void setup() {
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  randomSeed(analogRead(0));
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  float tempB = getRandom(11.2, 13.0);
  float humB  = getRandom(66.0, 84.0);
  float tempC = getRandom(11.6, 16.0);
  float humC  = getRandom(80.0, 89.0);

  int ledH = randomLowFrequency();
  int ledT = randomLowFrequency();
  int buzH = randomLowFrequency();
  int buzT = randomLowFrequency();

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("❌ No se pudo obtener la hora");
    return;
  }

  char fechaHora[25];
  strftime(fechaHora, sizeof(fechaHora), "%Y-%m-%d %H:%M:%S", &timeinfo);
  String fechaHoraStr = String(fechaHora);


  // Payload sensor A (Bogotá)
  String payloadA = "{";
  payloadA += "\"temperatura\":" + String(tempB, 2) + ",";
  payloadA += "\"humedad\":" + String(humB, 2) + ",";
  payloadA += "\"led_humedad\":" + String(ledH) + ",";
  payloadA += "\"led_temperatura\":" + String(ledT) + ",";
  payloadA += "\"buzzer_humedad\":" + String(buzH) + ",";
  payloadA += "\"buzzer_temperatura\":" + String(buzT) + ",";
  payloadA += "\"ubicacion\":\"Bogota\",";
  payloadA += "\"fecha_hora\":\"" + fechaHoraStr + "\"";
  payloadA += "}";

  // Payload sensor B (Medellín)
  String payloadB = "{";
  payloadB += "\"temperatura\":" + String(tempC, 2) + ",";
  payloadB += "\"humedad\":" + String(humC, 2) + ",";
  payloadB += "\"led_humedad\":" + String(ledH) + ",";
  payloadB += "\"led_temperatura\":" + String(ledT) + ",";
  payloadB += "\"buzzer_humedad\":" + String(buzH) + ",";
  payloadB += "\"buzzer_temperatura\":" + String(buzT) + ",";
  payloadB += "\"ubicacion\":\"medellin\",";
  payloadB += "\"fecha_hora\":\"" + fechaHoraStr + "\"";
  payloadB += "}";

  client.publish("Global_Tech_Measures/Colombia/Bogota/Invernaderos/sensorB", payloadA.c_str());
  client.publish("Global_Tech_Measures/Colombia/Medellin/Invernaderos/sensorC", payloadB.c_str());

  Serial.println("📤 Datos enviados");
  delay(5000);
}
