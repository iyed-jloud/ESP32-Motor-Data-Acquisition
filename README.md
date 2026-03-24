# ESP32-Motor-Data-Acquisition

This repository contains Arduino/C++ scripts developed for an ESP32 microcontroller to extract real-time datasets from a motor system for predictive maintenance.

### 1. Time and Voltage Acquisition (`time_voltage_acquisition.ino`)
This first script focuses on basic sensor reading.
- **Features:** Extracts Time (ms) and Voltage (V) from the motor.
- **Smart Trigger:** Includes a software trigger to automatically start recording the dataset only when the motor voltage exceeds 2.0V.
