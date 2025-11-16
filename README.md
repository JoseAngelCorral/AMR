# 🤖 AMR (Autonomous Mobile Robot) - Sistema de Control Completo

## 📋 Descripción del Proyecto

Sistema completo de control para robot móvil autónomo basado en Arduino Uno (o UNO R4 WiFi) con control de motores, odometría, interfaz web y navegación por táctil. El robot incluye encoders, drivers de motor BTS7960 y capacidades de tracking de posición en tiempo real.

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
## 🔌 Conexiones de Hardware y funcionamiento

Esta sección describe las conexiones físicas (pines) usadas por el firmware y el
propósito de cada bloque hardware. Usa estas indicaciones para cablear el robot
de forma segura y comprobar que las señales coinciden con el código fuente.

IMPORTANTE (alimentación y seguridad):
- Los BTS7960 controlan la alimentación de los motores y deben alimentarse desde
	una fuente externa (12V o 24V según tus motores). Nunca alimentes los motores
	desde la salida 5V del Arduino.
- Conectar las masas (GND) del Arduino y de la fuente de motor: GND común.
- REN / LEN (enables) de cada BTS7960 deben estar a nivel HIGH (VCC) para permitir
	el driver; en este diseño están alimentados externamente y no se manejan desde
	Arduino.

Resumen de pines (firmware actual)
- Encoders
	- Encoder izquierdo:  A = `PIN 8`   | B = `PIN 2`  (INT0)
	- Encoder derecho :  A = `PIN 9`   | B = `PIN 3`  (INT1)
	- Nota: en Arduino Uno los pines 2 y 3 son las interrupciones externas usadas
		por las ISRs del encoder. Los pines A (8/9) son señales de fase.

- Drivers de motor (BTS7960)
	- Motor izquierdo (BTS7960 #1):
		- RPWM = `PIN 10`  (PWM - usado para sentido "atrás")
		- LPWM = `PIN 11`  (PWM - usado para sentido "adelante")
	- Motor derecho (BTS7960 #2):
		- RPWM = `PIN 5`   (PWM - usado para sentido "atrás")
		- LPWM = `PIN 6`   (PWM - usado para sentido "adelante")
	- REN / LEN (enables) → deben estar a VCC/5V para habilitar los drivers.
	- En el código `MotorDriver.cpp` se documenta y usa la convención de que
		LPWM activa el movimiento "adelante" y RPWM activa "atrás".

- Sensores IR analógicos (arreglo):
	- LEFT_SIDE  -> `A0`
	- FRONT_LEFT -> `A1`
	- BACK_CENTER-> `A2`
	- FRONT_RIGHT-> `A4`
	- RIGHT_SIDE -> `A5`
	- Conectar VCC -> `5V`, GND -> `GND`, OUT -> pin analógico correspondiente.

- Otros periféricos / notas:
	- WiFi (UNO R4 WiFi): el sketch usa la librería `WiFiS3` y arranca como punto
		de acceso con SSID `AMR_Robot_AP`. Si usas otra placa o core, adapta según
		la plataforma.

Funcionamiento y propósito de cada conexión
- Encoders:
	- Proveen conteo de pulsos por cada paso de la rueda. El firmware usa los
		pulsos (left/right) para odometría y control en lazo cerrado (PID).
	- Las señales B de cada encoder van a pines 2 y 3 para generar interrupciones
		y contar pulsos con mínima latencia.
	- `Encoder::pulsesToCentimeters()` convierte pulsos a distancia usando la
		circunferencia de rueda y la constante `pulsesPerRevolution` (por defecto
		calibrada a 3418 PPR en `Encoder.h`).

- Drivers BTS7960 y motores:
	- BTS7960 usa dos entradas PWM por motor (RPWM/LPWM). Activando una u otra
		se define el sentido; las señales PWM modulan la potencia.
	- El firmware mantiene los enables (REN/LEN) externamente en HIGH. Si tu
		placa requiere controlar enables desde Arduino, agrégalo y actualiza
		`MotorDriver::init()` para configurar el pin.
	- En `MotorDriver.cpp` las funciones `moveForward`, `moveBackward`,
		`turnLeft` y `turnRight` aplican combinaciones de RPWM/LPWM según el sentido
		deseado. Por ejemplo, para avanzar se aplica PWM en LPWM y se pone RPWM=0.

- Sensores IR:
	- Lectura analógica con promedio de 3 muestras (configurable). Un umbral
		determina detección (por defecto `IR_THRESHOLD` en el sketch).
	- Se usan para detección de obstáculos y podrían integrarse en la lógica de
		seguridad (por ejemplo impedir avanzar si un sensor frontal detecta algo).

Consejos prácticos de cableado y pruebas
- Antes de conectar los motores, prueba la lógica con el test de motores:
	- Conecta sólo el Arduino al PC y deja los BTS7960 sin alimentar; ejecuta el
		comando `T` (test) para comprobar que la lógica PWM funciona (salidas PWM
		cambiarán, pero sin alimentación externa los motores no girarán).
- Conecta la fuente de motor y coloca GND común con Arduino. Ejecuta `T` y
	observa la respuesta de los motores (con cautela).
- Para comprobar encoders:
	- Abre el Serial Monitor a 115200 baudios.
	- Mueve manualmente una rueda y comprueba que los contadores cambian (usa el
		comando `P` o las salidas `encoders` impresas en `setup()`/diagnóstico).
- Si un motor gira en sentido opuesto al esperado, intercambia las salidas
	RPWM/LPWM en el cableado del BTS7960 (o modifica la lógica en
	`MotorDriver.cpp`).

## ✅ Tests y funcionamiento de las pruebas

El firmware incluye utilidades de prueba para verificar motores y calibrar
los encoders desde Serial. Ejecuta las pruebas con el Serial Monitor a
115200 baudios.

- Comando `T` — Test completo de motores
	- Qué hace: ejecuta una secuencia automática de pruebas en ambos motores:
		1) Motor izquierdo adelante (LPWM activo) durante 1s
		2) Motor izquierdo atrás (RPWM activo) durante 1s
		3) Motor derecho adelante (LPWM activo) durante 1s
		4) Motor derecho atrás (RPWM activo) durante 1s
	- Salidas por Serial: mensajes de inicio y pasos como `Izq+`, `Izq-`, `Der+`,
		`Der-` y finalmente `Test OK`.
	- Seguridad: puedes ejecutar `T` con el BTS7960 sin alimentar (solo Arduino
		conectado) para comprobar que las salidas PWM cambian sin mover los
		motores; después conecta la fuente de motor (GND común) y repite con
		precaución.

- Comando `V` — Avanzar exactamente 1 vuelta (calibración PPR)
	- Qué hace: arranca el movimiento hacia adelante y espera hasta que la
		rueda que más ha girado alcance el número de pulsos definido en
		`encoders.getPulsesPerRevolution()` (valor por defecto o calibrado).
	- Proceso:
		1) Lee contadores iniciales (left0/right0) y obtiene `target = pulsesPerRevolution`.
		2) Llama a `motors.moveForward()` (usa la velocidad por defecto `DEFAULT_SPEED`).
		3) Mide continuamente los incrementos de tics e imprime periódicamente
			 `Ticks L:<n> R:<n> Avg:<n> Max:<n>` hasta que `Max >= target`.
		4) Para los motores, calcula el pulso medido por revolución (la rueda
			 que registró más pulsos) y actualiza en runtime
			 `encoders.setPulsesPerRevolution(measured)`.
	- Salidas por Serial: impresiones de progreso y líneas finales como
		`Measured pulses/rev:<n>` y `Pulses_per_rev updated to: <n>` y `Hecho: 1 vuelta`.
	- Uso: repetir la prueba varias veces y promediar los resultados para una
		calibración más estable. Si quieres persistir el valor entre reinicios,
		puedo añadir almacenamiento en EEPROM.

- Función `testRightMotor()` (en `MotorDriver.cpp`) — test específico del motor
	- Existe como API interna y realiza una prueba larga del motor derecho
		(Pin6 adelante, Pin5 atrás). Si deseas un comando Serial corto (ej. `M`)
		que invoque esta función, lo puedo añadir al menú de comandos.

## 📊 Tabla de velocidades (PWM) — manual y automático

La tabla siguiente resume los valores actuales usados en firmware (valores
basados en constantes definidas en `MotorDriver.h` y uso en
`AMR_Complete.ino`):

| Modo | Acción | Valor PWM (aprox.) | % de MAX (255) | Comentarios |
|---|---:|---:|---:|---|
| Manual | Adelante / Atrás (hold - `W` / `S`) | 102 | 40% | Implementado como (int)(MAX_SPEED * 0.40f) |
| Manual | Giro en sitio (hold - `Q` / `E`) | 51 | 20% | Implementado como (int)(MAX_SPEED * 0.20f); `turnLeft/turnRight` permiten < MIN_SPEED |
| Automático | Avance por defecto (moveForward()) | 102 | 40% | `DEFAULT_SPEED = 102` definido en `MotorDriver.h` (igual a velocidad manual hold `W`/`S`) |
| Automático | Giro automático 90° (`A` / `D`) | 51 | 20% | `TURN_SPEED = 51` definido en `MotorDriver.h`; igual a giro manual hold `Q`/`E` |
| Sistema | MIN_SPEED (umbral aplicado en setLeft/Right) | 80 | 31.4% | Velocidad mínima usada para garantizar que el motor mueva (evita fricción estática). NOTA: las funciones `turnLeft/turnRight` manuales permiten 0..MAX_SPEED y pueden ser más bajas que `MIN_SPEED`. |
| Sistema | MAX_SPEED | 255 | 100% | Valor máximo PWM |

Notas:
- Los valores en PWM son enteros 0..255. Los porcentajes son aproximados.
- La convención en `MotorDriver.cpp` es: LPWM activa el movimiento "adelante"
	y RPWM activa "atrás" (por eso `moveForward()` activa LPWM y pone RPWM a 0).
- El lazo PID de velocidad (si está activado) calcula salidas que se mapean a
	PWM y, actualmente, el firmware aplica una capa de protección que promedia
	las salidas PWM izquierda/derecha antes de aplicarlas a ambos motores para
	reducir la deriva en curvas (ver `MotorDriver::updateVelocityControl`).
