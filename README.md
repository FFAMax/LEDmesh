# LEDmesh: Synchronized LED Control over Broadcast for OpenWrt Devices

**LEDmesh** is a minimalistic, broadcast-based LED synchronization system for OpenWrt-compatible devices (e.g. Google Wi-Fi, IPQ40xx-based routers) with controllable multicolor LEDs. It enables control of LED clusters using a single UDP packet, with zero cloud, zero middleware, and near-zero latency.

This project demonstrates a **bare-metal approach to LED orchestration** in local wired networks using standard Linux facilities and OpenWrt’s sysfs interface.

## 🔦 Use Case

Repurposed routers and IoT hardware (such as Google Wi-Fi units) are often equipped with three-color LEDs and robust hardware specs (RAM, CPU, networking). This project showcases how such devices can be clustered and visually coordinated — turning a roomful of idle devices into a dynamic LED wall, notification system, or network visualization tool.

## 💡 Key Concepts

- No cloud: everything works over LAN via UDP broadcast  
- No daemon dependencies: pure POSIX C, tiny binary  
- Real-time: LED updates take effect on all devices almost instantly  
- Uniform: all devices react in perfect sync to the same network packet

## 🧠 Architecture

### Receiver (`led_daemon`)
- Runs on OpenWrt (ARMv7, musl).
- Listens on a UDP port (default: `9999`).
- Parses simple 4-byte packets:
  - `0x4C` — magic byte
  - 3 bytes for Red, Green, Blue (0–255)

It applies brightness values via:

```
/sys/class/leds/LED0_Red/brightness  
/sys/class/leds/LED0_Green/brightness  
/sys/class/leds/LED0_Blue/brightness  
```

### Sender: `led_benchmark` & `led_send`
- A command-line tool for sending demo sequences (rainbow, blinking, single-color pulses).
- Runs on macOS or Linux.
- Sends broadcast packets to `255.255.255.255:9999`.

## 🚀 Quick Start

### 1. Build daemon for OpenWrt (from host)

```bash
arm-openwrt-linux-muslgnueabi-gcc led_daemon.c -o led_daemon
scp led_daemon root@192.168.1.1:/tmp/
```

### 2. Run on OpenWrt device

```bash
chmod +x /tmp/led_daemon
/tmp/led_daemon
```

### 3. Build and run sender on Mac/Linux

```bash
gcc led_benchmark.c -o led_benchmark -lm
./led_benchmark
```

## 🌈 Demo Features

- LED cycling: each LED blinks sequentially  
- RGB blinking: all channels flash in sync  
- Rainbow gradient: smooth HSV-to-RGB transition, synchronized across all devices  

## 📦 Files

```
.
├── led_daemon.c        # C daemon for OpenWrt that listens for LED control packets
├── led_send.c          # Minimal packet sender
├── led_benchmark.c     # Broadcast-controlled rainbow & blinking test
├── README.md           # This file
```

## 🔐 Security

This project is meant for trusted local networks. No encryption/authentication is included by design.

## ⚠️ Requirements

- OpenWrt 23.05+ (musl, ARMv7 target)
- `LED0_Red`, `LED0_Green`, `LED0_Blue` exposed via `/sys/class/leds/`
- UDP broadcast allowed on the LAN

## 🧩 Ideas for Extension

- MQTT gateway for home automation  
- GUI controller (macOS app, web app)  
- Per-device addressability via extended packets  
- LED animations, music sync, notifications, etc.

## 🧪 How It Was Made

Created under the seasoned guidance of a network engineer, with the questionable help of GPT, countless software versions, frustrated swearing, low-level debugging, and stubborn experimentation. A testament to perseverance and slightly unhinged curiosity.

## 📜 License

MIT

---

Made with 💻, 🛠️, and ⚡ by repurposing Google Wi-Fi hardware.

# Misc

## 📎 QuickHowTo: Cross-compilation for OpenWrt (IPQ40xx / Google Wi-Fi)

To compile the `led_daemon` for Google Wi-Fi or other OpenWrt-compatible IPQ40xx hardware, use the official OpenWrt SDK:

### 🛠️ 1. Download & extract SDK

```bash
wget https://downloads.openwrt.org/releases/23.05.5/targets/ipq40xx/chromium/openwrt-sdk-23.05.5-ipq40xx-chromium_gcc-12.3.0_musl_eabi.Linux-x86_64.tar.xz
tar xf openwrt-sdk-23.05.5-ipq40xx-chromium_gcc-12.3.0_musl_eabi.Linux-x86_64.tar.xz
cd openwrt-sdk-23.05.5-ipq40xx-chromium_gcc-12.3.0_musl_eabi.Linux-x86_64
```

### ⚙️ 2. Set up toolchain environment

```bash
export STAGING_DIR="$PWD/staging_dir"
export TOOLCHAIN="$STAGING_DIR/toolchain-arm_cortex-a7+neon-vfpv4_gcc-12.3.0_musl_eabi"
export PATH="$TOOLCHAIN/bin:$PATH"
```

### 🧪 3. Compile `led_daemon`

```bash
arm-openwrt-linux-muslgnueabi-gcc led_daemon.c -o led_daemon
```

You can then transfer the binary to your OpenWrt device via `scp` and run it.

### 📘 Reference

OpenWrt SDK used:
**openwrt-sdk-23.05.5-ipq40xx-chromium_gcc-12.3.0_musl_eabi**

Download:  
https://downloads.openwrt.org/releases/23.05.5/targets/ipq40xx/chromium/

---

## Update: `control_hsv` Utility

A new utility `control_hsv` has been added to the project. This is a standalone CLI tool for sending LED color control packets using HSV (Hue, Saturation, Brightness) input parameters.

### Features:
- Accepts HSV values from the command line:
  - `--hue` (0–360 degrees)
  - `--sat` (0–100 percent)
  - `--brightness` (0–100 percent)
- Internally converts HSV to RGB and sends the appropriate UDP broadcast packet to the LED controller.
- Includes `--demo` mode, which runs a continuous HSV hue sweep (rainbow effect) for testing or demonstration purposes.

### Examples:
```bash
gcc control_hsv.c -o control_hsv -lm
./control_hsv --hue 120 --sat 80 --brightness 60
./control_hsv --demo
```

---


## Update: Sunrise 

## 🌅 Sunrise Simulation

This repository includes two shell scripts designed to simulate the effect of a sunrise using controllable RGB LEDs on Google Wi-Fi devices.

### `sunrise.sh`

This script is meant to be launched at system boot or via a cron job. It reacts to real-world time and simulates the early morning sky colors:

- **05:35 – 05:45**: LEDs gradually increase brightness from 0% to 70% with a cool blue hue (`hue = 210`, `saturation = 100%`).
- **05:45 – 05:55**: The color transitions from bluish white to full white by:
  - Decreasing hue from 210° to ~60°
  - Reducing saturation from 100% to 10%
  - Increasing brightness from 70% to 100%

The result is a smooth fade from pre-dawn blue into a bright white, mimicking sunrise light conditions.

To enable it automatically at boot or every minute, you can set up a cron job on OpenWrt:

```bash
# Edit crontab
crontab -e
```

Add the following line:

```cron
* * * * * /root/sunrise.sh
```
Or run manually in loop
```
while true; do bash sunrise.sh ; sleep 60; done
```

Make sure the script is executable:

```bash
chmod +x /root/sunrise.sh
```

### `sunrise_demo.sh`

This is a **demo version** of the above logic.

Instead of using the current time, it emulates the full 20-minute sunrise cycle in real time — every minute is played back in **1 second**.

This script is ideal for testing the lighting behavior without needing to wait for early morning hours.

```bash
./sunrise_demo.sh
```

Each second it prints the emulated time and sends corresponding HSV values to the `control_hsv` binary.

### Requirements

Both scripts require the `control_hsv` utility to be compiled and available in the same folder or in your `$PATH`.


---

