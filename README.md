# PhantomKey
👻 PhantomKey – A stealthy USB HID device to auto-bypass BIOS errors like "Chassis Fan Not Detected".

# 👻 PhantomKey

**Pressing Enter from beyond the BIOS...**

PhantomKey is a tiny USB gadget based on the Seeed XIAO SAMD21 that spoofs a keyboard to automatically bypass BIOS errors like "Chassis Fan Not Detected" by pressing Enter at just the right time — completely hands-free.

---

## 🧠 Problem Statement

Some motherboards throw a fit if a chassis fan isn’t connected — halting boot with an annoying prompt:  
> "**CPU Fan Error! Press Enter to continue.**"

This is fine for one reboot. But if you're running a headless server? Nightmare.

Enter **PhantomKey**. Plug it into any USB port and let it silently spoof an `Enter` keypress a few seconds after power-on. Then it disconnects and goes to sleep. No fuss. No BIOS drama.

---

## ⚙️ Features

- ⌨️ USB HID keyboard emulation
- ⏱️ Timed Enter presses
- 💡 LED indicators for each stage
- 💤 Detaches from USB and sleeps afterward
- 🌈 Super tiny (fits in the XIAO’s footprint)
- 🔒 Open Source & CC0 licensed

---

## 🛠️ Hardware Used

- [Seeed Studio XIAO SAMD21](https://wiki.seeedstudio.com/XIAO_SAMD21/)
- 3x LEDs (TX, RX, Status)
- Optional: USB extension cable

---

## 🧪 How It Works

1. Wait 10s → press Enter.
2. Wait 5s → press Enter again (some BIOSes double-check).
3. Wait 30s → disconnect USB, turn off LEDs.
4. Enter deep sleep mode.

No user input required.

---

## 🚀 Getting Started

1. Flash `PhantomKey.ino` to the XIAO SAMD21.
2. Plug it into your motherboard’s USB port.
3. Boot the machine.
4. Let PhantomKey handle the rest!

---

## 🪪 License

This project is licensed under [CC0 1.0 Universal](https://creativecommons.org/publicdomain/zero/1.0/).  
Use it freely. No attribution needed. No strings attached.

![CC0](https://licensebuttons.net/p/zero/1.0/88x31.png)

---

## 📸 Screenshots

<p align="center">
  <img src="img/20250407_212854.JPG" width="400">
  <img src="img/20250408_140839.JPG" width="400">
</p>
