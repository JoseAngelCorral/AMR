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
 * 
 * CONEXIONES:
 * Encoder Izq:  A=Pin2, B=Pin4
 * Encoder Der:  A=Pin3, B=Pin5
 * Motor Izq:    RPWM=Pin5, LPWM=Pin6, REN=Pin7, LEN=Pin8
 * Motor Der:    RPWM=Pin9, LPWM=Pin10, REN=Pin11, LEN=Pin12
 */

// ========================================
//             LIBRERÍAS
// ========================================
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
unsigned long lastPositionDisplay = 0;
const unsigned long POSITION_UPDATE_INTERVAL = 50;   // Actualizar cada 50ms
const unsigned long POSITION_DISPLAY_INTERVAL = 2000; // Mostrar cada 2s

// Variables para giros automáticos
bool turningInProgress = false;
float targetAngle = 0;
unsigned long turnStartTime = 0;
const unsigned long MAX_TURN_TIME = 4000; // 4 segundos máximo para girar

// ========================================
//              SETUP
// ========================================
void setup() {
    Serial.begin(115200);
    
    // Banner de inicio
    Serial.println(F("========================================"));
    Serial.println(F("        AMR - CONTROL SYSTEM"));
    Serial.println(F("========================================"));
    Serial.println(F("Hardware:"));
    Serial.println(F("- Arduino Uno"));
    Serial.println(F("- Encoder E386G5: 1200 PPR"));
    Serial.println(F("- Driver BTS7960: 43A máx"));
    Serial.println(F("- Ruedas: 16cm diámetro"));
    Serial.println(F("- Posición inicial: (0,0)"));
    Serial.println();
    Serial.println(F("CONTROLES:"));
    Serial.println(F("  W = Adelante     S = Atrás"));
    Serial.println(F("  A = Giro Izq     D = Giro Der"));
    Serial.println(F("  X = Parar        R = Reset"));
    Serial.println(F("  P = Posición     H = Ayuda"));
    Serial.println(F("========================================"));
    
    // Inicializar hardware
    Serial.print(F("Inicializando motores... "));
    motors.init();
    Serial.println(F("OK"));
    
    Serial.print(F("Inicializando encoders... "));
    encoders.init();
    Serial.println(F("OK"));
    
    Serial.print(F("Inicializando odometría... "));
    odometry.init(0.0, 0.0, 0.0);
    Serial.println(F("OK"));
    
    Serial.println();
    Serial.println(F("¡SISTEMA LISTO! Envía comandos por serie."));
    Serial.println(F("Posición inicial: (0.00, 0.00) - 0.0°"));
    Serial.println(F("========================================"));
    Serial.println();
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
    
    // Mostrar posición periódicamente
    if (millis() - lastPositionDisplay >= POSITION_DISPLAY_INTERVAL) {
        Serial.print(F("POS: "));
        odometry.printPosition();
        lastPositionDisplay = millis();
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
        Serial.println(F("⚠ Giro en progreso, usa X para parar"));
        return;
    }
    
    switch (cmd) {
        case 'W':
            Serial.println(F("→ ADELANTE"));
            motors.moveForward();
            break;
            
        case 'S':
            Serial.println(F("→ ATRÁS"));
            motors.moveBackward();
            break;
            
        case 'A':
            Serial.println(F("→ GIRANDO IZQUIERDA 90°"));
            startAutoTurn(-90);
            break;
            
        case 'D':
            Serial.println(F("→ GIRANDO DERECHA 90°"));
            startAutoTurn(90);
            break;
            
        case 'X':
            Serial.println(F("→ PARAR"));
            motors.stop();
            turningInProgress = false;
            break;
            
        case 'R':
            Serial.println(F("→ RESET POSICIÓN"));
            motors.stop();
            odometry.resetPosition();
            turningInProgress = false;
            Serial.println(F("✓ Posición reseteada a (0,0) - 0°"));
            break;
            
        case 'P':
            Serial.print(F("→ POSICIÓN ACTUAL: "));
            odometry.printPosition();
            break;
            
        case 'H':
            showHelp();
            break;
            
        case '\r':
        case '\n':
            // Ignorar caracteres de nueva línea
            break;
            
        default:
            if (isPrintable(cmd)) {
                Serial.print(F("? Comando desconocido: "));
                Serial.println(cmd);
                Serial.println(F("Envía 'H' para ayuda"));
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
    
    Serial.print(F("Objetivo: "));
    Serial.print(targetAngle, 1);
    Serial.println(F("°"));
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
        Serial.print(F("✓ Giro completo: "));
        Serial.print(currentAngle, 1);
        Serial.println(F("°"));
        return;
    }
    
    // Verificar timeout
    if (millis() - turnStartTime > MAX_TURN_TIME) {
        motors.stop();
        turningInProgress = false;
        Serial.print(F("⚠ Timeout giro: "));
        Serial.print(currentAngle, 1);
        Serial.print(F("° (obj: "));
        Serial.print(targetAngle, 1);
        Serial.println(F("°)"));
    }
}

// ========================================
//            FUNCIONES AYUDA
// ========================================
void showHelp() {
    Serial.println();
    Serial.println(F("========== COMANDOS =========="));
    Serial.println(F("MOVIMIENTO:"));
    Serial.println(F("  W - Avanzar"));
    Serial.println(F("  S - Retroceder"));
    Serial.println(F("  A - Girar izquierda 90°"));
    Serial.println(F("  D - Girar derecha 90°"));
    Serial.println(F("  X - Parar todo"));
    Serial.println();
    Serial.println(F("INFORMACIÓN:"));
    Serial.println(F("  P - Mostrar posición"));
    Serial.println(F("  R - Reset a origen (0,0)"));
    Serial.println(F("  H - Esta ayuda"));
    Serial.println();
    Serial.println(F("ESTADO ACTUAL:"));
    Serial.print(F("  Posición: "));
    odometry.printPosition();
    Serial.println(F("============================="));
    Serial.println();
}

// ========================================
//            UTILIDADES
// ========================================
bool isPrintable(char c) {
    return (c >= 32 && c <= 126);
}