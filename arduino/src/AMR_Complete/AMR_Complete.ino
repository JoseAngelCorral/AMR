/*
 * ========================================
 *          AMR CONTROL SYSTEM
 * ========================================
 * 
 * HARDWARE:
 * - Arduino Uno
 * - Encoder E386G5 (7000 PPR × 3 = 21000 PPR)
 * - Driver BTS7960 para motores (43A máx)
 * - Ruedas 15.50cm diámetro
 * 
 * CONTROLES SERIE:
 * W = Adelante    S = Atrás
 * A = Izq 90°     D = Der 90°   (giro por encoder)
 * Q = Izq continuo (hasta X)   E = Der continuo (hasta X)
 * X = Parar       R = Reset posición
 * P = Mostrar posición
 * T = Test motores
 *
 * VELOCIDADES (definidas en MotorDriver.h):
 * - DEFAULT_SPEED = 170 -> ~67% de 255 (velocidad por defecto de avance)
 * - TURN_SPEED    = 140 -> ~55% de 255 (velocidad para giros continuos/turn)
 * - MIN_SPEED     = 80  -> ~31% de 255 (velocidad mínima para superar fricción)
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

// Nota: Este sketch controla el robot AMR. Está organizado en secciones
// claras: librerías, instancias globales, variables de control, setup,
// loop principal, procesamiento de comandos, rutinas de giro y utilidades.
// Se mantienen los tests esenciales: 'T' (test de motores), 'V' (avanzar
// 1 vuelta) y 'I' (inspección). Las pruebas de depuración no esenciales
// fueron removidas para simplificar la interfaz serie.

// ========================================
//         INSTANCIAS GLOBALES
// ========================================
MotorDriver motors;
Encoder encoders;
Odometry odometry(&encoders);

// --------------------------------------------------------------------------------
// Nota sobre las instancias:
// - `motors` encapsula el control de los drivers BTS7960 (funciones de movimiento,
//   giro y pruebas de motor).
// - `encoders` gestiona los contadores de los encoders y proporciona utilidades
//   como `getPulsesPerRevolution()`.
// - `odometry` usa `encoders` para calcular la posición (x,y,theta) del robot.
// --------------------------------------------------------------------------------

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
// Variables para control de giros por encoder
long turnStartLeft0 = 0;
long turnStartRight0 = 0;
long turnTargetPulses = 0;

// Variables para imprimir tics mientras se avanza con 'W'
bool printTicksWhileMoving = false;
unsigned long lastTickPrintMillis = 0;
const unsigned int TICK_PRINT_INTERVAL = 100; // ms
long tickPrintLeft0 = 0;
long tickPrintRight0 = 0;

// -------------------------------------------------------------------------------
// Notas sobre las variables de control:
// - `POSITION_UPDATE_INTERVAL` define la frecuencia con la que se actualiza la
//   odometría (en ms). Valores cortos aumentan la precisión a costa de CPU.
// - Las variables relacionadas con `turningInProgress` controlan giros automáticos
//   basados en contadores de encoder y timeout.
// - `printTicksWhileMoving` activa la impresión periódica de tics cuando se usa 'W'.
// -------------------------------------------------------------------------------

// ========================================
//       SENSORES INFRARROJOS (ANALÓGICOS)
// ========================================
// Se asumió que los sensores IR devuelven un valor analógico 0-1023 (5V)
// y que el valor aumenta cuando detectan un objeto. Ajusta el umbral según
// tu modelo de sensor y condiciones de iluminación.

// Mapeo de pines (según indicación del usuario):
// LEFT_SIDE (lateral izquierdo)  -> A0
// FRONT_LEFT (frontal izquierdo) -> A1
// BACK_CENTER (trasero central)  -> A2
// FRONT_RIGHT (frontal derecho)  -> A4
// RIGHT_SIDE (lateral derecho)   -> A5
const int IR_LEFT_SIDE_PIN   = A0; // Lateral izquierdo
const int IR_FRONT_LEFT_PIN  = A1; // Frontal izquierdo
const int IR_BACK_CENTER_PIN = A2; // Trasero central
const int IR_FRONT_RIGHT_PIN = A4; // Frontal derecho
const int IR_RIGHT_SIDE_PIN  = A5; // Lateral derecho

// Parámetros de lectura
const int IR_NUM_SAMPLES = 3;      // número de lecturas para promediar
int IR_THRESHOLD = 600;            // umbral por defecto (0-1023). Ajustar por calibración

// Estructura para devolver lecturas
struct IRSensors {
    int rawLeft;
    int rawFrontLeft;
    int rawBack;
    int rawFrontRight;
    int rawRight;
    bool left;
    bool frontLeft;
    bool back;
    bool frontRight;
    bool right;
};

// Inicializar pines analógicos (no es necesario pinMode para analogRead,
// pero dejamos una función para futura configuración y documentación)
void setupIRSensors() {
    // No es necesario configurar A0..A5 con pinMode para analogRead en Arduino,
    // pero si los sensores necesitan alimentación o referencias externas, eso se
    // debe hacer en el cableado físico.
    // Añadir una pequeña espera para estabilizar sensores si es necesario
    delay(20);
}

// Leer un pin IR con promedio de N muestras
int readIRRaw(int pin) {
    long acc = 0;
    for (int i = 0; i < IR_NUM_SAMPLES; ++i) {
        acc += analogRead(pin);
        delay(4);
    }
    int avg = (int)(acc / IR_NUM_SAMPLES);
    return avg;
}

IRSensors readIRSensors() {
    IRSensors s;
    s.rawLeft = readIRRaw(IR_LEFT_SIDE_PIN);
    s.rawFrontLeft = readIRRaw(IR_FRONT_LEFT_PIN);
    s.rawBack = readIRRaw(IR_BACK_CENTER_PIN);
    s.rawFrontRight = readIRRaw(IR_FRONT_RIGHT_PIN);
    s.rawRight = readIRRaw(IR_RIGHT_SIDE_PIN);

    // Detección booleana (suponer HIGH -> mayor valor -> detectado)
    s.left = s.rawLeft >= IR_THRESHOLD;
    s.frontLeft = s.rawFrontLeft >= IR_THRESHOLD;
    s.back = s.rawBack >= IR_THRESHOLD;
    s.frontRight = s.rawFrontRight >= IR_THRESHOLD;
    s.right = s.rawRight >= IR_THRESHOLD;
    return s;
}

// Enviar telemetría simple por Serial
void sendIRTelemetry(const IRSensors &s) {
    Serial.print(F("IR: "));
    Serial.print(F("L:")); Serial.print(s.left ? 1 : 0); Serial.print(F("(")); Serial.print(s.rawLeft); Serial.print(F(") "));
    Serial.print(F("FL:")); Serial.print(s.frontLeft ? 1 : 0); Serial.print(F("(")); Serial.print(s.rawFrontLeft); Serial.print(F(") "));
    Serial.print(F("B:")); Serial.print(s.back ? 1 : 0); Serial.print(F("(")); Serial.print(s.rawBack); Serial.print(F(") "));
    Serial.print(F("FR:")); Serial.print(s.frontRight ? 1 : 0); Serial.print(F("(")); Serial.print(s.rawFrontRight); Serial.print(F(") "));
    Serial.print(F("R:")); Serial.print(s.right ? 1 : 0); Serial.print(F("(")); Serial.print(s.rawRight); Serial.println(F(")"));
}


// ========================================
//              SETUP
// ========================================
void setup() {
    Serial.begin(115200);
    
    // Banner de inicio
    Serial.println(F("=== AMR SYSTEM ==="));
    Serial.println(F("Enc:3850PPR Ruedas:15.5cm"));
    Serial.println(F("W/S:Adelante/Atras"));
    Serial.println(F("A/D:Izq/Der X:Stop"));
    Serial.println(F("P:Pos R:Reset T:Test"));
    
    // Inicializar hardware
    motors.init();
    encoders.init();
    odometry.init(0.0, 0.0, 0.0);
    // Inicializar sensores IR analógicos (A0..A5)
    setupIRSensors();
    
    Serial.println(F("LISTO! Pos:(0,0)"));
}

// ========================================
//                SETUP NOTES
// ========================================
// - Inicializamos `Serial` a 115200 baudios para la consola y control por USB.
// - Se inicializa el hardware: drivers (motors), encoders y odometría.
// - Si se integra el WiFi del R4, se puede abrir `Serial1` aquí para duplicar
//   telemetría hacia la interfaz de red (no activado por defecto).


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

    // Si estamos en modo impresión de tics mientras avanzamos (comando 'W')
    if (printTicksWhileMoving && millis() - lastTickPrintMillis >= TICK_PRINT_INTERVAL) {
        long dl = encoders.readLeft() - tickPrintLeft0;
        long dr = encoders.readRight() - tickPrintRight0;
        if (dl < 0) dl = 0;
        if (dr < 0) dr = 0;
        long avg = (dl + dr) / 2;
        Serial.print(F("Ticks L:")); Serial.print(dl);
        Serial.print(F(" R:")); Serial.print(dr);
        Serial.print(F(" Avg:")); Serial.println(avg);
        lastTickPrintMillis = millis();
    }
    
    delay(5); // Pequeña pausa para estabilidad
}

// ========================================
//                LOOP NOTES
// ========================================
// - El loop principal se encarga de:
//   1) Actualizar la odometría a intervalos regulares.
//   2) Leer y procesar comandos recibidos por `Serial`.
//   3) Gestionar giros automáticos y tareas periódicas (impresión de tics).
// - Mantener el delay(5) ayuda a estabilizar lecturas y evita bloquear CPU.

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
     

    // Agrupamos los comandos por tipo para mantener el código legible:
    //  - Movimiento: W/S/A/D/Q/E
    //  - Acciones utilitarias: X, R, P, H, I
    //  - Tests: T, V
    switch (cmd) {
    // ---------------------------
    // MOVIMIENTO: Adelante / Atrás / Giros
    // ---------------------------
    case 'W':
            Serial.println(F("Adelante"));
            // Iniciar impresión de tics mientras avanzamos
            tickPrintLeft0 = encoders.readLeft();
            tickPrintRight0 = encoders.readRight();
            lastTickPrintMillis = millis();
            printTicksWhileMoving = true;
            Serial.print(F("Imprimiendo tics cada "));
            Serial.print(TICK_PRINT_INTERVAL);
            Serial.println(F(" ms"));
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
            
        case 'Q':
            // Giro continuo a la izquierda hasta recibir 'X'
            Serial.println(F("Giro Izq continuo (hasta X)"));
            motors.turnLeft();
            break;
            
        

        // Removed debug tests (Z, Y) to keep only essential tests
            
        case 'E':
            // Giro continuo a la derecha hasta recibir 'X'
            Serial.println(F("Giro Der continuo (hasta X)"));
            motors.turnRight();
            break;

    // ---------------------------
    // TEST: Avanzar 1 vuelta (calibración de encoder)
    // ---------------------------
    case 'V':
            // Avanzar exactamente una revolución de rueda (ambas ruedas, promedio de encoders)
            Serial.println(F("Avanzar 1 vuelta"));
            {
                // Leer contadores iniciales
                long left0 = encoders.readLeft();
                long right0 = encoders.readRight();
                int target = encoders.getPulsesPerRevolution();
                unsigned long lastPrint = millis();

                Serial.print(F("Target pulses: "));
                Serial.println(target);

                // Arrancar motores hacia adelante
                motors.moveForward();

                // Esperar hasta alcanzar el objetivo (basado en la rueda que más avance)
                while (true) {
                    long dl = encoders.readLeft() - left0;
                    long dr = encoders.readRight() - right0;
                    if (dl < 0) dl = 0; // proteger contra lecturas invertidas momentáneas
                    if (dr < 0) dr = 0;
                    long maxv = (dl > dr) ? dl : dr;
                    if (maxv >= target) break;

                    // Imprimir tics periódicamente para ver progreso
                    if (millis() - lastPrint >= 100) {
                        long avg = (dl + dr) / 2;
                        Serial.print(F("Ticks L:")); Serial.print(dl);
                        Serial.print(F(" R:")); Serial.print(dr);
                        Serial.print(F(" Avg:")); Serial.print(avg);
                        Serial.print(F(" Max:")); Serial.println(maxv);
                        lastPrint = millis();
                    }

                    delay(20);
                }

                motors.stop();
                // Mostrar conteo final
                long finalL = encoders.readLeft() - left0;
                long finalR = encoders.readRight() - right0;
                Serial.print(F("Final L:")); Serial.print(finalL);
                Serial.print(F(" R:")); Serial.println(finalR);
                // Calcular pulso medido por vuelta (usar la rueda que más pulses registró)
                long measured = abs(finalL) > abs(finalR) ? abs(finalL) : abs(finalR);
                Serial.print(F("Measured pulses/rev:")); Serial.println(measured);
                // Actualizar configuración runtime
                encoders.setPulsesPerRevolution((int)measured);
                Serial.print(F("Pulses_per_rev updated to: ")); Serial.println(encoders.getPulsesPerRevolution());
                Serial.println(F("Hecho: 1 vuelta"));
            }
            break;
            
    // ---------------------------
    // UTILERÍAS / CONTROL
    // ---------------------------
    case 'X':
            Serial.println(F("Stop"));
            motors.stop();
            turningInProgress = false;
            // Detener impresión de tics si estaba activa
            printTicksWhileMoving = false;
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
            
    // ---------------------------
    // TEST: Test completo de motores
    // ---------------------------
    case 'T':
            Serial.println(F("Test"));
            motors.testMotors();
            break;
            
        // 'M' (diagnóstico motor derecho) removed - mantiene solo el test completo 'T'

        case 'I':
            // Inspección rápida de pines de encoder y estados
            Serial.println(F("Inspeccion: estados de pines de encoder"));
            Serial.print(F("L_A:")); Serial.print(ENCODER_LEFT_A_PIN);
            Serial.print(F(" val:")); Serial.print(digitalRead(ENCODER_LEFT_A_PIN));
            Serial.print(F(" L_B:")); Serial.print(ENCODER_LEFT_B_PIN);
            Serial.print(F(" val:")); Serial.print(digitalRead(ENCODER_LEFT_B_PIN));
            Serial.print(F(" R_A:")); Serial.print(ENCODER_RIGHT_A_PIN);
            Serial.print(F(" val:")); Serial.print(digitalRead(ENCODER_RIGHT_A_PIN));
            Serial.print(F(" R_B:")); Serial.print(ENCODER_RIGHT_B_PIN);
            Serial.print(F(" val:")); Serial.println(digitalRead(ENCODER_RIGHT_B_PIN));
            Serial.print(F("int L_B:")); Serial.print(digitalPinToInterrupt(ENCODER_LEFT_B_PIN));
            Serial.print(F(" int R_B:")); Serial.println(digitalPinToInterrupt(ENCODER_RIGHT_B_PIN));
            Serial.print(F("Pulses L:")); Serial.print(encoders.readLeft());
            Serial.print(F(" R:")); Serial.println(encoders.readRight());
            // Leer y mostrar sensores IR (raw + detectado)
            {
                IRSensors ir = readIRSensors();
                sendIRTelemetry(ir);
            }
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
    // Preparar giro basado en encoders
    turningInProgress = true;
    targetAngle = odometry.getThetaDegrees() + angleDelta;

    // Normalizar ángulo objetivo
    while (targetAngle > 180) targetAngle -= 360;
    while (targetAngle < -180) targetAngle += 360;

    // Calcular pulsos necesarios para este ángulo (por rueda)
    float pulsesF = (abs(angleDelta) * (float)encoders.getPulsesPerRevolution() * (float)WHEEL_BASE_CM) / (360.0 * (float)WHEEL_DIAMETER_CM);
    turnTargetPulses = (long)(pulsesF + 0.5);

    // Guardar contadores de inicio
    turnStartLeft0 = encoders.readLeft();
    turnStartRight0 = encoders.readRight();
    turnStartTime = millis();

    // Iniciar movimiento: sentido según signo del ángulo
    if (angleDelta > 0) {
        // Giro derecha: motor izquierdo adelante, motor derecho atrás
        motors.setBothMotors(TURN_SPEED, -TURN_SPEED);
    } else {
        // Giro izquierda: motor izquierdo atrás, motor derecho adelante
        motors.setBothMotors(-TURN_SPEED, TURN_SPEED);
    }

    Serial.print(F("Obj:"));
    Serial.print(targetAngle, 0);
    Serial.print(F(" targetPulses:")); Serial.println(turnTargetPulses);
}

// ========================================
//            GIROS AUTOMÁTICOS (Notas)
// ========================================
// - `startAutoTurn` calcula la cantidad de pulsos de encoder necesarios para
//   girar el ángulo solicitado y arranca los motores en sentido opuesto para
//   producir un giro en sitio. El seguimiento se realiza en `handleAutoTurn`.
// - Los cálculos usan WHEEL_BASE_CM y WHEEL_DIAMETER_CM definidos en Encoder.h
//   o en la configuración del proyecto.

void handleAutoTurn() {
    if (!turningInProgress) return;

    // Comprobar avance por encoders
    long dl = encoders.readLeft() - turnStartLeft0;
    long dr = encoders.readRight() - turnStartRight0;
    long adl = abs(dl);
    long adr = abs(dr);
    long maxMoved = (adl > adr) ? adl : adr;

    // Mostrar progreso ocasionalmente
    // Si alcanzamos la cantidad de pulsos objetivo, paramos
    if (maxMoved >= turnTargetPulses) {
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
    Serial.println(F("W:Adelante (imprime tics) / S:Atras"));
    Serial.println(F("A/D:Izq/Der 90"));
    Serial.println(F("X:Stop P:Pos R:Reset"));
    Serial.println(F("T:Test (motores) V:Avanzar 1 vuelta I:Inspeccionar"));
    odometry.printPosition();
}

// ========================================
//               AYUDA / DOCUMENTACIÓN
// ========================================
// - `showHelp()` imprime en Serial los comandos disponibles.
// - Mantén esta función actualizada si agregas/quitas comandos.

// ========================================
//            UTILIDADES
// ========================================
bool isPrintable(char c) {
    return (c >= 32 && c <= 126);
}