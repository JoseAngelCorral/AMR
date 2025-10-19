import matplotlib.pyplot as plt
import numpy as np
from rplidar import RPLidar
import time

# RPLidar C1 Configuration
PORT_NAME = 'COM10'
BAUDRATE = 115200  # Standard for RPLidar C1
MAX_RANGE = 4000   # C1 max range is ~4m
TIMEOUT = 3        # Connection timeout

def test_lidar_connection():
    """Test basic connection to RPLidar C1"""
    try:
        print(f"Testing connection to RPLidar C1 on {PORT_NAME}...")
        lidar = RPLidar(PORT_NAME, baudrate=BAUDRATE, timeout=TIMEOUT)
        
        # Wait a moment for initialization
        time.sleep(0.5)
        
        # Check health status
        health = lidar.get_health()
        print(f"Lidar Health: {health}")
        
        # Get device info
        info = lidar.get_info()
        print(f"Device Info: {info}")
        
        lidar.disconnect()
        print("Connection test successful!")
        return True
    except Exception as e:
        print(f"Connection test failed: {e}")
        print("Check:")
        print("- Is the lidar connected to the correct port?")
        print("- Is the lidar powered on?")
        print("- Are there any loose connections?")
        return False

def run_lidar_visualization():
    """Main lidar visualization function"""
    # Test connection first
    if not test_lidar_connection():
        print("Cannot connect to lidar. Check port and connections.")
        return
    
    # Initialize lidar with C1 specific settings
    lidar = RPLidar(PORT_NAME, baudrate=BAUDRATE, timeout=TIMEOUT)
    
    # Setup matplotlib
    fig = plt.figure(figsize=(10, 10))
    ax = plt.subplot(111, projection='polar')
    ax.set_ylim(0, MAX_RANGE)  # Set max range for C1
    ax.set_title('RPLidar C1 - Real-time Scan')
    line, = ax.plot([], [], 'bo', markersize=1)
    
    def update_points(scan):
        if len(scan) > 0:
            angles = np.radians([point[1] for point in scan])
            distances = [point[2] for point in scan]
            line.set_xdata(angles)
            line.set_ydata(distances)
            fig.canvas.draw()
            plt.pause(0.01)
    
    try:
        print("Starting lidar scan... Press Ctrl+C to stop")
        time.sleep(1)  # Give lidar time to initialize
        
        for i, scan in enumerate(lidar.iter_scans()):
            update_points(scan)
            if i % 10 == 0:  # Print status every 10 scans
                print(f"Scan #{i}, points: {len(scan)}")
                
    except KeyboardInterrupt:
        print("\nInterrupted by user.")
    except Exception as e:
        print(f"Error during scanning: {e}")
    finally:
        print("Stopping lidar...")
        lidar.stop()
        lidar.disconnect()
        plt.close(fig)
        print("Lidar stopped and disconnected.")

if __name__ == "__main__":
    run_lidar_visualization()



    # Para ejecutar este script, abre una terminal y navega al directorio donde está guardado el archivo.
    # Luego ejecuta el siguiente comando:

    # python lidar.py

    # Asegúrate de tener instaladas las dependencias necesarias:
    # pip install matplotlib numpy rplidar