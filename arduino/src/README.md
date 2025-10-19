# AMR Control System - Arduino Uno

## 📋 Descripción
Sistema completo de control para robot móvil autónomo (AMR) con:
- Encoder E386G5 con triplicador (1200 PPR)
- Control por comandos serie
- Odometría en tiempo real
- Giros automáticos de 90°

## 🔧 Hardware Requerido
- **Arduino Uno**
- **Encoder E386G5** (400 PPR base × 3 = 1200 PPR)
- **Driver BTS7960** para motores DC (43A máx, más potente que L298N)
- **Ruedas** de 16cm de diámetro
- **Motores DC** con reductora
- **Fuente de alimentación** 12V/24V para motores

## 🔌 Conexiones

### Encoders:
```
Encoder Izquierdo:   A → Pin 2 (INT0)    B → Pin 4
Encoder Derecho:     A → Pin 3 (INT1)    B → Pin 5
```

### Motores (BTS7960):
```
Motor Izquierdo:     RPWM → Pin 5    LPWM → Pin 6    REN → Pin 7    LEN → Pin 8
Motor Derecho:       RPWM → Pin 9    LPWM → Pin 10   REN → Pin 11   LEN → Pin 12
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
   └── Odometry.cpp
   ```

2. Abre `AMR_Complete.ino` en Arduino IDE
3. Compila y sube al Arduino Uno

### Opción 2: Usar archivos individuales
1. Los archivos ya están organizados correctamente en el directorio actual:
   - `AMR_Complete.ino` (archivo principal)
   - `MotorDriver.h` y `MotorDriver.cpp`
   - `Encoder.h` y `Encoder.cpp`
   - `Odometry.h` y `Odometry.cpp`

2. Abre directamente `AMR_Complete.ino` en Arduino IDE
3. Todos los archivos `.h` y `.cpp` se cargan automáticamente

## 🎮 Controles

Abre el Monitor Serie a **115200 baud** y usa estos comandos:

| Comando | Acción |
|---------|--------|
| `W` | Avanzar |
| `S` | Retroceder |
| `A` | Girar 90° izquierda |
| `D` | Girar 90° derecha |
| `X` | Parar |
| `R` | Reset posición a (0,0) |
| `P` | Mostrar posición actual |
| `H` | Mostrar ayuda |

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

## 📊 Salida del Monitor Serie

```
========================================
        AMR - CONTROL SYSTEM
========================================
Hardware:
- Arduino Uno  
- Encoder E386G5: 1200 PPR
- Ruedas: 16cm diámetro
- Posición inicial: (0,0)

¡SISTEMA LISTO! Envía comandos por serie.
Posición inicial: (0.00, 0.00) - 0.0°

→ ADELANTE
POS: Posición: (5.23, 0.00) cm, Ángulo: 0.0°, Distancia: 5.23 cm

→ GIRANDO DERECHA 90°
Objetivo: 90.0°
✓ Giro completo: 88.5°
```

## 🚀 Uso

1. **Conecta** el hardware según el diagrama
2. **Sube** el código al Arduino
3. **Abre** Monitor Serie (115200 baud)
4. **Envía** comandos W, A, S, D para mover
5. **Observa** la posición actualizándose automáticamente

## ⚡ Características

- ✅ **Odometría precisa** con encoders de alta resolución
- ✅ **Giros automáticos** de exactamente 90°
- ✅ **Seguimiento de posición** en tiempo real
- ✅ **Control por teclado** intuitivo
- ✅ **Reset de posición** para recalibrar
- ✅ **Tolerancia de error** en giros (±8°)
- ✅ **Timeout de seguridad** en movimientos

## 🐛 Troubleshooting

**Encoder no cuenta:**
- Verifica conexiones en pines 2, 3, 4, 5
- Comprueba alimentación del encoder

**Motores no se mueven:**
- Verifica conexiones BTS7960 (RPWM, LPWM, REN, LEN)
- Comprueba alimentación de motores (12V/24V)
- Revisa pines PWM 5, 6, 9, 10 y enables 7, 8, 11, 12
- Confirma que REN y LEN están en HIGH

**Posición incorrecta:**
- Ajusta `WHEEL_DIAMETER_CM` y `WHEEL_BASE_CM`
- Calibra `PULSES_PER_REVOLUTION`
- Usa comando `R` para resetear

## 📝 Notas

- El sistema usa **interrupciones** en pines 2 y 3 para máxima precisión
- La **odometría** se actualiza cada 50ms
- Los **giros** tienen timeout de 4 segundos por seguridad
- La **posición** se muestra cada 2 segundos automáticamente