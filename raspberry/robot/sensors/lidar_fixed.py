# RPLidar C1 Troubleshooting Guide and Working Code
# ===================================================

"""
HARDWARE TROUBLESHOOTING CHECKLIST:

❌ ISSUE IDENTIFIED: No communication with RPLidar on COM10
The device is not responding to any commands, which indicates a hardware problem.

STEP-BY-STEP TROUBLESHOOTING:

1. POWER CHECK:
   - Verify the RPLidar C1 power LED is ON (usually red/green)
   - If no LED: Check power supply, USB cable, or try different USB port
   - C1 draws power from USB, ensure USB port provides enough current

2. CONNECTION CHECK:
   - Try different USB cables (data cables, not just charging cables)
   - Try different USB ports on your computer
   - If using USB hub, try connecting directly to computer

3. DRIVER CHECK:
   - Open Device Manager (Windows key + X, then M)
   - Look for "Silicon Labs CP210x USB to UART Bridge"
   - If you see yellow warning or "Unknown Device", install CP210x drivers
   - Download from: https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers

4. PORT VERIFICATION:
   - In Device Manager, expand "Ports (COM & LPT)"
   - Note the actual COM port number for the CP210x device
   - It might not be COM10 - update PORT_NAME in code accordingly

5. DEVICE TEST:
   - Try connecting on another computer to isolate the issue
   - Check if any other software is accessing the device
   - Restart your computer (sometimes helps with driver issues)

6. RPLidar RESET:
   - Unplug the device completely
   - Wait 10 seconds
   - Plug back in and wait for driver recognition
"""

import matplotlib.pyplot as plt
import numpy as np
from rplidar import RPLidar
import time
import serial.tools.list_ports

# ====== CONFIGURATION ======
# Update these values based on your actual setup
PORT_NAME = 'COM10'  # ⚠️ UPDATE THIS if your device is on a different port
BAUDRATE = 115200    # Standard for RPLidar C1
MAX_RANGE = 4000     # C1 max range is ~4m
TIMEOUT = 3          # Connection timeout

def find_rplidar_port():
    """Automatically find RPLidar port"""
    print("Scanning for RPLidar devices...")
    ports = serial.tools.list_ports.comports()
    
    for port in ports:
        print(f"Found: {port.device} - {port.description}")
        if "CP210" in port.description or "USB" in port.description:
            print(f"⚠️  Potential RPLidar port: {port.device}")
            return port.device
    
    return None

def test_connection():
    """Test RPLidar connection"""
    print(f"Testing RPLidar C1 connection on {PORT_NAME}...")
    
    try:
        # Test basic serial connection first
        import serial
        ser = serial.Serial(PORT_NAME, BAUDRATE, timeout=1)
        ser.close()
        print("✓ Serial port accessible")
    except Exception as e:
        print(f"✗ Serial port error: {e}")
        return False
    
    try:
        # Test RPLidar connection
        lidar = RPLidar(PORT_NAME, baudrate=BAUDRATE, timeout=TIMEOUT)
        info = lidar.get_info()
        health = lidar.get_health()
        lidar.disconnect()
        
        print(f"✓ RPLidar C1 connected successfully!")
        print(f"  Device info: {info}")
        print(f"  Health: {health}")
        return True
        
    except Exception as e:
        print(f"✗ RPLidar connection failed: {e}")
        print("\n🔧 TROUBLESHOOTING NEEDED:")
        print("1. Check power LED on RPLidar")
        print("2. Try different USB cable/port")
        print("3. Check Device Manager for correct COM port")
        print("4. Install CP210x drivers if needed")
        return False

def run_lidar_scan():
    """Main lidar scanning function - ONLY run when connection test passes"""
    
    if not test_connection():
        print("\n❌ Cannot proceed - fix connection issues first!")
        return
    
    print("\n🚀 Starting RPLidar C1 visualization...")
    
    # Initialize lidar
    lidar = RPLidar(PORT_NAME, baudrate=BAUDRATE, timeout=TIMEOUT)
    
    # Setup matplotlib
    plt.ion()  # Interactive mode
    fig, ax = plt.subplots(subplot_kw=dict(projection='polar'), figsize=(10, 10))
    ax.set_ylim(0, MAX_RANGE)
    ax.set_title('RPLidar C1 - Real-time Scan\nPress Ctrl+C to stop')
    line, = ax.plot([], [], 'bo', markersize=1, alpha=0.7)
    
    def update_plot(scan_data):
        """Update the polar plot with new scan data"""
        if len(scan_data) > 0:
            # Extract angles and distances
            angles = np.radians([point[1] for point in scan_data])
            distances = [point[2] for point in scan_data]
            
            # Update plot
            line.set_xdata(angles)
            line.set_ydata(distances)
            
            # Refresh display
            fig.canvas.draw()
            fig.canvas.flush_events()
            plt.pause(0.01)
    
    try:
        print("✓ Lidar scan started - visualization should appear")
        print("  Press Ctrl+C to stop scanning")
        
        scan_count = 0
        for scan in lidar.iter_scans():
            update_plot(scan)
            scan_count += 1
            
            # Print progress every 50 scans
            if scan_count % 50 == 0:
                print(f"  Scans processed: {scan_count}, Points in last scan: {len(scan)}")
    
    except KeyboardInterrupt:
        print("\n⏹️  Scan stopped by user")
    
    except Exception as e:
        print(f"\n❌ Scan error: {e}")
    
    finally:
        print("🔧 Cleaning up...")
        try:
            lidar.stop()
            lidar.disconnect()
        except:
            pass
        plt.close(fig)
        print("✓ Cleanup complete")

def main():
    """Main function"""
    print("=" * 50)
    print("       RPLidar C1 Scanner")
    print("=" * 50)
    
    # Auto-detect port if possible
    detected_port = find_rplidar_port()
    if detected_port and detected_port != PORT_NAME:
        print(f"\n⚠️  Detected port {detected_port} differs from configured {PORT_NAME}")
        print(f"   Consider updating PORT_NAME = '{detected_port}' in the code")
    
    # Run the scanner
    run_lidar_scan()

if __name__ == "__main__":
    main()

"""
USAGE INSTRUCTIONS:
1. Fix hardware connection issues first (see troubleshooting guide above)
2. Update PORT_NAME if your device is on a different COM port
3. Run: python lidar.py
4. A polar plot window should open showing real-time lidar data
5. Press Ctrl+C to stop scanning

EXPECTED RESULTS:
- Polar plot with points representing detected obstacles
- Console output showing scan progress
- Points should move/change as you move objects around the lidar

If you still get errors after following the troubleshooting guide:
- The device may be faulty
- Wrong device model (not actually a C1)
- Incompatible firmware version
"""