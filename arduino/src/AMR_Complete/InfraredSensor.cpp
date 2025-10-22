#include "InfraredSensor.h"

void InfraredSensor::init() {
    // Configurar pines analógicos como entrada (por defecto en Arduino)
    pinMode(IR_LEFT_SIDE, INPUT);
    pinMode(IR_RIGHT_SIDE, INPUT);
    pinMode(IR_FRONT_LEFT, INPUT);
    pinMode(IR_FRONT_CENTER, INPUT);
    pinMode(IR_FRONT_RIGHT, INPUT);
    
    // Inicializar valores
    leftSideValue = 0;
    rightSideValue = 0;
    frontLeftValue = 0;
    frontCenterValue = 0;
    frontRightValue = 0;
    
    leftSideDetect = false;
    rightSideDetect = false;
    frontLeftDetect = false;
    frontCenterDetect = false;
    frontRightDetect = false;
    
    Serial.println(F("=== IR Sensors Init ==="));
    Serial.println(F("L:A0 R:A1 FL:A2 FC:A3 FR:A4"));
    Serial.println(F("Usar 'I' para info sensores"));
}

void InfraredSensor::readAllSensors() {
    // Leer valores analógicos (0-1023)
    leftSideValue = analogRead(IR_LEFT_SIDE);
    rightSideValue = analogRead(IR_RIGHT_SIDE);
    frontLeftValue = analogRead(IR_FRONT_LEFT);
    frontCenterValue = analogRead(IR_FRONT_CENTER);
    frontRightValue = analogRead(IR_FRONT_RIGHT);
    
    // Determinar estados de detección
    // Nota: Los sensores IR típicamente dan valores ALTOS cuando detectan obstáculos
    leftSideDetect = (leftSideValue > IR_THRESHOLD_NEAR);
    rightSideDetect = (rightSideValue > IR_THRESHOLD_NEAR);
    frontLeftDetect = (frontLeftValue > IR_THRESHOLD_NEAR);
    frontCenterDetect = (frontCenterValue > IR_THRESHOLD_NEAR);
    frontRightDetect = (frontRightValue > IR_THRESHOLD_NEAR);
}

void InfraredSensor::printSensorValues() {
    Serial.println(F("=== Sensores IR ==="));
    
    // Laterales
    Serial.print(F("Lat Izq (A0): "));
    Serial.print(leftSideValue);
    Serial.println(leftSideDetect ? F(" [OBSTACULO]") : F(" [LIBRE]"));
    
    Serial.print(F("Lat Der (A1): "));
    Serial.print(rightSideValue);
    Serial.println(rightSideDetect ? F(" [OBSTACULO]") : F(" [LIBRE]"));
    
    // Frontales
    Serial.print(F("Fro Izq (A2): "));
    Serial.print(frontLeftValue);
    Serial.println(frontLeftDetect ? F(" [OBSTACULO]") : F(" [LIBRE]"));
    
    Serial.print(F("Fro Cen (A3): "));
    Serial.print(frontCenterValue);
    Serial.println(frontCenterDetect ? F(" [OBSTACULO]") : F(" [LIBRE]"));
    
    Serial.print(F("Fro Der (A4): "));
    Serial.print(frontRightValue);
    Serial.println(frontRightDetect ? F(" [OBSTACULO]") : F(" [LIBRE]"));
    
    Serial.println();
}

void InfraredSensor::calibrate() {
    Serial.println(F("=== Calibracion IR ==="));
    Serial.println(F("Valores actuales (0-1023):"));
    
    // Mostrar valores sin detección
    Serial.print(F("LS:"));
    Serial.print(leftSideValue);
    Serial.print(F(" RS:"));
    Serial.print(rightSideValue);
    Serial.print(F(" FL:"));
    Serial.print(frontLeftValue);
    Serial.print(F(" FC:"));
    Serial.print(frontCenterValue);
    Serial.print(F(" FR:"));
    Serial.println(frontRightValue);
    
    Serial.print(F("Umbral: "));
    Serial.println(IR_THRESHOLD_NEAR);
}
