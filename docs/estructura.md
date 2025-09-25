# Estructura Modular del Proyecto

## raspberry/
- `main.py`: Orquestador principal.
- `robot/`: Librerías propias de alto nivel.
    - `sensors/`: Manejo de sensores.
    - `navigation/`: Algoritmos de navegación y evasión.
    - `comm/`: Comunicación con Arduino.
    - `dashboard/`: Interfaz web/control remoto.
- `config/`: Parámetros y configuraciones (JSON/YAML).
- `utils/`: Utilidades y funciones auxiliares.

## arduino/
- `src/`: Código fuente principal.
    - `include/`: Librerías propias (drivers sensores, motores).
    - `main.ino`: Programa principal.
- `lib/`: Librerías de terceros.

## maps/
- Mapas, rutas, zonas prohibidas, etc.

## tests/
- Pruebas para cada módulo.