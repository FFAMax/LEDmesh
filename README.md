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
