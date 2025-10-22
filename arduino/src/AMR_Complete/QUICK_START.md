# 🚀 Bug Algorithm - Quick Start Guide

Get your AMR robot navigating autonomously in 5 minutes!

## ⚡ Prerequisites

- ✅ Arduino Uno with AMR_Complete.ino uploaded
- ✅ 5 IR sensors connected to pins A0-A4
- ✅ Motors and encoders working (test with command `T`)
- ✅ Serial Monitor open at 115200 baud

## 📋 5-Minute Setup

### Step 1: Verify Hardware (30 seconds)
```
> T
=== TEST MOTORES ===
Izq+
Izq-
Der+
Der-
Test OK
```
✅ If motors work, continue. Otherwise check connections.

### Step 2: Check IR Sensors (1 minute)
```
> I
=== Sensores IR ===
Lat Izq (A0): 45 [LIBRE]
Lat Der (A1): 52 [LIBRE]
Fro Izq (A2): 38 [LIBRE]
Fro Cen (A3): 41 [LIBRE]
Fro Der (A4): 49 [LIBRE]
```
✅ All sensors should show readings. Place object near sensor and verify value increases.

### Step 3: Calibrate (2 minutes)
```
> C
=== Calibracion IR ===
Valores actuales (0-1023):
LS:45 RS:52 FL:38 FC:41 FR:49
Umbral: 400
```

Now place an object close to each sensor and send `C` again:
- If values are **>400** when object is near → calibration OK ✅
- If values are **<400** when object is near → lower threshold in code
- If values are **>400** without object → increase threshold in code

**To adjust threshold**: Edit `InfraredSensor.h` line 13:
```cpp
#define IR_THRESHOLD_NEAR 400   // Change this value
```

### Step 4: Test Bug Algorithm (1 minute)
Clear some space (minimum 2x2 meters) and activate:
```
> B
Bug Algorithm ACTIVADO
Modo: Exploracion autonoma
Estado: Avanzar
```

Robot should start moving forward. Place obstacle in front to test avoidance.

### Step 5: Monitor Operation (30 seconds)
While Bug Algorithm is running:
```
> G
=== Bug Algorithm Status ===
Activo: SI
Estado: Avanzar
Sin objetivo definido

> I
=== Sensores IR ===
Lat Izq (A0): 48 [LIBRE]
Lat Der (A1): 55 [LIBRE]
Fro Izq (A2): 42 [LIBRE]
Fro Cen (A3): 687 [OBSTACULO]  ← Detected!
Fro Der (A4): 45 [LIBRE]
```

## 🎮 Essential Commands

| Command | What it does | When to use |
|---------|--------------|-------------|
| `B` | Start/Stop Bug Algorithm | Begin autonomous navigation |
| `X` | Emergency STOP | Danger or need to stop immediately |
| `I` | Show sensor values | Check if sensors detect obstacles |
| `G` | Show algorithm status | See what robot is doing |
| `C` | Calibration mode | Find correct threshold values |

## 🎯 What to Expect

### Normal Operation:
1. **Robot advances** → `Estado: Avanzar`
2. **Detects obstacle** → Stops, analyzes sensors
3. **Turns to avoid** → `Estado: Evitar izq/der`
4. **Continues forward** → Back to `Estado: Avanzar`
5. **If wall detected** → `Estado: Seguir pared izq/der`

### State Transitions:
```
Avanzar → (obstacle) → Evitar → (clear) → Avanzar
Avanzar → (wall side) → Seguir pared → (clear) → Avanzar
```

## ⚠️ Troubleshooting (1 minute fixes)

### Problem: Robot spins continuously
**Cause**: Sensors too sensitive
**Fix**: Send `C` to check values. If >400 without obstacle, increase threshold:
```cpp
#define IR_THRESHOLD_NEAR 600  // Increased from 400
```

### Problem: Robot doesn't detect obstacles
**Cause**: Sensors not sensitive enough
**Fix**: Send `I` with object near sensor. If value doesn't increase above 400, decrease threshold:
```cpp
#define IR_THRESHOLD_NEAR 200  // Decreased from 400
```

### Problem: Robot stuck in corner
**Fix**: Press `X` to stop, `B` to restart, or manually reposition

### Problem: Bug Algorithm won't start
**Fix**: Make sure no manual turn in progress (wait for completion or press `X`)

## 📱 Quick Reference Card

```
╔════════════════════════════════════════╗
║     AMR Bug Algorithm Commands         ║
╠════════════════════════════════════════╣
║                                        ║
║  B - Start/Stop Bug Algorithm          ║
║  X - Emergency Stop                    ║
║  I - Show Sensor Values                ║
║  G - Algorithm Status                  ║
║  C - Calibrate Sensors                 ║
║  P - Show Position                     ║
║  H - Help                              ║
║                                        ║
║  Sensor Pins:                          ║
║  A0=Left Side    A1=Right Side        ║
║  A2=Front Left   A3=Front Center      ║
║  A4=Front Right                        ║
║                                        ║
║  Default Threshold: 400                ║
║  Adjust in: InfraredSensor.h          ║
║                                        ║
╚════════════════════════════════════════╝
```

## 🎓 Tips for Best Results

1. **Start in open space** - Test basic movement first
2. **Use simple obstacles** - Boxes or walls work great
3. **Watch Serial Monitor** - See state changes in real-time
4. **Calibrate first** - Takes 2 minutes, saves hours of frustration
5. **Keep `X` ready** - Emergency stop is your friend
6. **Test sensors individually** - Use command `I` to verify each one

## 🔄 Testing Sequence

### Test 1: Open Space (Should go straight)
```
Area: Open floor, no obstacles
Expected: Robot advances forward continuously
State: "Avanzar"
```

### Test 2: Single Obstacle (Should avoid)
```
Area: Place box in front of robot
Expected: Robot stops, turns, goes around
State: "Avanzar" → "Evitar" → "Avanzar"
```

### Test 3: Wall Following (Should follow wall)
```
Area: Place robot parallel to wall
Expected: Robot follows wall keeping distance
State: "Seguir pared"
```

### Test 4: Corridor (Should navigate through)
```
Area: Between two parallel walls
Expected: Robot navigates through, avoids ends
State: Various transitions
```

## 📊 Success Criteria

✅ Robot advances when path is clear  
✅ Robot stops when obstacle detected  
✅ Robot turns to avoid obstacles  
✅ Robot follows walls without collision  
✅ Emergency stop (`X`) works immediately  
✅ Sensor readings make sense (`I` command)  
✅ States transition logically (`G` command)  

## 🆘 Need More Help?

- **Detailed Guide**: See `BUG_ALGORITHM_GUIDE.md`
- **Hardware Setup**: See `HARDWARE_SETUP.txt`
- **Full Documentation**: See `README.md`
- **Technical Details**: See `IMPLEMENTATION_SUMMARY.md`

## 🎉 You're Ready!

If you've completed the 5-minute setup and tests pass, you're ready to explore autonomous navigation! 

Start with simple environments and gradually increase complexity.

---

**Quick Start Version**: 1.0  
**Last Updated**: October 2025  
**Estimated Time**: 5-10 minutes  

Happy navigating! 🤖
