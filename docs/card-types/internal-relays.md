---
title: Internal Relays
description:
  How to control built-in panel relays locally from Espcontrol cards without relying on Home Assistant.
---

# Internal Relays

Internal Relay cards control the panel's built-in relay hardware directly from the touchscreen. They do not send a command through Home Assistant, so they can still work when Home Assistant is offline.

This card type appears on devices that define built-in relays, such as the 4848S040 relay variant.

## Setting Up an Internal Relay

1. Select a card and change its type to **Internal**.
2. Set a **Label**.
3. Choose the **Internal Relay** to control.
4. Choose a **Mode**.
5. Choose the icon fields for that mode.

## Modes

**Switch** mode behaves like a normal switch card. Tapping it turns the selected relay on or off, and the card shows the current relay state.

Switch mode shows two icon fields:

- **On Icon** defaults to **Lightbulb**.
- **Off Icon** defaults to **Lightbulb Outline**.

**Push Button** mode behaves like a trigger card. Tapping it pulses the selected relay for 200 ms, then turns it off again.

Push Button mode shows one **Icon** field, because it does not have a separate on/off state.

## When to Use It

Use Internal Relay cards when the panel itself should operate the connected circuit, even during a Home Assistant outage. Use normal Switch cards with Home Assistant relay entities when you specifically want the command to go through Home Assistant automations or permissions.
