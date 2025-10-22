#include "BugAlgorithm.h"

BugAlgorithm::BugAlgorithm(InfraredSensor* sensors, MotorDriver* motors, Odometry* odometry) {
    this->sensors = sensors;
    this->motors = motors;
    this->odometry = odometry;
    
    currentState = BUG_IDLE;
    previousState = BUG_IDLE;
    isActive = false;
    stateStartTime = 0;
    lastSensorRead = 0;
    
    goalX = 0;
    goalY = 0;
    hasGoal = false;
}

void BugAlgorithm::init() {
    currentState = BUG_IDLE;
    isActive = false;
    Serial.println(F("=== Bug Algorithm Init ==="));
    Serial.println(F("Usar 'B' para activar/desactivar"));
}

void BugAlgorithm::start() {
    if (!isActive) {
        isActive = true;
        setState(BUG_GO_TO_GOAL);
        Serial.println(F("Bug Algorithm ACTIVADO"));
        Serial.println(F("Modo: Exploracion autonoma"));
    }
}

void BugAlgorithm::stop() {
    if (isActive) {
        isActive = false;
        motors->stop();
        setState(BUG_IDLE);
        Serial.println(F("Bug Algorithm DETENIDO"));
    }
}

void BugAlgorithm::update() {
    if (!isActive) return;
    
    // Leer sensores cada 50ms
    if (millis() - lastSensorRead >= 50) {
        sensors->readAllSensors();
        lastSensorRead = millis();
    }
    
    // Ejecutar lógica según estado actual
    switch (currentState) {
        case BUG_GO_TO_GOAL:
            handleGoToGoal();
            break;
            
        case BUG_FOLLOW_WALL_LEFT:
            handleFollowWallLeft();
            break;
            
        case BUG_FOLLOW_WALL_RIGHT:
            handleFollowWallRight();
            break;
            
        case BUG_TURN_AVOID_LEFT:
            handleTurnAvoidLeft();
            break;
            
        case BUG_TURN_AVOID_RIGHT:
            handleTurnAvoidRight();
            break;
            
        case BUG_REACHED_GOAL:
            motors->stop();
            break;
            
        default:
            motors->stop();
            break;
    }
    
    // Verificar transiciones de estado
    checkTransitions();
}

void BugAlgorithm::setState(BugState newState) {
    if (newState != currentState) {
        previousState = currentState;
        currentState = newState;
        stateStartTime = millis();
        
        // Debug: mostrar cambio de estado
        Serial.print(F("Estado: "));
        Serial.println(getStateName());
    }
}

void BugAlgorithm::handleGoToGoal() {
    // Comportamiento: avanzar hacia adelante
    // Si detecta obstáculo frontal, cambiar a modo evitación
    
    if (sensors->isAnyFrontBlocked()) {
        // Obstáculo frontal detectado
        motors->stop();
        
        // Decidir dirección de evitación según sensores laterales
        if (sensors->isFrontLeftBlocked() && !sensors->isFrontRightBlocked()) {
            // Obstáculo a la izquierda, girar derecha
            setState(BUG_TURN_AVOID_RIGHT);
        } else if (!sensors->isFrontLeftBlocked() && sensors->isFrontRightBlocked()) {
            // Obstáculo a la derecha, girar izquierda
            setState(BUG_TURN_AVOID_LEFT);
        } else if (sensors->isFrontCenterBlocked()) {
            // Obstáculo central, girar según sensor lateral
            if (!sensors->isRightSideBlocked()) {
                setState(BUG_TURN_AVOID_RIGHT);
            } else {
                setState(BUG_TURN_AVOID_LEFT);
            }
        } else {
            // Por defecto, girar izquierda
            setState(BUG_TURN_AVOID_LEFT);
        }
    } else {
        // Camino libre, avanzar
        motors->moveForward(150);
    }
}

void BugAlgorithm::handleFollowWallLeft() {
    // Seguir pared manteniendo sensor lateral izquierdo cerca
    
    if (sensors->isFrontCenterBlocked()) {
        // Obstáculo frontal, girar derecha
        setState(BUG_TURN_AVOID_RIGHT);
    } else if (sensors->isLeftSideBlocked()) {
        // Pared a la izquierda, seguir adelante
        motors->moveForward(130);
    } else {
        // Perdió la pared, volver a buscarla
        motors->turnLeft(100);
    }
    
    // Después de un tiempo, intentar volver a ir al objetivo
    if (millis() - stateStartTime > WALL_FOLLOW_TIME) {
        if (!sensors->isAnyFrontBlocked()) {
            setState(BUG_GO_TO_GOAL);
        }
    }
}

void BugAlgorithm::handleFollowWallRight() {
    // Seguir pared manteniendo sensor lateral derecho cerca
    
    if (sensors->isFrontCenterBlocked()) {
        // Obstáculo frontal, girar izquierda
        setState(BUG_TURN_AVOID_LEFT);
    } else if (sensors->isRightSideBlocked()) {
        // Pared a la derecha, seguir adelante
        motors->moveForward(130);
    } else {
        // Perdió la pared, volver a buscarla
        motors->turnRight(100);
    }
    
    // Después de un tiempo, intentar volver a ir al objetivo
    if (millis() - stateStartTime > WALL_FOLLOW_TIME) {
        if (!sensors->isAnyFrontBlocked()) {
            setState(BUG_GO_TO_GOAL);
        }
    }
}

void BugAlgorithm::handleTurnAvoidLeft() {
    // Girar a la izquierda para evitar obstáculo
    motors->turnLeft(120);
    
    // Después del tiempo de giro, revisar si puede avanzar
    if (millis() - stateStartTime > TURN_AVOID_TIME) {
        if (!sensors->isAnyFrontBlocked()) {
            setState(BUG_GO_TO_GOAL);
        } else if (sensors->isLeftSideBlocked()) {
            setState(BUG_FOLLOW_WALL_LEFT);
        } else {
            // Continuar girando
            stateStartTime = millis();
        }
    }
}

void BugAlgorithm::handleTurnAvoidRight() {
    // Girar a la derecha para evitar obstáculo
    motors->turnRight(120);
    
    // Después del tiempo de giro, revisar si puede avanzar
    if (millis() - stateStartTime > TURN_AVOID_TIME) {
        if (!sensors->isAnyFrontBlocked()) {
            setState(BUG_GO_TO_GOAL);
        } else if (sensors->isRightSideBlocked()) {
            setState(BUG_FOLLOW_WALL_RIGHT);
        } else {
            // Continuar girando
            stateStartTime = millis();
        }
    }
}

void BugAlgorithm::checkTransitions() {
    // Verificaciones de seguridad y transiciones adicionales
    
    // Si todos los sensores frontales detectan obstáculo muy cerca, retroceder
    if (currentState == BUG_GO_TO_GOAL || 
        currentState == BUG_FOLLOW_WALL_LEFT || 
        currentState == BUG_FOLLOW_WALL_RIGHT) {
        
        if (sensors->isFrontLeftBlocked() && 
            sensors->isFrontCenterBlocked() && 
            sensors->isFrontRightBlocked()) {
            // Obstáculo en todos los sensores frontales
            motors->stop();
            delay(200);
            motors->moveBackward(120);
            delay(BACKUP_TIME);
            motors->stop();
            
            // Girar para buscar camino libre
            if (!sensors->isRightSideBlocked()) {
                setState(BUG_TURN_AVOID_RIGHT);
            } else {
                setState(BUG_TURN_AVOID_LEFT);
            }
        }
    }
}

void BugAlgorithm::setGoal(float x, float y) {
    goalX = x;
    goalY = y;
    hasGoal = true;
    Serial.print(F("Objetivo: ("));
    Serial.print(x);
    Serial.print(F(", "));
    Serial.print(y);
    Serial.println(F(")"));
}

void BugAlgorithm::clearGoal() {
    hasGoal = false;
    Serial.println(F("Objetivo eliminado"));
}

const char* BugAlgorithm::getStateName() {
    switch (currentState) {
        case BUG_IDLE:
            return "Inactivo";
        case BUG_GO_TO_GOAL:
            return "Avanzar";
        case BUG_FOLLOW_WALL_LEFT:
            return "Seguir pared izq";
        case BUG_FOLLOW_WALL_RIGHT:
            return "Seguir pared der";
        case BUG_TURN_AVOID_LEFT:
            return "Evitar izq";
        case BUG_TURN_AVOID_RIGHT:
            return "Evitar der";
        case BUG_REACHED_GOAL:
            return "Objetivo alcanzado";
        default:
            return "Desconocido";
    }
}

void BugAlgorithm::printStatus() {
    Serial.println(F("=== Bug Algorithm Status ==="));
    Serial.print(F("Activo: "));
    Serial.println(isActive ? F("SI") : F("NO"));
    Serial.print(F("Estado: "));
    Serial.println(getStateName());
    
    if (hasGoal) {
        Serial.print(F("Objetivo: ("));
        Serial.print(goalX);
        Serial.print(F(", "));
        Serial.print(goalY);
        Serial.println(F(")"));
    } else {
        Serial.println(F("Sin objetivo definido"));
    }
    
    Serial.println();
}
