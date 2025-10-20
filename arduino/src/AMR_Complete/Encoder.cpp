#include "Encoder.h"

// Inicialización de variables estáticas
volatile long Encoder::leftPulses = 0;
volatile long Encoder::rightPulses = 0;

void Encoder::init() {
    // Configurar pines como entrada con pull-up interno
    pinMode(ENCODER_LEFT_A_PIN, INPUT_PULLUP);
    pinMode(ENCODER_LEFT_B_PIN, INPUT_PULLUP);
    pinMode(ENCODER_RIGHT_A_PIN, INPUT_PULLUP);
    pinMode(ENCODER_RIGHT_B_PIN, INPUT_PULLUP);
    
    // Configurar interrupciones para Arduino Uno 
    // Solo pin 2 (INT0) y pin 3 (INT1) soportan interrupciones
    attachInterrupt(1, leftEncoderISR, CHANGE);   // INT1 en pin 3 (Left B)
    attachInterrupt(0, rightEncoderISR, CHANGE);  // INT0 en pin 2 (Right B)
    
    // Reset contadores
    resetBoth();
    
    Serial.println(F("Enc OK 1200PPR"));
}

long Encoder::readLeft() {
    long temp;
    noInterrupts();
    temp = leftPulses;
    interrupts();
    return temp;
}

long Encoder::readRight() {
    long temp;
    noInterrupts();
    temp = rightPulses;
    interrupts();
    return temp;
}

void Encoder::resetLeft() {
    noInterrupts();
    leftPulses = 0;
    interrupts();
}

void Encoder::resetRight() {
    noInterrupts();
    rightPulses = 0;
    interrupts();
}

void Encoder::resetBoth() {
    noInterrupts();
    leftPulses = 0;
    rightPulses = 0;
    interrupts();
}

float Encoder::pulsesToCentimeters(long pulses) {
    // Convertir pulsos a distancia en centímetros
    // Distancia = (pulsos / pulsos_por_revolución) * circunferencia
    float revolutions = (float)pulses / PULSES_PER_REVOLUTION;
    return revolutions * WHEEL_CIRCUMFERENCE_CM;
}

float Encoder::pulsesToRevolutions(long pulses) {
    // Convertir pulsos a revoluciones
    return (float)pulses / PULSES_PER_REVOLUTION;
}

float Encoder::getLeftDistanceCm() {
    return pulsesToCentimeters(readLeft());
}

float Encoder::getRightDistanceCm() {
    return pulsesToCentimeters(readRight());
}

// Función de interrupción para encoder izquierdo
void Encoder::leftEncoderISR() {
    // Leer estados de ambos canales
    bool A = digitalRead(ENCODER_LEFT_A_PIN);
    bool B = digitalRead(ENCODER_LEFT_B_PIN);
    
    // Determinar dirección usando cuadratura
    if (A == B) {
        leftPulses--;  // Giro hacia atrás
    } else {
        leftPulses++;  // Giro hacia adelante
    }
}

// Función de interrupción para encoder derecho
void Encoder::rightEncoderISR() {
    // Leer estados de ambos canales
    bool A = digitalRead(ENCODER_RIGHT_A_PIN);
    bool B = digitalRead(ENCODER_RIGHT_B_PIN);
    
    // Determinar dirección usando cuadratura
    if (A == B) {
        rightPulses--;  // Giro hacia atrás
    } else {
        rightPulses++;  // Giro hacia adelante
    }
}