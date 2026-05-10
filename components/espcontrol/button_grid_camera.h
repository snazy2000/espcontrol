#pragma once

// Internal implementation detail for button_grid.h. Include button_grid.h from device YAML.
//
// Camera popup card: tapping the tile opens a fullscreen LVGL overlay that
// shows periodic JPEG snapshots fetched from HA's camera_proxy REST endpoint.
// The overlay tree and fetch loop live in common/addon/camera_popup.yaml;
// this header only wires the tile visual and opens the popup by assigning
// YAML globals and executing camera_popup_open_script.

inline void setup_camera_card(BtnSlot &s, const ParsedCfg &p) {
  lv_label_set_text(s.text_lbl,
    p.label.empty() ? (p.entity.empty() ? "Camera" : p.entity.c_str())
                    : p.label.c_str());
  const char *icon_cp =
    (p.icon.empty() || p.icon == "Auto") ? find_icon("Camera") : find_icon(p.icon.c_str());
  lv_label_set_text(s.icon_lbl, icon_cp);
  apply_push_button_transition(s.btn);
}

// Parse refresh interval (seconds) from the sensor field; default 5, clamp 2-120.
inline uint32_t camera_refresh_ms(const std::string &sensor_field) {
  if (sensor_field.empty()) return 5000u;
  int secs = atoi(sensor_field.c_str());
  if (secs < 2) secs = 2;
  if (secs > 120) secs = 120;
  return static_cast<uint32_t>(secs) * 1000u;
}

inline bool camera_entity_valid(const std::string &entity_id) {
  return entity_id.size() > 7 && entity_id.compare(0, 7, "camera.") == 0;
}

// Assign popup globals and execute the open script (defined in camera_popup.yaml).
// Safe to call from both main-grid handle_button_click and subpage event callbacks.
inline void camera_popup_open_from_cfg(const ParsedCfg &p) {
  if (!camera_entity_valid(p.entity)) {
    ESP_LOGW("camera_popup", "Expected camera.* entity, got: %s", p.entity.c_str());
    return;
  }
  id(camera_popup_entity) = p.entity;
  id(camera_popup_refresh_ms) = camera_refresh_ms(p.sensor);
  id(camera_popup_open_script).execute();
}
