

# Brick Embedded Inspection Device  / Apache 2.0 License

> Rugged, low-cost, next-generation asset inspection platform  
> **Author:** Alejandro Vazquez  
> **First commit:** [Commit 8c4c109f, 10/18/2025 11:30 AM] (see Git log for full provenance)

---

## Overview

Brick is a handheld inspection device designed for industrial and fleet asset management. Built from the ground up to be robust, affordable, and cloud-connected, Brick offers:

- Real-time inspection forms
- Asset tracking via RFID
- Intuitive touch UI (LVGL 8.3)
- Cloud sync and secure communication
- Rapid hardware startup and stable operation

**Target:** STM32 / Arduino GIGA R1 WiFi  
**Enclosure:** Custom (Serpac H75)  
**UI:** LVGL, Arduino_H7_Video

---

## Features

- Asset and inspection management (on device)
- Modular architecture for adding zones, defects, layouts
- RFID support via MFRC522
- Real-time clock integration
- Low power/fast boot
- Stable display and touch per [GIGA Embedded Startup Pattern](./giga_startup_best_practice.pdf)

---

## Getting Started

1. Clone this repo
2. PlatformIO install (see `platformio.ini`)
3. Wire according to `/docs/wiring.pdf`
4. Build and flash (`pio run -t upload`)
5. Connect via USB/Serial to monitor

---

## Provenance & License

- All code, documentation, and hardware integration in this repo was developed by Alejandro Vazquez, with commit history as provenance.
- No source code or IP from any previous employer was used.
- Commit history serves as the canonical proof of origin.
- License: Not licensed
- For IP or licensing inquiries, contact [alejandrovazquez@yahoo.com].

---

## Documentation

- [Startup Best Practices](./giga_startup_best_practice.pdf)
- [Pinout and Schematics](./ABX00063-full-pinout.pdf)
- [Business Case for Scaling](./Arduino_GIGA_Scaling_Business_Case.pdf)

---

## Credits

Alejandro Vazquez — Design, code, hardware, and documentation
