# AMR Control System - Arduino Uno

## 📋 Descripción
Sistema completo de control para robot móvil autónomo (AMR) con:
- Encoder E386G5 con triplicador (1200 PPR)
- Control por comandos serie
- Odometría en tiempo real
- Giros automáticos de 90°
- **Bug Algorithm** para navegación autónoma con evasión de obstáculos
- **5 Sensores Infrarrojos** (2 laterales, 3 frontales) para detección de obstáculos

## 🔧 Hardware Requerido
- **Arduino Uno**
- **Encoder E386G5** (400 PPR base × 3 = 1200 PPR)
- **Driver BTS7960** para motores DC (43A máx, más potente que L298N)
- **Ruedas** de 16cm de diámetro
- **Motores DC** con reductora
- **Fuente de alimentación** 12V/24V para motores
- **5 Sensores IR** digitales/analógicos para detección de obstáculos

## 🔌 Conexiones

### Encoders:
```
Encoder Izquierdo:   A → Pin 13    B → Pin 3 (INT1)
Encoder Derecho:     A → Pin 12    B → Pin 2 (INT0)
```

### Motores (BTS7960):
```
Motor Izquierdo:     RPWM → Pin 10 (Atrás)   LPWM → Pin 11 (Adelante)
Motor Derecho:       RPWM → Pin 5 (Atrás)    LPWM → Pin 6 (Adelante)
Enables (REN/LEN):   Alimentación externa (siempre HIGH)

NOTA: Lógica PWM corregida - LPWM = Adelante, RPWM = Atrás
```

### Sensores Infrarrojos:
```
Sensor Lateral Izquierdo:   A0
Sensor Lateral Derecho:      A1
Sensor Frontal Izquierdo:    A2
Sensor Frontal Central:      A3
Sensor Frontal Derecho:      A4

Configuración: VCC → 5V, GND → GND, OUT → Pin Analógico
```

### Alimentación BTS7960:
```
VCC → 5V Arduino
GND → GND Arduino  
B+ → Terminal Positivo Motor
B- → Terminal Negativo Motor
VCC Motor → 12V/24V (según motor)
GND Motor → GND fuente alimentación
```

## 💻 Instalación en Arduino IDE

### Opción 1: Proyecto Completo (Recomendado)
1. Copia todos estos archivos a una carpeta llamada `AMR_Complete`:
   ```
   AMR_Complete/
   ├── AMR_Complete.ino
   ├── MotorDriver.h
   ├── MotorDriver.cpp
   ├── Encoder.h
   ├── Encoder.cpp
   ├── Odometry.h
   ├── Odometry.cpp
   ├── InfraredSensor.h
   ├── InfraredSensor.cpp
   ├── BugAlgorithm.h
   └── BugAlgorithm.cpp
   ```

2. Abre `AMR_Complete.ino` en Arduino IDE
3. Compila y sube al Arduino Uno

### Opción 2: Usar archivos individuales
1. Los archivos ya están organizados correctamente en el directorio actual:
   - `AMR_Complete.ino` (archivo principal)
   - `MotorDriver.h` y `MotorDriver.cpp`
   - `Encoder.h` y `Encoder.cpp`
   - `Odometry.h` y `Odometry.cpp`
   - `InfraredSensor.h` y `InfraredSensor.cpp`
   - `BugAlgorithm.h` y `BugAlgorithm.cpp`

2. Abre directamente `AMR_Complete.ino` en Arduino IDE
3. Todos los archivos `.h` y `.cpp` se cargan automáticamente

## 🎮 Controles

Abre el Monitor Serie a **115200 baud** y usa estos comandos:

### Comandos de Movimiento Manual:
| Comando | Acción |
|---------|--------|
| `W` | Avanzar |
| `S` | Retroceder |
| `A` | Girar 90° izquierda |
| `D` | Girar 90° derecha |
| `X` | Parar |

### Comandos de Navegación Autónoma:
| Comando | Acción |
|---------|--------|
| `B` | Activar/Desactivar Bug Algorithm |
| `I` | Mostrar información de sensores IR |
| `C` | Calibrar sensores IR (valores raw) |
| `G` | Estado del Bug Algorithm |

### Comandos de Sistema:
| Comando | Acción |
|---------|--------|
| `R` | Reset posición a (0,0) |
| `P` | Mostrar posición actual |
| `H` | Mostrar ayuda |
| `T` | Test completo de motores |
| `M` | Diagnóstico motor derecho |

## 🤖 Bug Algorithm - Navegación Autónoma

El **Bug Algorithm** es un algoritmo de navegación reactiva que permite al robot explorar de manera autónoma evitando obstáculos.

### Estados del Bug Algorithm:
1. **IDLE**: Robot inactivo, esperando comandos
2. **GO_TO_GOAL**: Avanza hacia adelante buscando el camino libre
3. **FOLLOW_WALL_LEFT**: Sigue una pared por el lado izquierdo
4. **FOLLOW_WALL_RIGHT**: Sigue una pared por el lado derecho
5. **TURN_AVOID_LEFT**: Gira a la izquierda para evitar obstáculo
6. **TURN_AVOID_RIGHT**: Gira a la derecha para evitar obstáculo
7. **REACHED_GOAL**: Objetivo alcanzado (para expansión futura)

### Comportamiento:
- El robot avanza hasta detectar un obstáculo
- Decide la mejor dirección de evasión según los sensores
- Sigue la pared del obstáculo hasta encontrar camino libre
- Vuelve a avanzar hacia el objetivo

### Configuración de Sensores:
- **Sensores Laterales**: Detectan paredes a los lados (seguimiento de pared)
- **Sensores Frontales**: Detectan obstáculos adelante (3 zonas: izq, centro, der)
- **Umbral de detección**: Ajustable en `InfraredSensor.h` (por defecto 400 para obstáculo cercano)

### Uso:
1. Envía comando `B` para activar el Bug Algorithm
2. El robot comenzará a explorar autónomamente
3. Observa los cambios de estado en el Monitor Serie
4. Envía `B` nuevamente para desactivar
5. Usa `I` para ver lecturas de sensores en tiempo real
6. Usa `C` para calibrar (encontrar valores de umbral adecuados)

## 📍 Sistema de Coordenadas

- **Origen (0,0)**: Punto donde se energiza el Arduino
- **Eje X**: Positivo hacia la derecha
- **Eje Y**: Positivo hacia adelante  
- **Ángulo**: 0° = mirando hacia adelante, +90° = derecha, -90° = izquierda

## 🔧 Configuración

### Ajustar según tu robot:
En `Encoder.h`:
```cpp
#define PULSES_PER_REVOLUTION 1200   // E386G5 × 3
#define WHEEL_DIAMETER_CM 16.0       // Diámetro de rueda
```

En `Odometry.h`:
```cpp
#define WHEEL_BASE_CM 20.0   // Distancia entre ruedas
```

En `InfraredSensor.h`:
```cpp
#define IR_THRESHOLD_NEAR 400   // Umbral para obstáculo cercano
#define IR_THRESHOLD_FAR  200   // Umbral para obstáculo lejano
```
**Nota**: Usa el comando `C` para calibrar y encontrar los valores adecuados para tus sensores.

## 📊 Salida del Monitor Serie

```
=== AMR SYSTEM ===
Enc:1200PPR Ruedas:16cm
W/S:Adelante/Atras
A/D:Izq/Der X:Stop
P:Pos R:Reset T:Test
=== BTS7960 Init ===
MotIzq:10,11 MotDer:5,6
Enables externos. Usa 'T'
Enc OK 1200PPR
Odo OK
LISTO! Pos:(0,0)

> W
Adelante
> P
(5.2,0.0) 0°
> A  
Izq 90
Obj:-90
OK
> T
=== TEST MOTORES ===
Izq+
Izq-
Der+
Der-
Test OK
```

## 🚀 Uso

1. **Conecta** el hardware según el diagrama
2. **Sube** el código al Arduino
3. **Abre** Monitor Serie (115200 baud)
4. **Envía** comandos W, A, S, D para mover
5. **Observa** la posición actualizándose automáticamente

## ⚡ Características

### Control y Navegación:
- ✅ **Odometría precisa** con encoders de alta resolución
- ✅ **Giros automáticos** de exactamente 90°
- ✅ **Seguimiento de posición** en tiempo real
- ✅ **Control por teclado** intuitivo
- ✅ **Reset de posición** para recalibrar
- ✅ **Tolerancia de error** en giros (±8°)
- ✅ **Timeout de seguridad** en movimientos

### Bug Algorithm (Navegación Autónoma):
- ✅ **5 Sensores IR** (2 laterales, 3 frontales)
- ✅ **Detección de obstáculos** en tiempo real
- ✅ **Evasión inteligente** según dirección del obstáculo
- ✅ **Seguimiento de paredes** para rodear obstáculos
- ✅ **Máquina de estados** robusta con 7 estados
- ✅ **Calibración de sensores** para ajuste fino
- ✅ **Modo exploratorio** sin objetivo predefinido
- ✅ **Retroceso automático** si obstáculo en todos los sensores

## 🐛 Troubleshooting

### Problemas de Hardware Básico:

**Encoder no cuenta:**
- Verifica conexiones: Izq (13,3) Der (12,2)
- Comprueba alimentación del encoder
- Solo pines 2 y 3 tienen interrupciones (INT0, INT1)

**Sensores IR no detectan:**
- Verifica conexiones A0-A4
- Comprueba alimentación 5V de sensores
- Usa comando `I` para ver lecturas en tiempo real
- Usa comando `C` para calibrar y encontrar umbrales
- Ajusta `IR_THRESHOLD_NEAR` en `InfraredSensor.h`

**Motores no se mueven:**
- Verifica conexiones BTS7960 (RPWM, LPWM)
- Comprueba alimentación de motores (12V/24V)
- Revisa pines PWM: Izq (10,11) Der (5,6) 
- Confirma que enables externos están alimentados (REN/LEN = HIGH)
- Verifica alimentación externa de enables

**✅ Motores se mueven pero no responden bien:**

### Diagnóstico Automático:
1. **Envía `T`** - Test automático de cada motor (ambos motores)
2. **Envía `M`** - Diagnóstico específico del motor derecho
3. **Observa direcciones:** Izq+/Izq-/Der+/Der- durante tests

### Problemas Comunes:

**🔄 Dirección invertida: ¡YA SOLUCIONADO!**
- **Estado:** Lógica PWM corregida en el código
- **Actual:** LPWM = Adelante, RPWM = Atrás
- **Si persiste:** Verificar conexiones físicas BTS7960

**⚡ Velocidad insuficiente:**
- **Síntoma:** Se mueve muy lento o no supera fricción
- **Solución:** Aumentar velocidades en `MotorDriver.h`:
  ```cpp
  #define DEFAULT_SPEED 200    // Subir de 180
  #define MIN_SPEED 100        // Subir de 80
  ```

**🌀 Giros incorrectos:**
- **Síntoma:** Gira en dirección opuesta
- **Solución:** Verificar lógica `turnLeft()` y `turnRight()`

**⚠️ Respuesta errática:**
- **Causas:** Voltaje bajo, conexiones flojas, PWM insuficiente
- **Solución:** Verificar alimentación 12V/24V estable

**Posición incorrecta:**
- Ajusta `WHEEL_DIAMETER_CM` y `WHEEL_BASE_CM`
- Calibra `PULSES_PER_REVOLUTION`
- Usa comando `R` para resetear

### Problemas del Bug Algorithm:

**Bug Algorithm no se activa:**
- Verifica que los sensores IR estén conectados
- Usa comando `I` para verificar lectura de sensores
- Asegúrate de no estar en modo de giro manual (espera que termine)

**Robot no evita obstáculos:**
- Calibra sensores con comando `C`
- Ajusta umbrales `IR_THRESHOLD_NEAR` y `IR_THRESHOLD_FAR`
- Verifica que los sensores estén orientados correctamente
- Comprueba que detectan: acerca objeto y usa comando `I`

**Robot gira constantemente:**
- Sensores pueden estar muy sensibles (valores altos sin obstáculo)
- Aumenta `IR_THRESHOLD_NEAR` en `InfraredSensor.h`
- Verifica calibración con comando `C`

**Robot no sigue paredes:**
- Sensores laterales pueden no detectar a la distancia correcta
- Ajusta posición física de sensores laterales
- Modifica `WALL_FOLLOW_TIME` en `BugAlgorithm.h` si necesario

## 📝 Notas Importantes

### Memoria y Rendimiento:
- **Memoria optimizada**: Código reducido para Arduino Uno (<90% RAM)
- **Interrupciones**: Pines 2 y 3 para máxima precisión de encoders
- **Odometría**: Se actualiza cada 50ms automáticamente  
- **Giros**: Timeout de 4 segundos por seguridad
- **Strings optimizados**: Uso de F() macro para ahorrar RAM

### Bug Algorithm:
- **Sensores IR**: Lectura cada 50ms durante navegación autónoma
- **Prioridad**: Bug Algorithm desactiva control manual cuando está activo
- **Umbrales ajustables**: Modificar en `InfraredSensor.h` según tipo de sensor
- **Tipos de sensores compatibles**: IR digitales o analógicos (0-1023)
- **Retroceso automático**: Si todos los sensores frontales detectan obstáculo
- **Modo exploratorio**: Avanza y evita obstáculos sin objetivo predefinido

### General:
- **Dirección corregida**: LPWM=Adelante, RPWM=Atrás (funcionando)
- **Posición manual**: Usar comando `P` (no automático)
- **Diagnóstico**: Comandos `T` y `M` para troubleshooting
- **Calibración IR**: Comando `C` muestra valores raw para ajustar umbrales

## 🎯 Estado Actual: ✅ FUNCIONAL

### Core System:
- ✅ Compilación exitosa (memoria optimizada)
- ✅ Ambos motores responden correctamente
- ✅ Direcciones corregidas (adelante/atrás)
- ✅ Encoders contando correctamente  
- ✅ Odometría en tiempo real
- ✅ Comandos de diagnóstico operativos
- ✅ Sistema completo integrado y probado

### Bug Algorithm (NUEVO):
- ✅ 5 Sensores IR integrados (A0-A4)
- ✅ Máquina de estados implementada
- ✅ Detección y evasión de obstáculos
- ✅ Seguimiento de paredes (izquierda/derecha)
- ✅ Calibración de sensores
- ✅ Comandos de diagnóstico (I, C, G, B)
- ✅ Integración completa con odometría y motores

**Última actualización**: Octubre 2025 - Bug Algorithm implementado