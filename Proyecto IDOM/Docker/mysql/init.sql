CREATE DATABASE IF NOT EXISTS invernadero;
USE invernadero;

CREATE TABLE IF NOT EXISTS mediciones (
  id INT AUTO_INCREMENT PRIMARY KEY,
  temperatura DECIMAL(5,2) NOT NULL,
  humedad DECIMAL(5,2) NOT NULL,
  led_humedad TINYINT(1) DEFAULT 0,
  led_temperatura TINYINT(1) DEFAULT 0,
  buzzer_humedad TINYINT(1) DEFAULT 0,
  buzzer_temperatura TINYINT(1) DEFAULT 0,
  ubicacion VARCHAR(100) DEFAULT 'invernadero_1',
  fecha DATE NOT NULL,                        
  hora TIME NOT NULL
);
