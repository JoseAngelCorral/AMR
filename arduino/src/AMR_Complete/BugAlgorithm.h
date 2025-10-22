#pragma once

#ifndef BUG_ALGORITHM_H
#define BUG_ALGORITHM_H

#include <Arduino.h>
#include "InfraredSensor.h"
#include "MotorDriver.h"
#include "Odometry.h"

// Estados del Bug Algorithm
enum BugState {
    BUG_IDLE,              // Inactivo
    BUG_GO_TO_GOAL,        // Moverse hacia el objetivo
    BUG_FOLLOW_WALL_LEFT,  // Seguir pared por la izquierda
    BUG_FOLLOW_WALL_RIGHT, // Seguir pared por la derecha
    BUG_TURN_AVOID_LEFT,   // Girar para evitar obstáculo (izquierda)
    BUG_TURN_AVOID_RIGHT,  // Girar para evitar obstáculo (derecha)
    BUG_REACHED_GOAL       // Objetivo alcanzado
};

// Distancia de seguimiento de pared (simulada con temporizadores)
#define WALL_FOLLOW_TIME 2000      // Tiempo siguiendo pared antes de revisar
#define TURN_AVOID_TIME 800        // Tiempo de giro para evitar obstáculo
#define BACKUP_TIME 500            // Tiempo de retroceso si obstáculo frontal

class BugAlgorithm {
private:
    InfraredSensor* sensors;
    MotorDriver* motors;
    Odometry* odometry;
    
    BugState currentState;
    BugState previousState;
    
    bool isActive;
    unsigned long stateStartTime;
    unsigned long lastSensorRead;
    
    // Variables de objetivo (simple: seguir adelante hasta obstáculo)
    float goalX;
    float goalY;
    bool hasGoal;
    
public:
    BugAlgorithm(InfraredSensor* sensors, MotorDriver* motors, Odometry* odometry);
    
    void init();
    void start();
    void stop();
    void update();  // Llamar en el loop principal
    
    // Control de objetivo
    void setGoal(float x, float y);
    void clearGoal();
    
    // Estado actual
    bool isRunning() { return isActive; }
    BugState getState() { return currentState; }
    const char* getStateName();
    
    // Diagnóstico
    void printStatus();
    
private:
    void setState(BugState newState);
    void handleGoToGoal();
    void handleFollowWallLeft();
    void handleFollowWallRight();
    void handleTurnAvoidLeft();
    void handleTurnAvoidRight();
    void checkTransitions();
};

#endif // BUG_ALGORITHM_H
