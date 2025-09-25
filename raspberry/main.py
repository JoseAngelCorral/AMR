"""
Punto de entrada principal para la lógica en Raspberry Pi.
Orquesta la navegación, sensado, comunicación y dashboard.
"""

from robot.navigation.navigator import Navigator
from robot.sensors.lidar import Lidar
from robot.comm.arduino_serial import ArduinoSerial
from robot.dashboard.app import run_dashboard

def main():
    # Inicialización de módulos
    lidar = Lidar()
    nav = Navigator()
    arduino = ArduinoSerial()
    
    # Ejemplo de ciclo principal
    while True:
        obs = lidar.read()
        nav.update(obs)
        arduino.send(nav.motor_commands())
        # ... otros procesos ...
        
if __name__ == "__main__":
    main()