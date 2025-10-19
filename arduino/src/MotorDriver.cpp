#include "MotorDriver.h"

void MotorDriver::init() {
    // Configurar pines BTS7960 como salida
    // Motor Izquierdo
    pinMode(MOTOR_LEFT_RPWM, OUTPUT);  // PWM Adelante
    pinMode(MOTOR_LEFT_LPWM, OUTPUT);  // PWM Atrás
    pinMode(MOTOR_LEFT_REN, OUTPUT);   // Enable Adelante
    pinMode(MOTOR_LEFT_LEN, OUTPUT);   // Enable Atrás
    
    // Motor Derecho
    pinMode(MOTOR_RIGHT_RPWM, OUTPUT); // PWM Adelante
    pinMode(MOTOR_RIGHT_LPWM, OUTPUT); // PWM Atrás
    pinMode(MOTOR_RIGHT_REN, OUTPUT);  // Enable Adelante
    pinMode(MOTOR_RIGHT_LEN, OUTPUT);  // Enable Atrás
    
    // Habilitar todos los canales del BTS7960
    digitalWrite(MOTOR_LEFT_REN, HIGH);
    digitalWrite(MOTOR_LEFT_LEN, HIGH);
    digitalWrite(MOTOR_RIGHT_REN, HIGH);
    digitalWrite(MOTOR_RIGHT_LEN, HIGH);
    
    // Inicializar en estado parado
    stop();
    currentSpeed = 0;
    
    Serial.println("=== BTS7960 Motor Driver Inicializado ===");
    Serial.println("- Motor Izq: RPWM=5, LPWM=6, REN=7, LEN=8");
    Serial.println("- Motor Der: RPWM=9, LPWM=10, REN=11, LEN=12");
    Serial.println("- Todos los enables activados");
}

void MotorDriver::moveForward(int speed) {
    // Limitar velocidad mínima y máxima
    speed = constrain(speed, MIN_SPEED, MAX_SPEED);
    currentSpeed = speed;
    
    // Motor izquierdo hacia adelante (solo RPWM activo)
    analogWrite(MOTOR_LEFT_RPWM, speed);   // PWM adelante
    analogWrite(MOTOR_LEFT_LPWM, 0);       // PWM atrás = 0
    
    // Motor derecho hacia adelante (solo RPWM activo)
    analogWrite(MOTOR_RIGHT_RPWM, speed);  // PWM adelante
    analogWrite(MOTOR_RIGHT_LPWM, 0);      // PWM atrás = 0
}

void MotorDriver::moveBackward(int speed) {
    // Limitar velocidad mínima y máxima
    speed = constrain(speed, MIN_SPEED, MAX_SPEED);
    currentSpeed = speed;
    
    // Motor izquierdo hacia atrás (solo LPWM activo)
    analogWrite(MOTOR_LEFT_RPWM, 0);       // PWM adelante = 0
    analogWrite(MOTOR_LEFT_LPWM, speed);   // PWM atrás
    
    // Motor derecho hacia atrás (solo LPWM activo)
    analogWrite(MOTOR_RIGHT_RPWM, 0);      // PWM adelante = 0
    analogWrite(MOTOR_RIGHT_LPWM, speed);  // PWM atrás
}

void MotorDriver::turnLeft(int speed) {
    // Limitar velocidad
    speed = constrain(speed, MIN_SPEED, MAX_SPEED);
    currentSpeed = speed;
    
    // Motor izquierdo hacia atrás (giro en su lugar)
    analogWrite(MOTOR_LEFT_RPWM, 0);       // PWM adelante = 0
    analogWrite(MOTOR_LEFT_LPWM, speed);   // PWM atrás
    
    // Motor derecho hacia adelante
    analogWrite(MOTOR_RIGHT_RPWM, speed);  // PWM adelante
    analogWrite(MOTOR_RIGHT_LPWM, 0);      // PWM atrás = 0
}

void MotorDriver::turnRight(int speed) {
    // Limitar velocidad
    speed = constrain(speed, MIN_SPEED, MAX_SPEED);
    currentSpeed = speed;
    
    // Motor izquierdo hacia adelante
    analogWrite(MOTOR_LEFT_RPWM, speed);   // PWM adelante
    analogWrite(MOTOR_LEFT_LPWM, 0);       // PWM atrás = 0
    
    // Motor derecho hacia atrás
    analogWrite(MOTOR_RIGHT_RPWM, 0);      // PWM adelante = 0
    analogWrite(MOTOR_RIGHT_LPWM, speed);  // PWM atrás
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
        // Hacia adelante - usar solo RPWM
        if (speed < MIN_SPEED) speed = MIN_SPEED; // Velocidad mínima
        analogWrite(MOTOR_LEFT_RPWM, speed);
        analogWrite(MOTOR_LEFT_LPWM, 0);
    } else if (speed < 0) {
        // Hacia atrás - usar solo LPWM
        int absSpeed = -speed;
        if (absSpeed < MIN_SPEED) absSpeed = MIN_SPEED;
        analogWrite(MOTOR_LEFT_RPWM, 0);
        analogWrite(MOTOR_LEFT_LPWM, absSpeed);
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
        // Hacia adelante - usar solo RPWM
        if (speed < MIN_SPEED) speed = MIN_SPEED;
        analogWrite(MOTOR_RIGHT_RPWM, speed);
        analogWrite(MOTOR_RIGHT_LPWM, 0);
    } else if (speed < 0) {
        // Hacia atrás - usar solo LPWM
        int absSpeed = -speed;
        if (absSpeed < MIN_SPEED) absSpeed = MIN_SPEED;
        analogWrite(MOTOR_RIGHT_RPWM, 0);
        analogWrite(MOTOR_RIGHT_LPWM, absSpeed);
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

void MotorDriver::setSpeed(int left, int right) {
    // Función heredada para compatibilidad
    setBothMotors(left, right);
}