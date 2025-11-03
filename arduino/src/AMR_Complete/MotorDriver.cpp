#include "MotorDriver.h"

void MotorDriver::init() {
    // Configurar solo pines PWM como salida (enables externos)
    // Motor Izquierdo
    pinMode(MOTOR_LEFT_RPWM, OUTPUT);  // PWM Adelante
    pinMode(MOTOR_LEFT_LPWM, OUTPUT);  // PWM Atrás
    
    // Motor Derecho
    pinMode(MOTOR_RIGHT_RPWM, OUTPUT); // PWM Adelante
    pinMode(MOTOR_RIGHT_LPWM, OUTPUT); // PWM Atrás
    
    // Los enables están alimentados externamente (siempre HIGH)
    // No necesitamos configurar REN/LEN
    
    // Inicializar en estado parado
    stop();
    currentSpeed = 0;
    
    Serial.println(F("=== BTS7960 Init ==="));
    Serial.println(F("MotIzq:10,11 MotDer:5,6"));
    Serial.println(F("Enables externos. Usa 'T'"));
}

void MotorDriver::moveForward(int speed) {
    // Limitar velocidad mínima y máxima
    speed = constrain(speed, MIN_SPEED, MAX_SPEED);
    currentSpeed = speed;
    
    // Motor izquierdo hacia adelante (LPWM activo - corregido)
    analogWrite(MOTOR_LEFT_RPWM, 0);       // PWM adelante = 0
    analogWrite(MOTOR_LEFT_LPWM, speed);   // PWM atrás
    
    // Motor derecho hacia adelante (LPWM activo - corregido)
    analogWrite(MOTOR_RIGHT_RPWM, 0);      // PWM adelante = 0
    analogWrite(MOTOR_RIGHT_LPWM, speed);  // PWM atrás
}

void MotorDriver::moveBackward(int speed) {
    // Limitar velocidad mínima y máxima
    speed = constrain(speed, MIN_SPEED, MAX_SPEED);
    currentSpeed = speed;
    
    // Motor izquierdo hacia atrás (RPWM activo - corregido)
    analogWrite(MOTOR_LEFT_RPWM, speed);   // PWM adelante
    analogWrite(MOTOR_LEFT_LPWM, 0);       // PWM atrás = 0
    
    // Motor derecho hacia atrás (RPWM activo - corregido)
    analogWrite(MOTOR_RIGHT_RPWM, speed);  // PWM adelante
    analogWrite(MOTOR_RIGHT_LPWM, 0);      // PWM atrás = 0
}

void MotorDriver::turnLeft(int speed) {
    // Limitar velocidad
    speed = constrain(speed, MIN_SPEED, MAX_SPEED);
    currentSpeed = speed;
    
    Serial.print(F("TurnL: speed="));
    Serial.println(speed);
    
    // Motor izquierdo hacia atrás (giro en su lugar - corregido)
    analogWrite(MOTOR_LEFT_RPWM, speed);   // PWM adelante
    analogWrite(MOTOR_LEFT_LPWM, 0);       // PWM atrás = 0
    Serial.print(F("IzqR="));
    Serial.println(speed);
    
    // Motor derecho hacia adelante (corregido)
    analogWrite(MOTOR_RIGHT_RPWM, 0);      // PWM adelante = 0
    analogWrite(MOTOR_RIGHT_LPWM, speed);  // PWM atrás
    Serial.print(F("DerL="));
    Serial.println(speed);
}

void MotorDriver::turnRight(int speed) {
    // Limitar velocidad
    speed = constrain(speed, MIN_SPEED, MAX_SPEED);
    currentSpeed = speed;
    
    Serial.print(F("TurnR: speed="));
    Serial.println(speed);
    
    // Motor izquierdo hacia adelante (corregido)
    analogWrite(MOTOR_LEFT_RPWM, 0);       // PWM adelante = 0
    analogWrite(MOTOR_LEFT_LPWM, speed);   // PWM atrás
    Serial.print(F("IzqL="));
    Serial.println(speed);
    
    // Motor derecho hacia atrás (corregido)
    analogWrite(MOTOR_RIGHT_RPWM, speed);  // PWM adelante
    analogWrite(MOTOR_RIGHT_LPWM, 0);      // PWM atrás = 0
    Serial.print(F("DerR="));
    Serial.println(speed);
}

void MotorDriver::stop() {
    currentSpeed = 0;
    
    // Parar ambos motores BTS7960 (todos los PWM a 0)
    analogWrite(MOTOR_LEFT_RPWM, 0);
    analogWrite(MOTOR_LEFT_LPWM, 0);
    analogWrite(MOTOR_RIGHT_RPWM, 0);
    analogWrite(MOTOR_RIGHT_LPWM, 0);
}

void MotorDriver::setLeftMotor(int speed) {
    // Limitar velocidad entre -255 y 255
    speed = constrain(speed, -MAX_SPEED, MAX_SPEED);
    
    if (speed > 0) {
        // Hacia adelante - usar LPWM (corregido)
        if (speed < MIN_SPEED) speed = MIN_SPEED; // Velocidad mínima
        analogWrite(MOTOR_LEFT_RPWM, 0);
        analogWrite(MOTOR_LEFT_LPWM, speed);
    } else if (speed < 0) {
        // Hacia atrás - usar RPWM (corregido)
        int absSpeed = -speed;
        if (absSpeed < MIN_SPEED) absSpeed = MIN_SPEED;
        analogWrite(MOTOR_LEFT_RPWM, absSpeed);
        analogWrite(MOTOR_LEFT_LPWM, 0);
    } else {
        // Parado
        analogWrite(MOTOR_LEFT_RPWM, 0);
        analogWrite(MOTOR_LEFT_LPWM, 0);
    }
}

void MotorDriver::setRightMotor(int speed) {
    // Limitar velocidad entre -255 y 255
    speed = constrain(speed, -MAX_SPEED, MAX_SPEED);
    
    if (speed > 0) {
        // Hacia adelante - usar LPWM (corregido)
        if (speed < MIN_SPEED) speed = MIN_SPEED;
        analogWrite(MOTOR_RIGHT_RPWM, 0);
        analogWrite(MOTOR_RIGHT_LPWM, speed);
    } else if (speed < 0) {
        // Hacia atrás - usar RPWM (corregido)
        int absSpeed = -speed;
        if (absSpeed < MIN_SPEED) absSpeed = MIN_SPEED;
        analogWrite(MOTOR_RIGHT_RPWM, absSpeed);
        analogWrite(MOTOR_RIGHT_LPWM, 0);
    } else {
        // Parado
        analogWrite(MOTOR_RIGHT_RPWM, 0);
        analogWrite(MOTOR_RIGHT_LPWM, 0);
    }
}

void MotorDriver::setBothMotors(int leftSpeed, int rightSpeed) {
    setLeftMotor(leftSpeed);
    setRightMotor(rightSpeed);
}

void MotorDriver::testMotors() {
    Serial.println(F("=== TEST MOTORES ==="));
    
    // Test Motor Izquierdo Adelante
    Serial.println(F("Izq+"));
    analogWrite(MOTOR_LEFT_RPWM, 0);
    analogWrite(MOTOR_LEFT_LPWM, 150);
    delay(1000);
    stop();
    delay(300);
    
    // Test Motor Izquierdo Atrás  
    Serial.println(F("Izq-"));
    analogWrite(MOTOR_LEFT_RPWM, 150);
    analogWrite(MOTOR_LEFT_LPWM, 0);
    delay(1000);
    stop();
    delay(300);
    
    // Test Motor Derecho Adelante
    Serial.println(F("Der+"));
    analogWrite(MOTOR_RIGHT_RPWM, 0);
    analogWrite(MOTOR_RIGHT_LPWM, 150);
    delay(1000);
    stop();
    delay(300);
    
    // Test Motor Derecho Atrás
    Serial.println(F("Der-"));
    analogWrite(MOTOR_RIGHT_RPWM, 150);
    analogWrite(MOTOR_RIGHT_LPWM, 0);
    delay(1000);
    stop();
    delay(300);
    
    Serial.println(F("Test OK"));
}

void MotorDriver::setSpeed(int left, int right) {
    // Función heredada para compatibilidad
    setBothMotors(left, right);
}

void MotorDriver::testRightMotor() {
    Serial.println(F("=== TEST DER ==="));
    
    // Test Pin 6 (LPWM) - Adelante (corregido)
    Serial.println(F("Pin6+ (Adelante)"));
    analogWrite(MOTOR_RIGHT_RPWM, 0);
    analogWrite(MOTOR_RIGHT_LPWM, 200);
    delay(2000);
    analogWrite(6, 0);     
    delay(500);
    
    // Test Pin 5 (RPWM) - Atrás (corregido)
    Serial.println(F("Pin5- (Atras)"));
    analogWrite(MOTOR_RIGHT_RPWM, 200);
    analogWrite(MOTOR_RIGHT_LPWM, 0);
    delay(2000);
    analogWrite(MOTOR_RIGHT_RPWM, 0);
    delay(500);
    
    Serial.println(F("Revisar conexiones si no gira"));
}