#include "Odometry.h"
#include <math.h>

Odometry::Odometry(Encoder* enc) {
    encoder = enc;
    x = 0.0;
    y = 0.0;
    theta = 0.0;
    lastLeftPulses = 0;
    lastRightPulses = 0;
}

void Odometry::init(float startX, float startY, float startTheta) {
    x = startX;
    y = startY;
    theta = degreesToRadians(startTheta);
    
    // Inicializar lecturas previas
    lastLeftPulses = encoder->readLeft();
    lastRightPulses = encoder->readRight();
    
    Serial.println("=== Odometría Inicializada ===");
    Serial.print("Posición inicial: (");
    Serial.print(x, 2);
    Serial.print(", ");
    Serial.print(y, 2);
    Serial.print(") Ángulo: ");
    Serial.print(startTheta, 1);
    Serial.println("°");
}

void Odometry::update() {
    // Leer pulsos actuales
    long currentLeftPulses = encoder->readLeft();
    long currentRightPulses = encoder->readRight();
    
    // Calcular diferencias desde la última actualización
    long deltaLeftPulses = currentLeftPulses - lastLeftPulses;
    long deltaRightPulses = currentRightPulses - lastRightPulses;
    
    // Convertir a distancia
    float deltaLeftCm = encoder->pulsesToCentimeters(deltaLeftPulses);
    float deltaRightCm = encoder->pulsesToCentimeters(deltaRightPulses);
    
    // Calcular movimiento del robot
    float deltaDistance = (deltaLeftCm + deltaRightCm) / 2.0;
    float deltaTheta = (deltaRightCm - deltaLeftCm) / WHEEL_BASE_CM;
    
    // Actualizar orientación
    theta += deltaTheta;
    
    // Normalizar ángulo entre -π y π
    while (theta > PI) theta -= 2 * PI;
    while (theta < -PI) theta += 2 * PI;
    
    // Actualizar posición
    x += deltaDistance * cos(theta);
    y += deltaDistance * sin(theta);
    
    // Guardar lecturas para próxima iteración
    lastLeftPulses = currentLeftPulses;
    lastRightPulses = currentRightPulses;
}

float Odometry::radiansToDegrees(float radians) {
    return radians * 180.0 / PI;
}

float Odometry::degreesToRadians(float degrees) {
    return degrees * PI / 180.0;
}

void Odometry::setPosition(float newX, float newY, float newTheta) {
    x = newX;
    y = newY;
    theta = degreesToRadians(newTheta);
}

void Odometry::resetPosition() {
    x = 0.0;
    y = 0.0;
    theta = 0.0;
    encoder->resetBoth();
    lastLeftPulses = 0;
    lastRightPulses = 0;
}

void Odometry::printPosition() {
    Serial.print("Posición: (");
    Serial.print(x, 2);
    Serial.print(", ");
    Serial.print(y, 2);
    Serial.print(") cm, Ángulo: ");
    Serial.print(radiansToDegrees(theta), 1);
    Serial.print("°, Distancia del origen: ");
    Serial.print(getDistanceFromOrigin(), 2);
    Serial.println(" cm");
}

float Odometry::getDistanceFromOrigin() {
    return sqrt(x*x + y*y);
}