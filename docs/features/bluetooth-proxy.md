---
title: Bluetooth Proxy
description:
  How to use EspControl as an optional Home Assistant Bluetooth proxy for nearby Bluetooth Low Energy devices.
---

# Bluetooth Proxy

EspControl can act as a Home Assistant Bluetooth proxy for nearby Bluetooth Low Energy devices, such as many battery sensors, buttons, locks, and trackers.

The setting is off by default and is currently behind the developer features flag. Open the setup page with `?developer=experimental`, enable **Developer/Experimental Features** in **Settings > Developer**, then go to **Settings > Bluetooth** and turn on **Bluetooth Proxy**. The panel starts Bluetooth after the setting is enabled and keeps it enabled after restarts.

When Bluetooth Proxy or Developer/Experimental Features is off, the Bluetooth stack stays disabled on boot.

::: warning Memory and range
Bluetooth support uses extra memory and shares radio time with WiFi on WiFi panels. If a panel becomes unstable after enabling it, turn the setting off and restart the panel.
:::

## Notes

- Only Bluetooth Low Energy devices are supported.
- Home Assistant must be connected through the ESPHome integration.
- The advanced Ethernet-only firmware for the 7-inch JC1060P470 keeps the ESP32-C6 wireless co-processor off, so Bluetooth proxy is not available in that firmware variant.
