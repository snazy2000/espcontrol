# How to add an icon

All button icons are defined once in [`icons.json`](icons.json) and synced to four downstream files by a script. Never edit icon lists in those files directly.

## 1. Find the icon on MDI

Browse [Material Design Icons](https://materialdesignicons.com/) and note three things:

| Field | Example | Where to find it |
|-------|---------|-------------------|
| **name** | `Ceiling Fan` | Choose a user-friendly display name |
| **codepoint** | `F1797` | Shown on the icon detail page (hex, no `0x` prefix) |
| **mdi** | `ceiling-fan` | The MDI class name (used as `mdi-ceiling-fan` in CSS) |

## 2. Add the entry to `icons.json`

Open `common/assets/icons.json` and add an object to the `"icons"` array:

```json
{ "name": "Ceiling Fan", "codepoint": "F1797", "mdi": "ceiling-fan" }
```

The array order determines display order in the LVGL font glyph list and the C++ lookup table. The YAML select options and JS picker sort alphabetically, so position doesn't matter for those.

## 3. Run the sync script

```sh
python scripts/sync_icons.py
```

This patches four files between their `GENERATED:ICONS` markers:

- `common/assets/icons.yaml` — font glyph codepoints
- `common/config/button_template.yaml` — YAML select options
- `devices/.../device/sensors.yaml` — C++ `icon_entries[]` + domain defaults
- `docs/public/webserver/.../www.js` — JS `ICON_MAP` + `DOMAIN_ICONS`

## 4. Verify

```sh
python scripts/sync_icons.py --check
```

Exits 0 if everything is in sync. Use this in CI to catch manual edits that drift.

## Domain defaults

To change which icon is used when a button's icon is set to "Auto", edit the `"domain_defaults"` object in `icons.json`. Values must reference an icon `name` from the `"icons"` array.
