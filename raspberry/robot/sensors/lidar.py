import matplotlib.pyplot as plt
import numpy as np
from rplidar import RPLidar

PORT_NAME = 'COM10'
lidar = RPLidar(PORT_NAME)

fig = plt.figure()
ax = plt.subplot(111, projection='polar')
line, = ax.plot([], [], 'bo', markersize=2)

def update_points(scan):
    angles = np.radians([x[1] for x in scan])
    distances = [x[2] for x in scan]
    line.set_xdata(angles)
    line.set_ydata(distances)
    plt.draw()
    plt.pause(0.01)

try:
    for scan in lidar.iter_scans():
        update_points(scan)
except KeyboardInterrupt:
    print("Interrumpido por usuario.")
finally:
    lidar.stop()
    lidar.disconnect()
    plt.close(fig)



    # Para ejecutar este script, abre una terminal y navega al directorio donde está guardado el archivo.
    # Luego ejecuta el siguiente comando:

    # python lidar.py

    # Asegúrate de tener instaladas las dependencias necesarias:
    # pip install matplotlib numpy rplidar