import serial
import serial.tools.list_ports
from rplidar import RPLidar
import time

def list_available_ports():
    """List all available COM ports"""
    print("Available COM ports:")
    ports = serial.tools.list_ports.comports()
    for port in ports:
        print(f"  {port.device} - {port.description}")
    return [port.device for port in ports]

def test_serial_connection(port, baudrate=115200):
    """Test basic serial connection"""
    try:
        print(f"\nTesting serial connection on {port} at {baudrate} baud...")
        ser = serial.Serial(port, baudrate, timeout=2)
        time.sleep(0.1)
        ser.close()
        print(f"✓ Serial connection successful on {port}")
        return True
    except Exception as e:
        print(f"✗ Serial connection failed on {port}: {e}")
        return False

def test_rplidar_connection(port, baudrate=115200):
    """Test RPLidar connection with different approaches"""
    print(f"\n--- Testing RPLidar on {port} ---")
    
    # Test 1: Basic connection
    try:
        print("Test 1: Basic RPLidar connection...")
        lidar = RPLidar(port, baudrate=baudrate, timeout=1)
        lidar.disconnect()
        print("✓ Basic connection successful")
    except Exception as e:
        print(f"✗ Basic connection failed: {e}")
        return False
    
    # Test 2: Get device info
    try:
        print("Test 2: Getting device info...")
        lidar = RPLidar(port, baudrate=baudrate, timeout=3)
        info = lidar.get_info()
        print(f"✓ Device info: {info}")
        lidar.disconnect()
    except Exception as e:
        print(f"✗ Get info failed: {e}")
        return False
    
    # Test 3: Get health
    try:
        print("Test 3: Getting health status...")
        lidar = RPLidar(port, baudrate=baudrate, timeout=3)
        health = lidar.get_health()
        print(f"✓ Health status: {health}")
        lidar.disconnect()
    except Exception as e:
        print(f"✗ Get health failed: {e}")
        return False
    
    return True

def main():
    print("=== RPLidar C1 Diagnostic Tool ===\n")
    
    # Step 1: List available ports
    available_ports = list_available_ports()
    
    if not available_ports:
        print("\n⚠️  No COM ports found. Check if the lidar is connected.")
        return
    
    # Step 2: Test each port for basic serial connectivity
    print("\n=== Testing Serial Connections ===")
    working_ports = []
    for port in available_ports:
        if test_serial_connection(port):
            working_ports.append(port)
    
    if not working_ports:
        print("\n⚠️  No working serial ports found.")
        return
    
    # Step 3: Test RPLidar connection on working ports
    print("\n=== Testing RPLidar Connections ===")
    
    # Common baud rates for RPLidar
    baud_rates = [115200, 256000, 230400]
    
    for port in working_ports:
        for baudrate in baud_rates:
            if test_rplidar_connection(port, baudrate):
                print(f"\n🎉 SUCCESS! RPLidar C1 working on {port} at {baudrate} baud")
                print(f"Update your code to use: PORT_NAME = '{port}', BAUDRATE = {baudrate}")
                return
    
    print("\n❌ RPLidar connection failed on all ports and baud rates.")
    print("\nTroubleshooting tips:")
    print("1. Check power connection to the lidar")
    print("2. Try a different USB cable")
    print("3. Restart the lidar (unplug and reconnect)")
    print("4. Check if another program is using the lidar")
    print("5. Try different USB ports")

if __name__ == "__main__":
    main()