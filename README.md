# 🤖 AMR (Autonomous Mobile Robot) - Sistema de Control Completo

## 📋 Descripción del Proyecto

Sistema completo de control para robot móvil autónomo basado en Arduino Uno (o UNO R4 WiFi) con control de motores, odometría, interfaz web y navegación por teclado/táctil. El robot incluye encoders, drivers de motor BTS7960 y capacidades de tracking de posición en tiempo real.

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
````markdown
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

### Sensores Infrarrojos (IR) - Analógicos

Este proyecto soporta un arreglo de 5 sensores IR analógicos conectados a los
pines A0..A5 del Arduino (configuración usada en el sketch `AMR_Complete.ino`).
Se asumió que los sensores funcionan a 5V y devuelven un valor analógico en el
rango 0..1023 donde valores mayores representan detección.

Conexión recomendada:
- VCC del sensor -> 5V del Arduino
- GND del sensor -> GND común
- OUT del sensor -> pin analógico del Arduino

Mapeo por defecto en el firmware:
- LEFT_SIDE (lateral izquierdo)  -> `A0`
- FRONT_LEFT (frontal izquierdo) -> `A1`
- BACK_CENTER (trasero central)  -> `A2`
- FRONT_RIGHT (frontal derecho)  -> `A4`
- RIGHT_SIDE (lateral derecho)   -> `A5`

Parámetros y notas:
- Umbral por defecto: `IR_THRESHOLD = 600` (0..1023). Ajustar tras calibración.
- Lectura: el sketch toma 3 muestras y promedia para reducir ruido (configurable).
- En el comando de inspección `I` el sketch imprime tanto el valor bruto como el
	estado detectado (0/1) por cada sensor. Ejemplo de salida:

```
IR: L:0(123) FL:1(712) B:0(200) FR:0(189) R:0(145)
```

Calibración rápida desde Serial Monitor:
1. Abrir Serial Monitor a 115200 baudios.
2. Con el sensor apuntando a un área libre de obstáculos, enviar `I` y anotar los
	 valores "raw" (mínimos).
3. Colocar un objeto delante del sensor y enviar `I` para obtener los valores
	 máximos detectados.
4. Elegir un umbral intermedio entre mínimo y máximo (por ejemplo (min+max)/2)
	 y actualizar `IR_THRESHOLD` en el código o realizar una calibración automática
	 si se implementa posteriormente.

Acciones sugeridas en el robot:
- Evitar avanzar (comando `W`) si cualquiera de los sensores frontales detecta
	un obstáculo (lógica a añadir en el sketch opcionalmente).
- Enviar telemetría periódica de IR por `Serial1` si se usa Arduino R4 con WiFi
	para exponer datos al dashboard (pendiente de implementación).


## 🎮 Controles del Sistema

### Comandos de Movimiento:
- **`W`** - Avanzar hacia adelante
- **`S`** - Retroceder
- **`A`** - Girar 90° a la izquierda
- **`D`** - Girar 90° a la derecha  
- **`X`** - Parar todos los motores

### Controles manuales en el dashboard (UI)
- En la interfaz web (dashboard) se dispone de una cruceta (D-pad) táctil con los mismos comandos:
	- Mantener pulsado `↑ Adelante` emite `W` en modo hold y avanza mientras está pulsado.
	- Mantener pulsado `↓ Atrás` emite `S` en modo hold y retrocede mientras está pulsado.
	- Mantener pulsado `← Izq` emite `Q` en modo hold y produce un giro manual en sitio mientras se mantiene pulsado.
	- Mantener pulsado `Der →` emite `E` en modo hold y gira manual en sitio mientras se mantiene pulsado.
	- El botón central `⏹ Stop` envía `X`.

Velocidades manuales por defecto implementadas:
- Giro manual (Q/E): 20% PWM de `MAX_SPEED` (para giros lentos y controlables en sitio).
- Avance/retroceso manual (W/S): 40% PWM de `MAX_SPEED` (más estable para control táctil).

Nota: los giros automáticos por 90° siguen manteniéndose con `A`/`D` y usan el cálculo por encoders (`startAutoTurn`) — la funcionalidad automática de 90° no cambió.

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

### Endpoints Web (Dashboard)
- `GET /` - Página principal (dashboard)
- `GET /data` - Telemetría en JSON: posición y lecturas IR
- `GET /routes` - Lista de rutas (JSON)
- `GET /routes_ui` - Página de UI para selección e inicio de rutas
- `GET /start_route?route=<i>&dir=ida|retorno&delay=<ms>` - Programar ejecución de ruta
- `GET /stop_route` - Detener ejecución de ruta
- `GET /route_status` - Estado de ejecución de ruta (JSON)
- `GET /confirm_route` - Confirmar inicio programado (inicia inmediatamente si estaba en espera)
- `GET /cmd?c=<CHAR>` - Enviar comando simple desde la UI (ej: `c=W`)

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
2. Enviar comandos individuales (W/A/S/D/X/P/R/T/I/K/H) o usar el dashboard web
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
 - ✅ Interfaz web/Dashboard con D-pad táctil
 - ✅ Hold-to-turn y hold-to-move en la UI (W/S/Q/E)
 - ✅ Ajustes manuales: giro 20% PWM, avance/retroceso 40% PWM
 - ✅ Endpoints REST simples para rutas y telemetría

## 🔮 Desarrollo Futuro

-### Próximas características:
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