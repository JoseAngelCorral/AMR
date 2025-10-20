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
| `T` | Test completo de motores |
| `M` | Diagnóstico motor derecho |

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

- ✅ **Odometría precisa** con encoders de alta resolución
- ✅ **Giros automáticos** de exactamente 90°
- ✅ **Seguimiento de posición** en tiempo real
- ✅ **Control por teclado** intuitivo
- ✅ **Reset de posición** para recalibrar
- ✅ **Tolerancia de error** en giros (±8°)
- ✅ **Timeout de seguridad** en movimientos

## 🐛 Troubleshooting

**Encoder no cuenta:**
- Verifica conexiones: Izq (13,3) Der (12,2)
- Comprueba alimentación del encoder
- Solo pines 2 y 3 tienen interrupciones (INT0, INT1)

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

## 📝 Notas Importantes

- **Memoria optimizada**: Código reducido para Arduino Uno (<90% RAM)
- **Interrupciones**: Pines 2 y 3 para máxima precisión de encoders
- **Odometría**: Se actualiza cada 50ms automáticamente  
- **Giros**: Timeout de 4 segundos por seguridad
- **Strings optimizados**: Uso de F() macro para ahorrar RAM
- **Dirección corregida**: LPWM=Adelante, RPWM=Atrás (funcionando)
- **Posición manual**: Usar comando `P` (no automático)
- **Diagnóstico**: Comandos `T` y `M` para troubleshooting

## 🎯 Estado Actual: ✅ FUNCIONAL

- ✅ Compilación exitosa (memoria optimizada)
- ✅ Ambos motores responden correctamente
- ✅ Direcciones corregidas (adelante/atrás)
- ✅ Encoders contando correctamente  
- ✅ Odometría en tiempo real
- ✅ Comandos de diagnóstico operativos
- ✅ Sistema completo integrado y probado

**Última actualización**: Octubre 2025 - Versión optimizada y funcional