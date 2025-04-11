#include "Adafruit_TinyUSB.h"
#include "Arduino.h"

// ================== Configuration ==================
#define LED_TX 11       // Blue 1
#define LED_RX 12       // Blue 2
#define LED_ORANGE 13   // Status LED

#define ENTER_DELAY_1 10000UL     // First ENTER at 10s
#define ENTER_DELAY_2 15000UL     // Second ENTER at 15s
#define EJECT_DELAY   30000UL     // Eject after 30s from second ENTER
#define FAILSAFE_TIMEOUT 20000UL  // Give up waiting for USB after 20s

#define FAST_BLINK_INTERVAL 100   // 100ms for fast blink
#define SLEEP_BLINK_COUNT   3
#define EJECT_BLINK_COUNT   2
// ===================================================

Adafruit_USBD_HID usb_hid;
uint8_t keycode[6] = { 0 };

uint8_t desc_hid_report[] = {
  TUD_HID_REPORT_DESC_KEYBOARD()
};

unsigned long startTime = 0;
unsigned long orangeLastBlink = 0;
bool orangeState = false;

bool ejected = false;
unsigned long secondEnterTime = 0;
bool stageOneDone = false;
bool stageTwoDone = false;
bool sleeping = false;

void setup() {
  pinMode(LED_TX, OUTPUT);
  pinMode(LED_RX, OUTPUT);
  pinMode(LED_ORANGE, OUTPUT);

  digitalWrite(LED_TX, HIGH);      // LEDs OFF (inverted logic)
  digitalWrite(LED_RX, HIGH);
  digitalWrite(LED_ORANGE, HIGH);

  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usb_hid.setBootProtocol(HID_ITF_PROTOCOL_KEYBOARD);
  usb_hid.begin();

  startTime = millis();

  // Non-blocking USB mount wait with timeout
  while (!TinyUSBDevice.mounted()) {
    if (millis() - startTime >= FAILSAFE_TIMEOUT) {
      // Fail-safe: give up waiting
      break;
    }

    if (millis() - orangeLastBlink >= 500) {
      digitalWrite(LED_ORANGE, !digitalRead(LED_ORANGE)); // Toggle
      orangeLastBlink = millis();
    }
  }

  digitalWrite(LED_ORANGE, HIGH); // Make sure LED starts off
}

void loop() {
  if (!sleeping) blinkOrangeEverySecond();

  unsigned long now = millis();

  if (!stageOneDone && now >= ENTER_DELAY_1) {
    sendEnterKey();
    digitalWrite(LED_TX, LOW);   // Blue 1 ON
    stageOneDone = true;
  }

  if (!stageTwoDone && now >= ENTER_DELAY_2) {
    sendEnterKey();
    digitalWrite(LED_RX, LOW);   // Blue 2 ON
    secondEnterTime = now;
    stageTwoDone = true;
  }

  if (stageTwoDone && !ejected && now - secondEnterTime >= EJECT_DELAY) {
    // Fast blink x2 before eject
    fastBlinkOrange(EJECT_BLINK_COUNT);

    // Gracefully stop HID and detach USB
    usb_hid.end();
    TinyUSBDevice.detach();
    ejected = true;

    // Turn off all LEDs
    digitalWrite(LED_TX, HIGH);
    digitalWrite(LED_RX, HIGH);
    digitalWrite(LED_ORANGE, HIGH);

    delay(5000); // Delay before sleep

    // Fast blink x3 before going to sleep
    fastBlinkOrange(SLEEP_BLINK_COUNT);

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
  if (!ejected && millis() - orangeLastBlink >= 1000) {
    digitalWrite(LED_ORANGE, !digitalRead(LED_ORANGE));
    orangeLastBlink = millis();
  }
}

void fastBlinkOrange(uint8_t times) {
  for (uint8_t i = 0; i < times * 2; i++) {
    digitalWrite(LED_ORANGE, LOW);
    delay(FAST_BLINK_INTERVAL);
    digitalWrite(LED_ORANGE, HIGH);
    delay(FAST_BLINK_INTERVAL);
  }
}

void goToSleep() {
  sleeping = true;

  // Optionally set all pins to input for low power
  pinMode(LED_TX, INPUT);
  pinMode(LED_RX, INPUT);
  pinMode(LED_ORANGE, INPUT);

  __WFI(); // Wait For Interrupt — enter low-power mode
  while (1); // Deep stop
}
