import serial
import time
import struct

def send_raw_command(port, baudrate=115200):
    """Send raw commands to test RPLidar communication"""
    print(f"Testing raw serial communication on {port}...")
    
    try:
        ser = serial.Serial(port, baudrate, timeout=2)
        time.sleep(0.5)
        
        # RPLidar command bytes
        # Get device info command: A5 20 00 00 00 C1
        get_info_cmd = bytes([0xA5, 0x20, 0x00, 0x00, 0x00, 0xC1])
        
        print("Sending GET_INFO command...")
        ser.write(get_info_cmd)
        time.sleep(0.1)
        
        # Try to read response
        response = ser.read(100)  # Read up to 100 bytes
        
        if response:
            print(f"Received {len(response)} bytes: {response.hex()}")
            
            # Check if it starts with the expected response header
            if len(response) >= 2 and response[0] == 0xA5 and response[1] == 0x5A:
                print("✓ Valid response header detected!")
                
                # Try to parse device info
                if len(response) >= 20:
                    # Skip header (7 bytes) and parse device info (20 bytes)
                    model = response[7]
                    firmware_minor = response[8]
                    firmware_major = response[9] 
                    hardware = response[10]
                    serial_number = response[11:27].hex()
                    
                    print(f"Model: 0x{model:02X}")
                    print(f"Firmware: {firmware_major}.{firmware_minor}")
                    print(f"Hardware: {hardware}")
                    print(f"Serial: {serial_number}")
                    
                    # Check if it's really a C1 model (C1 typically has model ID 0x28)
                    if model == 0x28:
                        print("✓ Confirmed: This is an RPLidar C1")
                    else:
                        print(f"⚠️  Model ID 0x{model:02X} - This might not be a C1!")
                        print("Common model IDs:")
                        print("  0x28 = RPLidar C1")
                        print("  0x40 = RPLidar A1")
                        print("  0x60 = RPLidar A2")
                        print("  0x80 = RPLidar A3")
                
            else:
                print("✗ Invalid response header")
                print("Expected: A5 5A, Got:", response[:2].hex() if len(response) >= 2 else "No data")
        else:
            print("✗ No response received")
        
        ser.close()
        return len(response) > 0
        
    except Exception as e:
        print(f"✗ Raw communication failed: {e}")
        return False

def test_different_protocols():
    """Test with different communication protocols"""
    port = 'COM10'
    
    print("=== Raw RPLidar Communication Test ===\n")
    
    # Test common baud rates
    baud_rates = [115200, 256000, 230400, 57600, 38400, 19200, 9600]
    
    for baudrate in baud_rates:
        print(f"\n--- Testing baudrate {baudrate} ---")
        if send_raw_command(port, baudrate):
            print(f"✓ Communication successful at {baudrate} baud")
            return True
        else:
            print(f"✗ No valid communication at {baudrate} baud")
    
    return False

if __name__ == "__main__":
    if not test_different_protocols():
        print("\n❌ Raw communication failed at all baud rates")
        print("\nThis indicates:")
        print("1. Device is not responding")
        print("2. Wrong COM port")
        print("3. Hardware/cable issue")
        print("4. Device might not be an RPLidar")
        print("\nPlease check:")
        print("- Power LED on the device")
        print("- USB cable connection")
        print("- Try different USB port")
        print("- Check device manager for correct COM port")
    else:
        print("\n✅ Raw communication working - library compatibility issue")