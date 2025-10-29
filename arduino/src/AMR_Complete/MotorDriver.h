#pragma once

#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include <Arduino.h>

// Pines para control de motores BTS7960 - Configuración real
// Motor Izquierdo (BTS7960 #1)
#define MOTOR_LEFT_RPWM 10   // PWM Adelante motor izquierdo
#define MOTOR_LEFT_LPWM 11   // PWM Atrás motor izquierdo  
// REN y LEN: Alimentación externa (siempre HIGH)

// Motor Derecho (BTS7960 #2)  
#define MOTOR_RIGHT_RPWM 5   // PWM Adelante motor derecho
#define MOTOR_RIGHT_LPWM 6   // PWM Atrás motor derecho
// REN y LEN: Alimentación externa (siempre HIGH)

#// Velocidades para BTS7960 (más potente que L298N)
#// Valores ajustados: ver README/AMR_Complete para porcentajes
#define DEFAULT_SPEED 170    // Velocidad por defecto (0-255)
#define TURN_SPEED 140       // Velocidad para giros
#define MAX_SPEED 255        // Velocidad máxima
#define MIN_SPEED 80         // Velocidad mínima para superar fricción

class MotorDriver {
private:
    byte currentSpeed;
    
public:
    void init();
    
    // Control básico de movimiento
    void moveForward(int speed = DEFAULT_SPEED);
    void moveBackward(int speed = DEFAULT_SPEED);
    void turnLeft(int speed = TURN_SPEED);
    void turnRight(int speed = TURN_SPEED);
    void stop();
    
    // Control individual de motores
    void setLeftMotor(int speed);   // speed: -255 a 255
    void setRightMotor(int speed);  // speed: -255 a 255
    void setBothMotors(int leftSpeed, int rightSpeed);
    
    // Control heredado
    void setSpeed(int left, int right);
    
    // Funciones de diagnóstico
    void testMotors();              // Test automático de motores
    void testRightMotor();          // Diagnóstico específico motor derecho
    
    // Getters
    int getCurrentSpeed() { return currentSpeed; }
};

#endif // MOTOR_DRIVER_H