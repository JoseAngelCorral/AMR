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

// Velocidades para BTS7960 (más potente que L298N)
// Valores ajustados: ver README/AMR_Complete para porcentajes
#define DEFAULT_SPEED 170    // Velocidad por defecto (0-255)
#define TURN_SPEED 140       // Velocidad para giros
#define MAX_SPEED 255        // Velocidad máxima
#define MIN_SPEED 80         // Velocidad mínima para superar fricción

class MotorDriver {
private:
    byte currentSpeed;
    // --- PID velocity control members ---
    bool velocityControlEnabled = false;
    unsigned long lastPIDMillis = 0;
    unsigned int pidIntervalMs = 50; // PID update interval (ms)

    // Targets and applied setpoints (pulses per second)
    float targetLeftPps = 0.0f;
    float targetRightPps = 0.0f;
    float appliedLeftPps = 0.0f;
    float appliedRightPps = 0.0f;

    // PID state
    float Kp = 0.08f; // initial guess, tune on hardware
    float Ki = 0.02f;
    float Kd = 0.002f;
    float leftIntegral = 0.0f;
    float rightIntegral = 0.0f;
    float leftPrevError = 0.0f;
    float rightPrevError = 0.0f;
    float integralClamp = 500.0f; // anti-windup

    // Soft-start ramp time in ms (time to go from 0 -> target)
    unsigned long rampTimeMs = 800;
    
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
    
    // --- Velocity PID API ---
    // Enable/disable closed-loop velocity control
    void enableVelocityControl(bool en);
    bool isVelocityControlEnabled();

    // Set PID gains (tune on hardware)
    void setPIDGains(float kp, float ki, float kd);

    // Set PID update interval
    void setPIDInterval(unsigned int ms);

    // Set ramp time (ms) for soft-start
    void setRampTime(unsigned long ms);

    // Set target speed in pulses per second (encoder pulses/sec)
    void setTargetPulsesPerSecond(float leftPps, float rightPps);

    // Must be called periodically (from main loop) with encoder deltas and dt
    void updateVelocityControl(long leftDeltaPulses, long rightDeltaPulses, unsigned long dtMs);
    
    // Getters
    int getCurrentSpeed() { return currentSpeed; }
};

#endif // MOTOR_DRIVER_H