# Camera Card Documentation

## Overview

The camera card displays live snapshots from Home Assistant camera entities directly on the ESP device display. It works with **any** camera integration (Frigate, RTSP, IP cameras, etc.) by fetching snapshots from Home Assistant's `/api/camera_proxy/` endpoint.

## Features

- 📸 **Live snapshots** from any HA camera entity
- 🔄 **Configurable refresh intervals** (1-300 seconds, or manual only)
- 📐 **Aspect ratio control** (Auto, 16:9, 4:3, 1:1, 21:9)
- 🎨 **Scaling modes** (Fit - preserve aspect, Fill - crop to fit)
- 🌐 **Automatic HA host detection** from internal_url
- 💾 **Memory efficient** snapshot buffering

## Setup

### 1. Enable in Device Configuration

Add the camera addon to your device's `packages.yaml`:

```yaml
packages:
  # ... existing packages ...
  camera_snapshots: !include ../../common/addon/camera_snapshots.yaml
```

### 2. Configure in Web UI

1. Open the Settings tab on your ESP device web interface
2. Add a new button and select **Camera** type
3. Configure:
   - **Camera Entity**: e.g., `camera.front_door` (must be accessible via HA API)
   - **Label**: Display name (e.g., "Front Door")
   - **Icon**: Camera or custom icon
   - **Refresh Interval**: Seconds between fetches (0 = manual only, blank = no auto-refresh)
   - **Aspect Ratio**: Select appropriate ratio for your camera
   - **Scaling**: Fit (preserve aspect) or Fill (crop to fit button)

### 3. Enable Internal HA Access

Camera snapshots are fetched from Home Assistant. Ensure the device can reach HA:

- Device and HA must be on same network
- HA's `internal_url` must be accessible (typically set automatically)
- Default port is 8123 (override in addon if needed)

## How It Works

### Snapshot Fetching

1. **Interval timer** (5-second check):
   - Evaluates each camera card's refresh interval
   - If interval elapsed, queues HTTP request

2. **HTTP request**:
   - Fetches `http://ha-host:8123/api/camera_proxy/camera.entity_id`
   - Receives raw JPEG snapshot

3. **JPEG decode**:
   - Processes JPEG data
   - Extracts pixel buffer

4. **Display update**:
   - Renders decoded image on button
   - Replaces icon placeholder

### Refresh Behavior

| Refresh Interval | Behavior |
|---|---|
| 0 (blank) | Manual only - displays static snapshot from last HA state change |
| 1-300 | Automatic refresh every N seconds |

### Memory Management

- Each camera card maintains a snapshot buffer (~100KB typical for 320×240 JPEG)
- Decoded pixels stored in PSRAM during display
- Old snapshots released when new ones arrive
- Maximum 25 cameras (limited by MAX_GRID_SLOTS)

## Examples

### Example 1: Doorbell Camera (Manual Refresh)

```
Camera Entity: camera.front_door_snapshot
Label: Front Door
Refresh Interval: (leave blank - manual only)
Aspect Ratio: 16:9
Scaling: Fit
```

### Example 2: Frigate Event Detection (Auto-Refresh)

```
Camera Entity: camera.frigate_front_door
Label: Front Door
Refresh Interval: 5
Aspect Ratio: 4:3
Scaling: Fill
```

For Frigate integration:
1. Add Frigate integration to Home Assistant
2. This creates `camera.frigate_*` entities for each camera
3. Card will display latest snapshot with 5-second refresh

### Example 3: IP Camera via RTSP (Fast Updates)

```
Camera Entity: camera.backyard
Label: Backyard
Refresh Interval: 2
Aspect Ratio: Auto
Scaling: Fit
```

## Troubleshooting

### Snapshots Not Displaying

1. **Check HA host detection**:
   - Device logs: `adb logcat | grep camera_grid`
   - Should show: `Updated HA host: 192.168.x.x`

2. **Verify camera entity**:
   - In HA, go to Developer Tools → States
   - Search for `camera.your_entity_id`
   - Should exist and have `state: idle` or `state: recording`

3. **Check HTTP access**:
   - From ESP device terminal: `curl http://ha-host:8123/api/camera_proxy/camera.entity_id`
   - Should return JPEG data (binary)

### Crashes or Resets

- **PSRAM exhaustion**: Reduce number of cameras or lower resolution snapshots
- **Memory leak**: Check logs for repeated "Failed to decode" errors
- **Network timeout**: Increase refresh interval if HA is slow

### Poor Image Quality

- **Aspect ratio mismatch**: Select correct ratio for camera FOV
- **Scaling wrong**: Try "Fill" if image looks letterboxed
- **JPEG compression**: Camera compression set in HA (not controllable from ESP)

## Advanced Configuration

### Custom HA Port

Edit `common/addon/camera_snapshots.yaml` and change:
```yaml
globals:
  - id: camera_ha_port
    initial_value: "8123"  # Change this
```

### Manual Snapshot Request

From a Home Assistant automation or script:
```yaml
service: esphome.your_device_push_button_pressed
data:
  entity_id: button.camera_refresh
```

## Limitations

- Maximum concurrent HTTP request: 1 (queued if multiple cameras refresh simultaneously)
- Maximum snapshot size: ~1MB (PSRAM constraint)
- JPEG only (most camera entities provide this)
- Refresh check interval: 5 seconds (minimum practical latency ~5 seconds)

## Technical Details

### Data Flow

```
Home Assistant Camera Entity (camera.front_door)
    ↓
HTTP Request: GET /api/camera_proxy/camera.front_door
    ↓
ESP Device receives JPEG bytes
    ↓
JPEG Decoder (ESP-IDF)
    ↓
Decoded RGB pixel buffer
    ↓
LVGL Image Widget (lv_img)
    ↓
Display
```

### Configuration Storage

Camera settings stored in button config string (semicolon-delimited):
```
entity;label;icon;icon_on;sensor;unit;type;precision
camera.front_door;Front Door;Camera;Auto;5;;camera;fit
                  └─────────┘        └┘
                    entity_id        type
```

### JPEG Decoder

Uses ESP-IDF's built-in JPEG decoderto minimize dependencies. Supports:
- JPEG baseline and progressive
- Common color spaces (YCbCr, RGB)
- Progressive decoding for responsive updates
