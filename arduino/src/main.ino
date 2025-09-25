// Programa principal para Arduino: control de motores y sensado bajo nivel

#include "include/MotorDriver.h"
#include "include/UltrasonicSensor.h"
#include "include/Encoder.h"
#include "include/IMU.h"

// Instancias globales
MotorDriver motors;
UltrasonicSensor usensor;
Encoder encoders;
IMU imu;

void setup() {
    Serial.begin(115200);
    motors.init();
    usensor.init();
    encoders.init();
    imu.init();
}

void loop() {
    // Leer comandos de Raspberry
    if (Serial.available()) {
        // Leer y ejecutar comandos
    }
    // Leer sensores y enviar datos si es necesario
}