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

// --------------------
// PID velocity control
// --------------------
void MotorDriver::enableVelocityControl(bool en) {
    velocityControlEnabled = en;
    if (!en) {
        // reset integrators and stop applying setpoints
        leftIntegral = rightIntegral = 0.0f;
        leftPrevError = rightPrevError = 0.0f;
        appliedLeftPps = appliedRightPps = 0.0f;
        // ensure motors are stopped or left under direct control
    } else {
        lastPIDMillis = millis();
    }
    Serial.print(F("VelocityControl "));
    Serial.println(en ? F("ENABLED") : F("DISABLED"));
}

bool MotorDriver::isVelocityControlEnabled() {
    return velocityControlEnabled;
}

void MotorDriver::setTargetPulsesPerSecond(float leftPps, float rightPps) {
    targetLeftPps = leftPps;
    targetRightPps = rightPps;
    // when first setting, don't instantly jump applied setpoint if off
    if (!velocityControlEnabled) {
        appliedLeftPps = targetLeftPps;
        appliedRightPps = targetRightPps;
    }
}

void MotorDriver::setPIDGains(float kp, float ki, float kd) {
    Kp = kp; Ki = ki; Kd = kd;
}

void MotorDriver::setPIDInterval(unsigned int ms) {
    if (ms < 5) ms = 5;
    pidIntervalMs = ms;
}

void MotorDriver::setRampTime(unsigned long ms) {
    rampTimeMs = ms;
}

// updateVelocityControl: called from main loop with encoder delta counts and elapsed ms
void MotorDriver::updateVelocityControl(long leftDeltaPulses, long rightDeltaPulses, unsigned long dtMs) {
    if (!velocityControlEnabled) return;
    if (dtMs == 0) return;

    // Only run at configured interval
    unsigned long now = millis();
    if (now - lastPIDMillis < pidIntervalMs) return;
    unsigned long elapsed = now - lastPIDMillis;
    lastPIDMillis = now;

    float dt = (float)elapsed / 1000.0f; // seconds

    // measured pulses per second
    float measLeftPps = (float)leftDeltaPulses / dt;
    float measRightPps = (float)rightDeltaPulses / dt;

    // Soft-start ramp applied setpoints towards target
    if (rampTimeMs > 0) {
        float maxDeltaLeft = (abs(targetLeftPps) / (float)rampTimeMs) * (float)elapsed; // pps per this interval
        float maxDeltaRight = (abs(targetRightPps) / (float)rampTimeMs) * (float)elapsed;

        // adjust appliedLeftPps towards targetLeftPps
        if (appliedLeftPps < targetLeftPps) {
            appliedLeftPps += maxDeltaLeft;
            if (appliedLeftPps > targetLeftPps) appliedLeftPps = targetLeftPps;
        } else if (appliedLeftPps > targetLeftPps) {
            appliedLeftPps -= maxDeltaLeft;
            if (appliedLeftPps < targetLeftPps) appliedLeftPps = targetLeftPps;
        }

        if (appliedRightPps < targetRightPps) {
            appliedRightPps += maxDeltaRight;
            if (appliedRightPps > targetRightPps) appliedRightPps = targetRightPps;
        } else if (appliedRightPps > targetRightPps) {
            appliedRightPps -= maxDeltaRight;
            if (appliedRightPps < targetRightPps) appliedRightPps = targetRightPps;
        }
    } else {
        appliedLeftPps = targetLeftPps;
        appliedRightPps = targetRightPps;
    }

    // PID for left
    float errL = appliedLeftPps - measLeftPps;
    leftIntegral += errL * dt;
    // clamp integral
    if (leftIntegral > integralClamp) leftIntegral = integralClamp;
    if (leftIntegral < -integralClamp) leftIntegral = -integralClamp;
    float derivL = (errL - leftPrevError) / dt;
    leftPrevError = errL;
    float outL = Kp * errL + Ki * leftIntegral + Kd * derivL;

    // PID for right
    float errR = appliedRightPps - measRightPps;
    rightIntegral += errR * dt;
    if (rightIntegral > integralClamp) rightIntegral = integralClamp;
    if (rightIntegral < -integralClamp) rightIntegral = -integralClamp;
    float derivR = (errR - rightPrevError) / dt;
    rightPrevError = errR;
    float outR = Kp * errR + Ki * rightIntegral + Kd * derivR;

    // Map PID output (which is in arbitrary scale) to PWM command.
    // We assume gains are tuned so that out ~ PWM range (-255..255).
    int pwmL = (int)round(outL);
    int pwmR = (int)round(outR);

    // Constrain PWM
    pwmL = constrain(pwmL, -MAX_SPEED, MAX_SPEED);
    pwmR = constrain(pwmR, -MAX_SPEED, MAX_SPEED);

    // Apply to motors
    setBothMotors(pwmL, pwmR);

    // Optional debug print (comment/uncomment for tuning)
    // Serial.print(F("PID dt:")); Serial.print(dt); Serial.print(F("ms mL:")); Serial.print(measLeftPps); Serial.print(F(" tL:")); Serial.print(appliedLeftPps);
    // Serial.print(F(" pL:")); Serial.print(pwmL); Serial.print(F(" mR:")); Serial.print(measRightPps); Serial.print(F(" tR:")); Serial.print(appliedRightPps); Serial.print(F(" pR:")); Serial.println(pwmR);
}