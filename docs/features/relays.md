---
title: Built-in Relays
description:
  How Espcontrol exposes the built-in relays on the Guition 4848S040C relay variant in Home Assistant.
---

# Built-in Relays

Some Guition 4-inch 4848S040 panels are sold as a relay variant, often listed as **4848S040C**. These boards include three physical relays that can switch external low-voltage circuits.

The relays have been confirmed working on the relay variant using the standard 4848S040 Espcontrol firmware.

## Home Assistant Entities

On the 4848S040 firmware, Espcontrol exposes each relay in two ways:

- **Relay switches** stay on until you turn them off again.
- **Relay push buttons** turn the relay on for 200 ms, then turn it off again.

| Relay | Switch entity | Push button entity | Relay pin |
|---|---|---|---|
| **Relay 1** | `switch.<device_name>_relay_1` | `button.<device_name>_relay_1_push` | GPIO40 |
| **Relay 2** | `switch.<device_name>_relay_2` | `button.<device_name>_relay_2_push` | GPIO2 |
| **Relay 3** | `switch.<device_name>_relay_3` | `button.<device_name>_relay_3_push` | GPIO1 |

They appear as normal Home Assistant entities on the Espcontrol device. You can control them from Home Assistant dashboards, automations, scripts, and voice assistants.

![Home Assistant controls card showing Display Backlight plus Relay 1, Relay 2, and Relay 3 switches](/images/relay-controls.svg)

Home Assistant may adjust the exact entity ID if you have renamed the device or if another entity already used the same name. To find them, open **Settings > Devices & services**, select your Espcontrol device, then look for the relay switches and relay push buttons under the device entities.

## Switches vs Push Buttons

Each relay has both a switch and a push button version because they suit different wiring jobs.

| Entity type | What it does | Typical uses |
|---|---|---|
| **Relay switch** | Turns the relay on and leaves it on until you turn it off. | Keeping a low-voltage light, LED strip, fan input, buzzer, or other simple circuit on for a while. |
| **Relay push button** | Pulses the relay on for 200 ms, then turns it off automatically. | Mimicking a quick press on a garage door input, gate opener, doorbell, dimmer, lighting controller, scene controller, or any device that expects a momentary button press. |

Use the switch version when the relay should represent an ongoing on/off state. Use the push button version when the connected device only needs a short signal and then takes care of the action itself.

## Using Relays on the Touchscreen

Relay controls on the touchscreen still work through Home Assistant. The panel does not call the relay hardware locally when you tap a screen button. Instead, it sends a Home Assistant action for the entity you configured, and Home Assistant sends that command back to the Espcontrol device.

That means Home Assistant actions must be set up, and Home Assistant must be connected, for touchscreen relay controls to work.

The relay entities are exposed to Home Assistant, so you can add them onto the Espcontrol touchscreen using the normal entity setup:

1. Open the Espcontrol setup page in your browser.
2. Choose an empty button slot.
3. Leave the type as **Switch** for the normal entity-control setup.
4. Set the entity to a relay switch entity, such as `switch.kitchen_panel_relay_1`, or a relay push button entity, such as `button.kitchen_panel_relay_1_push`.
5. Save the button configuration.

Tapping a relay switch entity asks Home Assistant to toggle the relay. Tapping a relay push button entity asks Home Assistant to press that entity, which briefly pulses the relay instead.

The push button entities are useful when the relay is wired in parallel with existing momentary wall switches, for example when it is driving a dimmer or lighting controller that expects a short button press rather than a permanent on/off output.

## Hardware Detection

The relays are controlled directly by GPIO pins. That means there is no separate relay controller chip for the firmware to identify, so Espcontrol cannot reliably auto-detect whether a specific physical board has the relay hardware fitted.

If your board is the relay variant, the entities will control the built-in relays. If your board does not have the relay hardware, the same entities may still appear in Home Assistant but will not switch a physical relay.

## Startup Behaviour

The relay outputs default to **off** after startup or restart. This helps avoid a relay turning on unexpectedly when the panel reboots.

If you need a relay to return to a particular state after restart, handle that with a Home Assistant automation once the Espcontrol device reconnects.

## Testing

After updating the firmware, test each relay from Home Assistant before connecting it to anything important. You should hear or feel each relay click when it changes state.

## Safety

Check the relay rating printed on your board or supplied by the seller before connecting anything. Do not use the relays for mains voltage unless your specific relay board, wiring, enclosure, and local electrical rules make that safe.
