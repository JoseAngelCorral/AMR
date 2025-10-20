#pragma once

#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>

// Configuración del encoder E386G5 con triplicador
#define PULSES_PER_REVOLUTION 1200   // E386G5 (400 PPR) × 3 = 1200 pulsos por revolución
#define WHEEL_DIAMETER_CM 16.0       // Diámetro de la rueda en centímetros
#define WHEEL_CIRCUMFERENCE_CM (PI * WHEEL_DIAMETER_CM)  // Circunferencia en cm

// Pines para Arduino Uno - Configuración personalizada
#define ENCODER_LEFT_A_PIN 13        // Pin A del encoder izquierdo
#define ENCODER_LEFT_B_PIN 3         // Pin B del encoder izquierdo (INT1)
#define ENCODER_RIGHT_A_PIN 12       // Pin A del encoder derecho
#define ENCODER_RIGHT_B_PIN 2        // Pin B del encoder derecho (INT0)

class Encoder {
private:
    static volatile long leftPulses;     // Contador de pulsos del encoder izquierdo
    static volatile long rightPulses;    // Contador de pulsos del encoder derecho
    
public:
    // Inicialización del encoder
    void init();
    
    // Lectura de pulsos
    long readLeft();
    long readRight();
    
    // Reset de contadores
    void resetLeft();
    void resetRight();
    void resetBoth();
    
    // Conversión de pulsos a distancia
    float pulsesToCentimeters(long pulses);
    float pulsesToRevolutions(long pulses);
    
    // Cálculo de distancia diferencial para cada rueda
    float getLeftDistanceCm();
    float getRightDistanceCm();
    
    // Funciones de interrupción estáticas
    static void leftEncoderISR();
    static void rightEncoderISR();
    
    // Getters para configuración
    static int getPulsesPerRevolution() { return PULSES_PER_REVOLUTION; }
    static float getWheelDiameter() { return WHEEL_DIAMETER_CM; }
};

#endif // ENCODER_H