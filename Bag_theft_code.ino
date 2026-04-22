#include <SPI.h>
#include <MFRC522.h>
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

// Pins
#define SS_PIN    5
#define RST_PIN   4
#define TRIG_PIN  13
#define ECHO_PIN  12
#define BUZZER_PIN 14
#define BUTTON_PIN 32

MFRC522 mfrc522(SS_PIN, RST_PIN);

// Your 3 Authorized Tags
byte authorizedUIDs[3][4] = {
  {0x82, 0x2E, 0x44, 0x51}, // Tag 1
  {0x73, 0x22, 0x3A, 0x29}, // Tag 2
  {0xD3, 0x47, 0x4D, 0xF7}  // Tag 3
};

bool isArmed = false;
bool dist_alarm=false;
int threshold = 15; // Trigger alarm if object is further than 15cm
bool moved=false;

void setup() {
  Serial.begin(115200);
  SPI.begin();
  SerialBT.begin("ESP32_Bag_Alarm");
  mfrc522.PCD_Init();
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  Serial.println("Bag Security System Ready.");
  digitalWrite(BUZZER_PIN,HIGH);

  
}

void loop() {
  int buttonState = digitalRead(BUTTON_PIN);
  
  if (buttonState == LOW) {
    dist_alarm=true;
    Serial.println("Touch Detected! State: HIGH");
    delay(200); // Small debounce
  }
  else
  {
    dist_alarm=false;
    Serial.println("Touch Detected! State: LOW");
    if(!isArmed)
    {
      digitalWrite(BUZZER_PIN, HIGH);
    }
  }
  
  if(dist_alarm)
  {
    // Scan for 5 seconds
    BTScanResults* pResults = SerialBT.discover(1500);
    if (pResults) {
    int count = pResults->getCount();
    Serial.printf("Found %d devices\n", count);
    
    for (int i = 0; i < count; i++) {
      BTAddress addr = pResults->getDevice(i)->getAddress();
      String name = pResults->getDevice(i)->getName().c_str();
      int rssi = pResults->getDevice(i)->getRSSI();
      String str1="Galaxy M13";
      String str2="JBL TUNE215TWS";
      if(str1==name || str2==name )
      {
        Serial.print("Device Name: "); Serial.println(name.length() > 0 ? name : "Unknown");
        Serial.print("Address: "); Serial.println(addr.toString().c_str());
        Serial.print("Signal Strength (RSSI): "); Serial.println(rssi);
        if(rssi<-55)
        {
          Serial.println("Too far");
          digitalWrite(BUZZER_PIN, LOW); // Constant siren
          Serial.println("ALARM! BAG MOVED!");
          moved=true;
        }
        else {
          if(!isArmed)
          {
            digitalWrite(BUZZER_PIN, HIGH);
          }
          moved=false;
        }
      
      Serial.println("-----------------------");
      }
    }  
    }
    else {
      Serial.println("No devices found.");
    }
    delay(2000);
  }

  // 1. Check for RFID Scan
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    bool matchFound = false;
    
    for (int tagIndex = 0; tagIndex < 3; tagIndex++) {
      bool currentMatch = true;
      for (byte i = 0; i < 4; i++) {
        if (mfrc522.uid.uidByte[i] != authorizedUIDs[tagIndex][i]) {
          currentMatch = false;
          break;
        }
      }
      if (currentMatch) { matchFound = true; break; }
    }

    if (matchFound) {
      isArmed = !isArmed;
      digitalWrite(BUZZER_PIN, HIGH); // Stop alarm on disarm
      
      // Feedback Beeps
      if (isArmed) {
        // One high beep for Armed
        digitalWrite(BUZZER_PIN,LOW);
        delay(200);
        digitalWrite(BUZZER_PIN,HIGH);

      } else {
        digitalWrite(BUZZER_PIN,LOW);
        delay(200);
        digitalWrite(BUZZER_PIN,HIGH);
        delay(200);
        digitalWrite(BUZZER_PIN,LOW);
        delay(200);
        digitalWrite(BUZZER_PIN,HIGH);
      }
      
      Serial.print("Access Granted. Status: ");
      Serial.println(isArmed ? "ARMED" : "DISARMED");
      delay(1500); 
    }
    mfrc522.PICC_HaltA();
  }

  // 2. If Armed, check for "Theft" (Object removal)
  if (isArmed) {
    long duration, distance;
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    duration = pulseIn(ECHO_PIN, HIGH);
    distance = (duration / 2) / 29.1;

    Serial.print("Distance:");
    Serial.println(distance);

    if (distance > threshold || distance == 0) { // distance 0 often means "out of range"
      digitalWrite(BUZZER_PIN, LOW); // Constant siren
      Serial.println("ALARM! BAG OPENED!");
    } else {
      if(!moved)
      {
        digitalWrite(BUZZER_PIN, HIGH);
      }
    }
  }
}