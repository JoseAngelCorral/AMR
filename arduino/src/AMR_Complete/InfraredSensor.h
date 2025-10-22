#pragma once

#ifndef INFRARED_SENSOR_H
#define INFRARED_SENSOR_H

#include <Arduino.h>

// Pines para los sensores infrarrojos
#define IR_LEFT_SIDE      A0    // Sensor lateral izquierdo
#define IR_RIGHT_SIDE     A1    // Sensor lateral derecho
#define IR_FRONT_LEFT     A2    // Sensor frontal izquierdo
#define IR_FRONT_CENTER   A3    // Sensor frontal central
#define IR_FRONT_RIGHT    A4    // Sensor frontal derecho

// Umbrales de detección (ajustar según calibración)
#define IR_THRESHOLD_NEAR 400   // Umbral para obstáculo cercano
#define IR_THRESHOLD_FAR  200   // Umbral para obstáculo lejano

class InfraredSensor {
private:
    // Valores analógicos de cada sensor
    int leftSideValue;
    int rightSideValue;
    int frontLeftValue;
    int frontCenterValue;
    int frontRightValue;
    
    // Estados de detección
    bool leftSideDetect;
    bool rightSideDetect;
    bool frontLeftDetect;
    bool frontCenterDetect;
    bool frontRightDetect;
    
public:
    void init();
    
    // Lectura de sensores
    void readAllSensors();
    
    // Obtener valores analógicos
    int getLeftSide() { return leftSideValue; }
    int getRightSide() { return rightSideValue; }
    int getFrontLeft() { return frontLeftValue; }
    int getFrontCenter() { return frontCenterValue; }
    int getFrontRight() { return frontRightValue; }
    
    // Detección de obstáculos (retorna true si detecta)
    bool isLeftSideBlocked() { return leftSideDetect; }
    bool isRightSideBlocked() { return rightSideDetect; }
    bool isFrontLeftBlocked() { return frontLeftDetect; }
    bool isFrontCenterBlocked() { return frontCenterDetect; }
    bool isFrontRightBlocked() { return frontRightDetect; }
    
    // Detección combinada
    bool isAnyFrontBlocked() { return frontLeftDetect || frontCenterDetect || frontRightDetect; }
    bool isAnySideBlocked() { return leftSideDetect || rightSideDetect; }
    bool isAnyBlocked() { return isAnyFrontBlocked() || isAnySideBlocked(); }
    
    // Diagnóstico
    void printSensorValues();
    void calibrate();  // Mostrar valores para calibración
};

#endif // INFRARED_SENSOR_H
