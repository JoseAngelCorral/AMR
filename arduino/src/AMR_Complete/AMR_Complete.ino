/*
 * ========================================
 *          AMR CONTROL SYSTEM
 * ========================================
 * 
 * HARDWARE:
 * - Arduino Uno
 * - Encoder E386G5 (400 PPR × 3 = 1200 PPR)
 * - Driver BTS7960 para motores (43A máx)
 * - Ruedas 16cm diámetro
 * 
 * CONTROLES SERIE:
 * W = Adelante    S = Atrás
 * A = Izq 90°     D = Der 90° 
 * X = Parar       R = Reset posición
 * P = Mostrar posición
 * T = Test motores M = Diagnóstico motor derecho
 * 
 * CONEXIONES:
 * Encoder Izq:  A=Pin13, B=Pin3 (INT1)
 * Encoder Der:  A=Pin12, B=Pin2 (INT0)  
 * Motor Izq:    RPWM=Pin10, LPWM=Pin11 (LPWM=Adelante)
 * Motor Der:    RPWM=Pin5, LPWM=Pin6 (LPWM=Adelante)
 * Enables:      Alimentación externa (siempre HIGH)
 */

// ========================================
//             LIBRERÍAS
// ========================================
#include <avr/pgmspace.h>  // Para PROGMEM
#include "MotorDriver.h"
#include "Encoder.h"
#include "Odometry.h"

// ========================================
//         INSTANCIAS GLOBALES
// ========================================
MotorDriver motors;
Encoder encoders;
Odometry odometry(&encoders);

// ========================================
//         VARIABLES DE CONTROL
// ========================================
unsigned long lastPositionUpdate = 0;
const unsigned int POSITION_UPDATE_INTERVAL = 50;   // Actualizar cada 50ms

// Variables para giros automáticos
bool turningInProgress = false;
float targetAngle = 0;
unsigned long turnStartTime = 0;
const unsigned int MAX_TURN_TIME = 4000; // 4 segundos máximo para girar

// ========================================
//              SETUP
// ========================================
void setup() {
    Serial.begin(115200);
    
    // Banner de inicio
    Serial.println(F("=== AMR SYSTEM ==="));
    Serial.println(F("Enc:1200PPR Ruedas:16cm"));
    Serial.println(F("W/S:Adelante/Atras"));
    Serial.println(F("A/D:Izq/Der X:Stop"));
    Serial.println(F("P:Pos R:Reset T:Test"));
    
    // Inicializar hardware
    motors.init();
    encoders.init();
    odometry.init(0.0, 0.0, 0.0);
    
    Serial.println(F("LISTO! Pos:(0,0)"));
}


// ========================================
//            LOOP PRINCIPAL
// ========================================
void loop() {
    // Actualizar odometría frecuentemente
    if (millis() - lastPositionUpdate >= POSITION_UPDATE_INTERVAL) {
        odometry.update();
        lastPositionUpdate = millis();
    }
    
    // Procesar comandos serie
    if (Serial.available()) {
        char command = Serial.read();
        processCommand(command);
        
        // Limpiar buffer serie
        while (Serial.available()) {
            Serial.read();
        }
    }
    
    // Manejar giros automáticos
    handleAutoTurn();
    
    delay(5); // Pequeña pausa para estabilidad
}

// ========================================
//         PROCESAMIENTO COMANDOS
// ========================================
void processCommand(char cmd) {
    cmd = toupper(cmd);
    
    // Prevenir comandos durante giro automático
    if (turningInProgress && cmd != 'X') {
        Serial.println(F("Girando..."));
        return;
    }
    
    switch (cmd) {
        case 'W':
            Serial.println(F("Adelante"));
            motors.moveForward();
            break;
            
        case 'S':
            Serial.println(F("Atras"));
            motors.moveBackward();
            break;
            
        case 'A':
            Serial.println(F("Izq 90"));
            startAutoTurn(-90);
            break;
            
        case 'D':
            Serial.println(F("Der 90"));
            startAutoTurn(90);
            break;
            
        case 'X':
            Serial.println(F("Stop"));
            motors.stop();
            turningInProgress = false;
            break;
            
        case 'R':
            Serial.println(F("Reset"));
            motors.stop();
            odometry.resetPosition();
            turningInProgress = false;
            break;
            
        case 'P':
            odometry.printPosition();
            break;
            
        case 'H':
            showHelp();
            break;
            
        case 'T':
            Serial.println(F("Test"));
            motors.testMotors();
            break;
            
        case 'M':
            Serial.println(F("Diag Der"));
            motors.testRightMotor();
            break;
            
        case '\r':
        case '\n':
            // Ignorar caracteres de nueva línea
            break;
            
        default:
            if (isPrintable(cmd)) {
                Serial.print(F("? "));
                Serial.println(cmd);
            }
            break;
    }
}

// ========================================
//            GIROS AUTOMÁTICOS
// ========================================
void startAutoTurn(float angleDelta) {
    turningInProgress = true;
    targetAngle = odometry.getThetaDegrees() + angleDelta;
    
    // Normalizar ángulo objetivo
    while (targetAngle > 180) targetAngle -= 360;
    while (targetAngle < -180) targetAngle += 360;
    
    turnStartTime = millis();
    
    // Iniciar movimiento
    if (angleDelta > 0) {
        motors.turnRight();
    } else {
        motors.turnLeft();
    }
    
    Serial.print(F("Obj:"));
    Serial.println(targetAngle, 0);
}

void handleAutoTurn() {
    if (!turningInProgress) return;
    
    float currentAngle = odometry.getThetaDegrees();
    float angleDiff = targetAngle - currentAngle;
    
    // Normalizar diferencia
    while (angleDiff > 180) angleDiff -= 360;
    while (angleDiff < -180) angleDiff += 360;
    
    // Verificar si alcanzamos el objetivo (±8° de tolerancia)
    if (abs(angleDiff) <= 8.0) {
        motors.stop();
        turningInProgress = false;
        Serial.println(F("OK"));
        return;
    }
    
    // Verificar timeout
    if (millis() - turnStartTime > MAX_TURN_TIME) {
        motors.stop();
        turningInProgress = false;
        Serial.println(F("Timeout"));
        return;
    }
}

// ========================================
//            FUNCIONES AYUDA
// ========================================
void showHelp() {
    Serial.println(F("=== COMANDOS ==="));
    Serial.println(F("W/S:Adel/Atras"));
    Serial.println(F("A/D:Izq/Der 90"));
    Serial.println(F("X:Stop P:Pos R:Reset"));
    Serial.println(F("T:Test M:DiagDer"));
    odometry.printPosition();
}

// ========================================
//            UTILIDADES
// ========================================
bool isPrintable(char c) {
    return (c >= 32 && c <= 126);
}