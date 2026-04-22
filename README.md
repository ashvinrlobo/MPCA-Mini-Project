# ESP32 Smart Anti-Theft Bag Security System

An intelligent, multi-layered security system designed to protect backpacks or luggage. This project utilizes an ESP32 microcontroller to monitor proximity via Bluetooth, detect unauthorized access using ultrasonic distance sensing, and manage system status through RFID authentication.

## Features

* **RFID Authentication:** Uses MFRC522 to Arm/Disarm the system. Supports up to 3 unique authorized tags.
* **Ultrasonic Perimeter Guard:** Monitors the distance inside the bag. If the bag is opened (distance exceeds 15cm), a siren is triggered.
* **Bluetooth Classic "Leash":** Scans for specific paired devices. If the signal strength (RSSI) drops below -55dBm while monitoring, the alarm sounds to prevent "snatch-and-run" theft.
* **Mode Button:** A physical push-button to manually trigger proximity monitoring.
* **Audio Feedback:** Distinct buzzer beep patterns for arming (1 beep) and disarming (2 beeps).

## Hardware Components

| Component | Quantity | Purpose |
| :--- | :--- | :--- |
| **ESP32 DevKit V1** | 1 | Main Controller |
| **RC522 RFID Module** | 1 | Keyless Entry / Authentication |
| **HC-SR04 Ultrasonic Sensor** | 1 | Open-Bag Detection |
| **Active Buzzer** | 1 | Alarm Siren |
| **Push Button** | 1 | Proximity Mode Trigger |

## Pin Mapping

### 1. RFID Module (MFRC522)
| RFID Pin | ESP32 GPIO | Color Code (Suggested) | Function |
| :--- | :--- | :--- | :--- |
| **SDA (SS)** | **GPIO 5** | White | Serial Data Line |
| **SCK** | **GPIO 18** | Yellow | Serial Clock |
| **MOSI** | **GPIO 23** | Blue | Master Out Slave In |
| **MISO** | **GPIO 19** | Green | Master In Slave Out |
| **IRQ** | *Not Used* | - | Interrupt Request |
| **GND** | **GND** | Black | Ground |
| **RST** | **GPIO 4** | Orange | Reset |
| **3.3V** | **3.3V** | Red | Power (DO NOT USE 5V) |

### 2. Ultrasonic Sensor (HC-SR04)
| Sensor Pin | ESP32 GPIO | Function |
| :--- | :--- | :--- |
| **VCC** | **VIN (5V)** | Power |
| **Trig** | **GPIO 13** | Trigger Pulse |
| **Echo** | **GPIO 12** | Echo Return (Use 1k/2k Resistor Divider) |
| **GND** | **GND** | Ground |

### 3. Alarm & Interface
| Component | ESP32 GPIO | Note |
| :--- | :--- | :--- |
| **Buzzer (+)** | **GPIO 14** | Active Buzzer (LOW = Sound) |
| **Buzzer (-)** | **GND** | Ground |
| **Button Pin 1**| **GPIO 32** | Input (Internal Pullup Used) |
| **Button Pin 2**| **GND** | Ground |

## Logic Flow (Pseudocode)

```text
START PROGRAM
    INITIALIZE RFID, Bluetooth, and Sensors
    LOOP FOREVER
        IF Button is Pressed:
            ENABLE Bluetooth Leash Scan
            IF Target Device RSSI < -55: TRIGGER Alarm
        IF RFID Tag Scanned:
            IF Tag is Authorized: TOGGLE Arm/Disarm State
        IF System is Armed:
            MEASURE Internal Bag Distance
            IF Distance > 15cm: TRIGGER Alarm
    ENDLOOP
END PROGRAM
```

## Installation & Setup

1.  **Software Prep:**
    * Install **Arduino IDE**.
    * Add ESP32 Board Support via `File > Preferences > Boards Manager URLs`.
    * Install the `MFRC522` library via the Library Manager.
2.  **Hardware Assembly:**
    * Connect the RFID via SPI as mapped in the detailed table.
    * Connect the Button between **GPIO 32** and **GND**.
3.  **Code Customization:**
    * Ensure the `str1` and `str2` variables match your phone's or earbuds' exact Bluetooth names.
4.  **Upload:**
    * Click Upload and open the Serial Monitor (115200 baud).

## Security Note

* **Buzzer Logic:** The system uses an Active Buzzer logic where LOW triggers the sound and HIGH silences it. Ensure your wiring matches this logic to avoid continuous beeping.

## Author
**Ashvin**
