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

¿Quieres que añada un diagrama de conexión ASCII o una imagen SVG en `docs/`?
Puedo generar un diagrama con las conexiones anteriores y añadirlo al repo.
