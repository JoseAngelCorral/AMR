from rplidar import RPLidar
import serial
import time
import struct

def reset_lidar_serial_buffer(port, baudrate=115200):
    """Reset the serial buffer to clear any corrupted data"""
    print(f"Resetting serial buffer on {port}...")
    try:
        ser = serial.Serial(port, baudrate, timeout=1)
        ser.flushInput()
        ser.flushOutput()
        time.sleep(0.5)
        ser.close()
        print("✓ Serial buffer reset")
        return True
    except Exception as e:
        print(f"✗ Buffer reset failed: {e}")
        return False

def test_rplidar_with_reset(port, baudrate=115200):
    """Test RPLidar connection with proper initialization sequence"""
    print(f"\nTesting RPLidar C1 with reset sequence on {port}...")
    
    # Step 1: Reset buffer
    if not reset_lidar_serial_buffer(port, baudrate):
        return False
    
    # Step 2: Wait for device to stabilize
    time.sleep(1)
    
    # Step 3: Try connection with longer timeout
    try:
        print("Connecting with extended timeout...")
        lidar = RPLidar(port, baudrate=baudrate, timeout=5)
        
        # Give time for initialization
        time.sleep(1)
        
        print("Getting device info...")
        info = lidar.get_info()
        print(f"✓ Device info: {info}")
        
        print("Getting health status...")
        health = lidar.get_health()
        print(f"✓ Health status: {health}")
        
        print("Testing scan capability...")
        # Start scanning briefly
        lidar.start('express')
        time.sleep(0.5)
        lidar.stop()
        
        lidar.disconnect()
        print("🎉 SUCCESS! RPLidar C1 is working properly!")
        return True
        
    except Exception as e:
        print(f"✗ Connection failed: {e}")
        try:
            lidar.disconnect()
        except:
            pass
        return False

def test_alternative_initialization(port, baudrate=115200):
    """Try alternative initialization sequence"""
    print(f"\nTrying alternative initialization on {port}...")
    
    try:
        # Reset buffer first
        reset_lidar_serial_buffer(port, baudrate)
        time.sleep(2)  # Longer wait
        
        # Create lidar instance
        lidar = RPLidar(port, baudrate=baudrate, timeout=10)  # Very long timeout
        
        # Try to reset the device
        try:
            lidar.reset()
            time.sleep(3)  # Wait after reset
        except:
            pass  # Reset might not be supported
        
        # Now try to get info
        info = lidar.get_info()
        print(f"✓ Device info with alternative method: {info}")
        
        health = lidar.get_health()
        print(f"✓ Health status: {health}")
        
        lidar.disconnect()
        print("🎉 Alternative initialization successful!")
        return True
        
    except Exception as e:
        print(f"✗ Alternative initialization failed: {e}")
        try:
            lidar.disconnect()
        except:
            pass
        return False

def main():
    port = 'COM10'
    baudrates = [115200, 256000, 230400, 9600, 57600]
    
    print("=== Advanced RPLidar C1 Connection Test ===\n")
    
    for baudrate in baudrates:
        print(f"\n--- Testing with baudrate {baudrate} ---")
        
        if test_rplidar_with_reset(port, baudrate):
            print(f"\n✅ WORKING CONFIGURATION FOUND!")
            print(f"Port: {port}")
            print(f"Baudrate: {baudrate}")
            return
            
        if test_alternative_initialization(port, baudrate):
            print(f"\n✅ WORKING CONFIGURATION FOUND!")
            print(f"Port: {port}")
            print(f"Baudrate: {baudrate}")
            return
    
    print("\n❌ All connection attempts failed.")
    print("\nThis could indicate:")
    print("1. Hardware issue with the RPLidar C1")
    print("2. Faulty USB cable")
    print("3. Power supply issue")
    print("4. Driver compatibility problem")
    print("5. The device might not be an RPLidar C1")

if __name__ == "__main__":
    main()