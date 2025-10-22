# Bug Algorithm - Guía de Implementación

## 🎯 ¿Qué es el Bug Algorithm?

El **Bug Algorithm** es un algoritmo de navegación reactiva simple pero efectivo que permite a un robot móvil navegar hacia un objetivo evitando obstáculos usando solo información local de sensores.

## 📊 Arquitectura del Sistema

```
┌─────────────────────────────────────────────────────────────┐
│                    AMR_Complete.ino                          │
│                   (Programa Principal)                       │
└─────────────────────────────────────────────────────────────┘
                            │
        ┌───────────────────┼───────────────────┐
        │                   │                   │
        ▼                   ▼                   ▼
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│ MotorDriver │    │   Encoder   │    │  Odometry   │
│   (BTS7960) │    │  (E386G5)   │    │ (Posición)  │
└─────────────┘    └─────────────┘    └─────────────┘
        │                   │                   │
        └───────────────────┼───────────────────┘
                            │
        ┌───────────────────┴───────────────────┐
        │                                       │
        ▼                                       ▼
┌─────────────────┐                  ┌──────────────────┐
│ InfraredSensor  │ ←────────────────│  BugAlgorithm   │
│ (5 sensores IR) │                  │ (Máq. Estados)  │
└─────────────────┘                  └──────────────────┘
```

## 🔢 Configuración de Sensores

### Distribución Física:
```
        Frontal Izq (A2)   Frontal Centro (A3)   Frontal Der (A4)
                  ↓                ↓                    ↓
              ┌───┴────────────────┴────────────────────┴───┐
              │                                              │
Lateral  →  A0│                   ROBOT                     │A1  ← Lateral
Izquierdo     │                                              │    Derecho
              │                  (Hacia adelante)            │
              └──────────────────────────────────────────────┘
```

### Tabla de Sensores:

| Sensor              | Pin  | Función                          |
|---------------------|------|----------------------------------|
| Lateral Izquierdo   | A0   | Detecta paredes/obstáculos izq   |
| Lateral Derecho     | A1   | Detecta paredes/obstáculos der   |
| Frontal Izquierdo   | A2   | Detecta obstáculos adelante-izq  |
| Frontal Central     | A3   | Detecta obstáculos adelante      |
| Frontal Derecho     | A4   | Detecta obstáculos adelante-der  |

## 🔄 Máquina de Estados

### Estados Disponibles:

```
┌──────────┐
│   IDLE   │  ← Estado inicial (robot parado)
└────┬─────┘
     │ Comando 'B'
     ▼
┌────────────┐     Obstáculo frontal      ┌─────────────────┐
│ GO_TO_GOAL │ ─────────────────────────→ │ TURN_AVOID_LEFT │
│  (Avanzar) │                            │   o RIGHT       │
└────┬───────┘                            └────────┬────────┘
     │                                             │
     │ Camino libre                                │ Giro completo
     │                                             │
     │                                             ▼
     │                            ┌─────────────────────────┐
     └────────────────────────────│ FOLLOW_WALL_LEFT/RIGHT  │
                                  │  (Seguir pared)         │
                                  └─────────────────────────┘
```

### Descripción de Estados:

1. **IDLE**: Robot inactivo, esperando activación
2. **GO_TO_GOAL**: Avanza hacia adelante hasta encontrar obstáculo
3. **TURN_AVOID_LEFT**: Gira a la izquierda para evitar obstáculo
4. **TURN_AVOID_RIGHT**: Gira a la derecha para evitar obstáculo
5. **FOLLOW_WALL_LEFT**: Sigue una pared por el lado izquierdo
6. **FOLLOW_WALL_RIGHT**: Sigue una pared por el lado derecho
7. **REACHED_GOAL**: Objetivo alcanzado (para expansión futura)

## 🎮 Comandos de Control

### Comandos del Bug Algorithm:

| Comando | Función                                    | Ejemplo de uso                |
|---------|-------------------------------------------|-------------------------------|
| `B`     | Activar/Desactivar Bug Algorithm          | Iniciar navegación autónoma   |
| `I`     | Mostrar información de sensores IR        | Ver lecturas en tiempo real   |
| `C`     | Calibrar sensores (mostrar valores raw)   | Ajustar umbrales de detección |
| `G`     | Mostrar estado del Bug Algorithm          | Ver estado actual y objetivo  |
| `X`     | Detener (funciona también con Bug activo) | Parada de emergencia          |

## 🧪 Calibración de Sensores

### Proceso de Calibración:

1. **Conecta sensores IR** a los pines A0-A4
2. **Abre Monitor Serie** (115200 baud)
3. **Envía comando `C`** para ver valores raw
4. **Acerca un objeto** a cada sensor y observa los valores
5. **Anota el valor de detección** (típicamente >200 cuando detecta)
6. **Ajusta umbrales** en `InfraredSensor.h`:

```cpp
// Valores típicos para sensores IR analógicos:
#define IR_THRESHOLD_NEAR 400   // Obstáculo muy cerca
#define IR_THRESHOLD_FAR  200   // Obstáculo lejos

// Ajustar según tu sensor:
// - Sensores IR digitales: usar 512 como umbral
// - Sensores IR analógicos: calibrar con comando 'C'
// - Valores bajos sin obstáculo: aumentar umbral
// - Valores altos sin obstáculo: disminuir umbral
```

### Ejemplo de Salida de Calibración:

```
> C
=== Calibracion IR ===
Valores actuales (0-1023):
LS:45 RS:52 FL:38 FC:41 FR:49
Umbral: 400

(Acercas un objeto al sensor frontal central)

> C
=== Calibracion IR ===
Valores actuales (0-1023):
LS:47 RS:50 FL:43 FC:687 FR:46  ← FC detecta obstáculo!
Umbral: 400
```

## 📐 Lógica de Decisión

### Algoritmo Simplificado:

```
WHILE bug_algorithm_activo:
    leer_sensores_IR()
    
    IF estado == GO_TO_GOAL:
        IF obstáculo_frontal:
            decidir_dirección_evitación()
            cambiar_a_TURN_AVOID_LEFT/RIGHT
        ELSE:
            avanzar()
    
    IF estado == TURN_AVOID_LEFT/RIGHT:
        girar()
        IF giro_completo AND camino_libre:
            cambiar_a_GO_TO_GOAL
        ELSE IF detecta_pared_lateral:
            cambiar_a_FOLLOW_WALL
    
    IF estado == FOLLOW_WALL:
        IF obstáculo_frontal:
            cambiar_a_TURN_AVOID
        ELSE IF pierde_pared:
            buscar_pared()
        ELSE:
            seguir_adelante()
        
        IF tiempo_siguiendo_pared > TIMEOUT AND camino_libre:
            cambiar_a_GO_TO_GOAL
END WHILE
```

## 🔧 Parámetros Ajustables

En `BugAlgorithm.h`:

```cpp
#define WALL_FOLLOW_TIME 2000    // Tiempo (ms) siguiendo pared antes de revisar
#define TURN_AVOID_TIME 800      // Tiempo (ms) de giro para evitar obstáculo
#define BACKUP_TIME 500          // Tiempo (ms) de retroceso si obstáculo frontal
```

En `InfraredSensor.h`:

```cpp
#define IR_THRESHOLD_NEAR 400    // Valor para obstáculo cercano
#define IR_THRESHOLD_FAR  200    // Valor para obstáculo lejano
```

En `MotorDriver.h`:

```cpp
#define DEFAULT_SPEED 180        // Velocidad para avanzar
#define TURN_SPEED 150           // Velocidad para giros
```

## 🎬 Escenarios de Uso

### Escenario 1: Pasillo Simple
```
Inicio → → → → → → → → → → Meta
         (avanza recto en GO_TO_GOAL)
```

### Escenario 2: Obstáculo Central
```
Inicio → → → ╔═══╗
             ║ ⚠ ║ (detecta obstáculo)
             ╚═══╝
                ↓ (TURN_AVOID_RIGHT)
                → → → → Meta
```

### Escenario 3: Seguir Pared
```
Inicio → → ╔═══════════╗
           ║           ║
           ║ Obstáculo ║ (FOLLOW_WALL_RIGHT)
           ║           ║
           ╚═══════════╝
              ↓ (vuelve a GO_TO_GOAL)
              → → Meta
```

## 🐛 Troubleshooting

### Problema: Robot gira constantemente sin obstáculo
**Causa**: Sensores muy sensibles o umbrales incorrectos  
**Solución**: 
1. Ejecutar comando `C` para ver valores sin obstáculo
2. Aumentar `IR_THRESHOLD_NEAR` si valores son altos sin obstáculo
3. Verificar conexiones de sensores

### Problema: Robot no detecta obstáculos
**Causa**: Umbrales muy altos o sensores no funcionan  
**Solución**:
1. Ejecutar comando `I` con objeto cerca de sensores
2. Verificar que valores aumentan con obstáculo presente
3. Disminuir `IR_THRESHOLD_NEAR` si es necesario
4. Verificar alimentación 5V de sensores

### Problema: Robot se atasca en esquinas
**Causa**: Lógica de seguimiento de pared necesita ajuste  
**Solución**:
1. Reducir `WALL_FOLLOW_TIME` para cambiar de estrategia más rápido
2. Aumentar tiempo de retroceso `BACKUP_TIME`
3. Verificar que sensores laterales funcionan correctamente

### Problema: Bug Algorithm no responde a comandos
**Causa**: Bug Algorithm activo bloquea control manual  
**Solución**:
1. Presionar `X` para parada de emergencia
2. Presionar `B` para desactivar Bug Algorithm
3. El sistema debe volver al control manual

## 📝 Notas de Implementación

### Diferencias con Bug1/Bug2 Clásico:
- **Sin objetivo definido**: Versión simplificada en modo exploratorio
- **Sin línea M**: No calcula línea directa start-goal
- **Reactivo puro**: Decisiones basadas solo en sensores actuales
- **Sin memoria de trayectoria**: No recuerda puntos visitados

### Posibles Mejoras Futuras:
- Implementar Bug2 con línea M y objetivo específico
- Agregar memoria de puntos visitados (evitar loops)
- Integrar con odometría para retornar a home
- Añadir sensores ultrasónicos para mejor rango
- Path planning con waypoints intermedios

## ✅ Checklist de Verificación

Antes de activar Bug Algorithm:

- [ ] 5 sensores IR conectados (A0-A4)
- [ ] Sensores alimentados con 5V
- [ ] Calibración realizada (comando `C`)
- [ ] Umbrales ajustados en código si necesario
- [ ] Motores funcionando correctamente (comando `T`)
- [ ] Espacio libre para navegación (min 2x2 metros)
- [ ] Monitor Serie abierto para observar estados
- [ ] Comando `X` listo para parada de emergencia

## 🎓 Recursos Adicionales

- **Paper original Bug Algorithm**: Lumelsky & Stepanov (1987)
- **Documentación Arduino**: Lectura de pines analógicos
- **Sensores IR recomendados**: Sharp GP2Y0A21YK, TCRT5000
- **README completo**: Ver `arduino/src/AMR_Complete/README.md`

---

**Última actualización**: Octubre 2025  
**Versión**: 1.0  
**Autor**: AMR Project Team
