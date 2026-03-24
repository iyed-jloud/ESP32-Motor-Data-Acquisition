# ESP32-Motor-Data-Acquisition

This repository contains Arduino/C++ scripts developed for an ESP32 microcontroller to extract real-time datasets from a motor system for predictive maintenance.

### 1. Time and Voltage Acquisition (`time_voltage_acquisition.ino`)
This first script focuses on basic sensor reading.
- **Features:** Extracts Time (ms) and Voltage (V) from the motor.
- **Smart Trigger:** Includes a software trigger to automatically start recording the dataset only when the motor voltage exceeds 2.0V.
  
- ### 2. Time, Voltage, and Speed Acquisition (`time_voltage_speed_acquisition.ino`)
This second script introduces hardware interrupts for motor speed measurement.
- **Features:** Extracts Time (ms), Voltage (V), and Speed (RPM).
- **Hardware Integration:** Uses an interrupt routine with a 2ms software anti-noise filter to accurately count encoder pulses and calculate the RPM.
