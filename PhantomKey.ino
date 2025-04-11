#include "Adafruit_TinyUSB.h"
#include "Arduino.h"

// Pin Definitions
#define LED_TX 11       // Blue 1
#define LED_RX 12       // Blue 2
#define LED_ORANGE 13   // Status LED

Adafruit_USBD_HID usb_hid;
uint8_t keycode[6] = { 0 };

uint8_t desc_hid_report[] = {
  TUD_HID_REPORT_DESC_KEYBOARD()
};

unsigned long orangeLastBlink = 0;
bool ejected = false;
unsigned long secondEnterTime = 0;
bool stageOneDone = false;
bool stageTwoDone = false;
bool sleeping = false;

void setup() {
  pinMode(LED_TX, OUTPUT);
  pinMode(LED_RX, OUTPUT);
  pinMode(LED_ORANGE, OUTPUT);

  digitalWrite(LED_TX, HIGH);      // OFF (inverted logic)
  digitalWrite(LED_RX, HIGH);
  digitalWrite(LED_ORANGE, HIGH);

  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usb_hid.setBootProtocol(HID_ITF_PROTOCOL_KEYBOARD);
  usb_hid.begin();

  while (!TinyUSBDevice.mounted()) {
    digitalWrite(LED_ORANGE, LOW);
    delay(200);
    digitalWrite(LED_ORANGE, HIGH);
    delay(200);
  }
}

void loop() {
  if (!sleeping) blinkOrangeEverySecond();

  unsigned long now = millis();

  if (!stageOneDone && now >= 10000) {
    sendEnterKey();
    digitalWrite(LED_TX, LOW);   // Blue 1 ON
    stageOneDone = true;
  }

  if (!stageTwoDone && now >= 15000) {
    sendEnterKey();
    digitalWrite(LED_RX, LOW);   // Blue 2 ON
    secondEnterTime = now;
    stageTwoDone = true;
  }

  if (stageTwoDone && !ejected && now - secondEnterTime >= 30000) {
    // Turn off all LEDs
    digitalWrite(LED_TX, HIGH);
    digitalWrite(LED_RX, HIGH);
    digitalWrite(LED_ORANGE, HIGH);

    TinyUSBDevice.detach();  // Eject
    ejected = true;

    delay(5000);             // Wait 5 seconds before sleep
    goToSleep();
  }
}

void sendEnterKey() {
  keycode[0] = HID_KEY_ENTER;
  usb_hid.keyboardReport(0, 0, keycode);
  delay(10);
  usb_hid.keyboardRelease(0);
}

void blinkOrangeEverySecond() {
  if (millis() - orangeLastBlink >= 1000 && !ejected) {
    digitalWrite(LED_ORANGE, !digitalRead(LED_ORANGE)); // Toggle
    orangeLastBlink = millis();
  }
}

void goToSleep() {
  sleeping = true;
  // Optional: power-saving pins, etc.
  __WFI(); // Wait For Interrupt — puts ARM core into low power
  while (1); // Deep stop
}
