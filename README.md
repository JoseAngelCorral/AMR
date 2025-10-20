# 🤖 AMR (Autonomous Mobile Robot) - Sistema de Control Completo

## 📋 Descripción del Proyecto

Sistema completo de control para robot móvil autónomo basado en Arduino Uno con control de motores, odometría y navegación por teclado. El robot incluye encoders rotativos, drivers de motor BTS7960 y capacidades de tracking de posición en tiempo real.

## 🛠️ Hardware Requerido

### Componentes Principales:
- **Arduino Uno** - Microcontrolador principal
- **Encoder E386G5** - 400 PPR × 3 = 1200 PPR total
- **Driver BTS7960** (x2) - Control de motores DC (hasta 43A)
- **Motores DC** (x2) - Con ruedas de 16cm de diámetro
- **Fuente de alimentación** - Para motores (12V/24V recomendado)

### Especificaciones:
- **Resolución de encoder**: 1200 pulsos por revolución
- **Diámetro de rueda**: 16cm
- **Velocidad máxima**: Variable (0-255 PWM)
- **Precisión de giro**: ±8° de tolerancia

## 🔌 Conexiones de Hardware

### Encoders:
```
Motor Izquierdo:
- Encoder A: Pin 13 (Arduino)
- Encoder B: Pin 3 (INT1 - Interrupción)

Motor Derecho:  
- Encoder A: Pin 12 (Arduino)
- Encoder B: Pin 2 (INT0 - Interrupción)
```

### Motores (BTS7960):
```
Motor Izquierdo:
- RPWM: Pin 10 (PWM Atrás)
- LPWM: Pin 11 (PWM Adelante)

Motor Derecho:
- RPWM: Pin 5 (PWM Atrás) 
- LPWM: Pin 6 (PWM Adelante)

Enables (REN/LEN): Alimentación externa (siempre HIGH)
```

### Alimentación:
- **Arduino**: 5V USB o 7-12V jack
- **Motores**: Fuente externa 12V/24V conectada a BTS7960
- **Enables**: Conectar VCC de Arduino a REN/LEN de ambos BTS7960

## 🎮 Controles del Sistema

### Comandos de Movimiento:
- **`W`** - Avanzar hacia adelante
- **`S`** - Retroceder
- **`A`** - Girar 90° a la izquierda
- **`D`** - Girar 90° a la derecha  
- **`X`** - Parar todos los motores

### Comandos de Información:
- **`P`** - Mostrar posición actual (x,y,θ)
- **`R`** - Reset posición a origen (0,0,0°)
- **`H`** - Mostrar ayuda de comandos

### Comandos de Diagnóstico:
- **`T`** - Test completo de ambos motores
- **`M`** - Diagnóstico específico del motor derecho

## 📡 Comunicación Serial

- **Baudios**: 115200
- **Protocolo**: Comandos de un solo carácter
- **Respuesta**: Confirmación y estado por Serial Monitor

### Ejemplo de uso:
```
> W
Adelante
> P  
(15.2,8.7) 45°
> A
Izq 90
Obj:-45
OK
```

## 🧭 Sistema de Odometría

### Características:
- **Posición inicial**: (0,0) en coordenadas cartesianas
- **Orientación inicial**: 0° (Norte)
- **Actualización**: Cada 50ms
- **Precisión**: Submilimétrica en distancias cortas

### Cálculos:
- **Distancia por pulso**: π × 16cm ÷ 1200 = 0.0419 cm/pulso
- **Wheelbase**: Distancia entre ruedas (configurable)
- **Algoritmo**: Odometría diferencial con integración de Euler

## 📁 Estructura del Proyecto

```
AMR/
├── arduino/src/AMR_Complete/
│   ├── AMR_Complete.ino          # Programa principal
│   ├── MotorDriver.h/.cpp        # Control de motores BTS7960  
│   ├── Encoder.h/.cpp            # Manejo de encoders E386G5
│   └── Odometry.h/.cpp           # Sistema de odometría
├── raspberry/                    # Lógica Python (futuro)
├── docs/                         # Documentación técnica
└── README.md                     # Esta documentación
```

## 🚀 Instalación y Uso

### 1. Configuración del Arduino IDE:
```bash
1. Abrir Arduino IDE
2. Seleccionar placa: Arduino Uno
3. Seleccionar puerto COM correcto
4. Abrir archivo: AMR_Complete.ino
```

### 2. Compilación y Carga:
```bash
1. Verificar código (Ctrl+R)
2. Comprobar memoria: <90% RAM, <40% Flash
3. Subir a Arduino (Ctrl+U)
```

### 3. Operación:
```bash
1. Abrir Serial Monitor (115200 baudios)
2. Enviar comandos individuales (W/A/S/D/X/P/R/T/M/H)
3. Observar respuestas del sistema
```

## 🔧 Resolución de Problemas

### Problema: Motores giran en dirección incorrecta
**Solución**: Intercambiar conexiones RPWM/LPWM en el BTS7960

### Problema: Encoders no cuentan
**Solución**: 
- Verificar conexiones en pines 2 y 3 (interrupciones)
- Comprobar alimentación de encoders (5V)
- Usar comando `P` para verificar conteo

### Problema: Error de memoria al compilar
**Solución**: Código ya optimizado para Arduino Uno (uso <90% RAM)

### Problema: Motor no responde
**Solución**:
- Usar comando `M` para diagnóstico específico
- Verificar alimentación externa de motores
- Comprobar conexiones REN/LEN (enables)

## 📊 Especificaciones Técnicas

### Rendimiento:
- **Velocidad lineal máxima**: ~50 cm/s (aprox)
- **Velocidad angular**: ~45°/s en giros
- **Precisión de posición**: ±2cm en trayectos cortos
- **Precisión angular**: ±8° en giros automáticos

### Memoria (Arduino Uno):
- **Flash**: ~12KB/32KB (37%)
- **RAM**: ~1.8KB/2KB (90%) - Optimizado
- **EEPROM**: No utilizada

### Comunicación:
- **Serial**: 115200 baud, 8N1
- **Latencia**: <50ms respuesta promedio
- **Buffer**: Limpieza automática de comandos

## 🎯 Características Implementadas

- ✅ Control bidireccional de motores
- ✅ Odometría diferencial en tiempo real  
- ✅ Giros automáticos de 90° con feedback
- ✅ Sistema de coordenadas cartesianas
- ✅ Diagnóstico completo de hardware
- ✅ Interfaz serial interactiva
- ✅ Gestión de memoria optimizada
- ✅ Manejo de interrupciones para encoders

## 🔮 Desarrollo Futuro

### Próximas características:
- [ ] Integración con Raspberry Pi
- [ ] Control remoto WiFi/Bluetooth
- [ ] Navegación autónoma con sensores
- [ ] Mapeo y localización (SLAM)
- [ ] Interface web de control
- [ ] Integración con sensores adicionales (LiDAR, cámara)

## 📞 Contacto y Soporte

Para reportar problemas, sugerencias o contribuciones:
- **Repositorio**: https://github.com/JoseAngelCorral/AMR
- **Documentación técnica**: Ver carpeta `docs/`

## 📄 Licencia

Este proyecto está bajo licencia MIT. Ver archivo LICENSE para más detalles.

---

**Última actualización**: Octubre 2025  
**Versión del firmware**: v1.0  
**Estado**: Funcional y operativo ✅