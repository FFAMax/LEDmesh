# 📦 BIM — Precompiled Binaries for Google Wi-Fi

This folder contains precompiled binaries built specifically for **Google Wi-Fi devices**. These are the round, white units with a single **USB Type-C power connector**.

All binaries were built using the OpenWrt SDK for the `ipq40xx/chromium` target and are intended to run directly on stock or OpenWrt-modified devices without additional dependencies.

## 🔧 Notes

- Architecture: `arm_cortex-a7+neon-vfpv4`
- Toolchain: `OpenWrt 23.05.5 SDK`
- Format: statically linked ELF binaries (musl)

## 📂 Included

- `led_daemon` — listens for UDP packets and controls the onboard SATA LEDs (Red/Green/Blue)
- `led_control` — simple client to send color commands
- `led_benchmark` — test utility with demo modes (color transitions, blink patterns)

## ⚠️ Disclaimer

These binaries are provided **as-is**, with no warranty. They are meant for development, experimentation, and controlled deployment in trusted environments.

## 📎 For source code and build instructions, see the main [README.md](../README.md).
