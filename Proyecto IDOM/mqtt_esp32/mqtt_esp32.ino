#include <BluetoothSerial.h>
BluetoothSerial SerialBT;

enum Modo { AUTOMATICO, MANUAL };
enum Rango { MICRO, MILI };

Modo modo = AUTOMATICO;
Rango rango_manual = MICRO;

unsigned long tiempo_ultimo_cambio = 0;
const unsigned long intervalo_cambio = 10000; // 10 segundos

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32Sensor"); // Nombre Bluetooth
  Serial.println("Bluetooth iniciado");

  tiempo_ultimo_cambio = millis();
}

void loop() {
  // Revisar si llegó comando por Bluetooth para cambiar modo o rango
  if (SerialBT.available()) {
    char c = SerialBT.read();
    if (c == 'A') {       // Automático
      modo = AUTOMATICO;
      Serial.println("Modo AUTOMATICO");
    } else if (c == 'M') { // Manual
      modo = MANUAL;
      Serial.println("Modo MANUAL");
    } else if (c == '0') { // Rango microamperios
      rango_manual = MICRO;
      Serial.println("Rango MANUAL: MICRO");
    } else if (c == '1') { // Rango miliamperios
      rango_manual = MILI;
      Serial.println("Rango MANUAL: MILI");
    }
  }

  float corriente = 0.0;

  if (modo == AUTOMATICO) {
    // Cambiar rango cada 10 segundos
    if (millis() - tiempo_ultimo_cambio >= intervalo_cambio) {
      if (rango_manual == MICRO) rango_manual = MILI;
      else rango_manual = MICRO;
      tiempo_ultimo_cambio = millis();
    }

    // Generar corriente aleatoria según rango actual
    if (rango_manual == MICRO) {
      // Microamperios entre 5 y 10
      corriente = random(5000, 10001) / 1000.0; // µA
    } else {
      // Miliamperios entre 0 y 10
      corriente = random(0, 10001) / 1000.0; // mA
    }
  } else {
    // Modo manual: corriente fija según rango manual
    if (rango_manual == MICRO) {
      // Entre 5 y 10 microamperios
      corriente = random(5000, 10001) / 1000.0; // µA
    } else {
      // Entre 0 y 10 miliamperios
      corriente = random(0, 10001) / 1000.0; // mA
    }
  }

  // Enviar dato formateado como texto Bluetooth (por ejemplo: "MICRO:7.2" o "MILI:4.3")
  String textoEnviar = "";
  if (rango_manual == MICRO) {
    textoEnviar = String(corriente, 3)+" [uA]";  // 3 decimales
  } else {
    textoEnviar = String(corriente, 3)+" [mA]";
  }
  SerialBT.println(textoEnviar);

  delay(1000); // Enviar dato cada 1 segundo
}

