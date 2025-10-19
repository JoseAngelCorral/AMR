#pragma once

#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include <Arduino.h>

// Pines para control de motores BTS7960 - Arduino Uno
// Motor Izquierdo (BTS7960 #1)
#define MOTOR_LEFT_RPWM 5    // PWM Adelante motor izquierdo
#define MOTOR_LEFT_LPWM 6    // PWM Atrás motor izquierdo
#define MOTOR_LEFT_REN 7     // Enable Adelante motor izquierdo
#define MOTOR_LEFT_LEN 8     // Enable Atrás motor izquierdo

// Motor Derecho (BTS7960 #2)  
#define MOTOR_RIGHT_RPWM 9   // PWM Adelante motor derecho
#define MOTOR_RIGHT_LPWM 10  // PWM Atrás motor derecho
#define MOTOR_RIGHT_REN 11   // Enable Adelante motor derecho
#define MOTOR_RIGHT_LEN 12   // Enable Atrás motor derecho

// Velocidades para BTS7960 (más potente que L298N)
#define DEFAULT_SPEED 180    // Velocidad por defecto (0-255)
#define TURN_SPEED 150       // Velocidad para giros
#define MAX_SPEED 255        // Velocidad máxima
#define MIN_SPEED 80         // Velocidad mínima para superar fricción

class MotorDriver {
private:
    int currentSpeed;
    
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
    
    // Getters
    int getCurrentSpeed() { return currentSpeed; }
};

#endif // MOTOR_DRIVER_H