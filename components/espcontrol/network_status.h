// =============================================================================
// NETWORK STATUS - Clock-bar network icon and device information modal
// =============================================================================
#pragma once

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>
#include "esphome/components/network/ip_address.h"
#include "esphome/components/network/util.h"

constexpr const char *NETWORK_ICON_WIFI_1 = "\U000F091F";
constexpr const char *NETWORK_ICON_WIFI_2 = "\U000F0922";
constexpr const char *NETWORK_ICON_WIFI_3 = "\U000F0925";
constexpr const char *NETWORK_ICON_WIFI_4 = "\U000F0928";
constexpr const char *NETWORK_ICON_ETHERNET = "\U000F0200";

struct NetworkStatusModalUi {
  lv_obj_t *overlay = nullptr;
  lv_obj_t *panel = nullptr;
};

inline NetworkStatusModalUi &network_status_modal_ui() {
  static NetworkStatusModalUi ui;
  return ui;
}

inline uint32_t network_status_parse_color(const std::string &hex, uint32_t fallback) {
  if (hex.length() != 6) return fallback;
  char *end = nullptr;
  uint32_t value = strtoul(hex.c_str(), &end, 16);
  return end && *end == '\0' ? value : fallback;
}

inline const char *network_status_wifi_icon(float pct) {
  if (!std::isfinite(pct) || pct < 25.0f) return NETWORK_ICON_WIFI_1;
  if (pct < 50.0f) return NETWORK_ICON_WIFI_2;
  if (pct < 75.0f) return NETWORK_ICON_WIFI_3;
  return NETWORK_ICON_WIFI_4;
}

inline void network_status_set_wifi_icon(lv_obj_t *label, float pct) {
  if (!label) return;
  lv_label_set_text(label, network_status_wifi_icon(pct));
}

inline void network_status_set_ethernet_icon(lv_obj_t *label) {
  if (!label) return;
  lv_label_set_text(label, NETWORK_ICON_ETHERNET);
}

inline void network_status_update_visibility(lv_obj_t *button, lv_obj_t *main_page_obj,
                                             bool clock_bar_enabled) {
  if (!button) return;
  if (clock_bar_enabled && lv_scr_act() == main_page_obj) {
    lv_obj_clear_flag(button, LV_OBJ_FLAG_HIDDEN);
  } else {
    lv_obj_add_flag(button, LV_OBJ_FLAG_HIDDEN);
  }
}

inline std::string network_status_ip_address() {
  auto ips = esphome::network::get_ip_addresses();
  if (!ips.empty()) {
    char ip_buf[esphome::network::IP_ADDRESS_BUFFER_SIZE];
    ips[0].str_to(ip_buf);
    return ip_buf;
  }
  return "Not available";
}

inline std::string network_status_wifi_strength_text(float pct) {
  if (!std::isfinite(pct)) return "Not available";
  if (pct < 0.0f) pct = 0.0f;
  if (pct > 100.0f) pct = 100.0f;
  char buf[12];
  snprintf(buf, sizeof(buf), "%d%%", (int)(pct + 0.5f));
  return buf;
}

inline std::string network_status_uptime_text(float seconds) {
  if (!std::isfinite(seconds) || seconds < 0.0f) return "Not available";
  uint32_t total = (uint32_t)(seconds + 0.5f);
  uint32_t days = total / 86400U;
  uint32_t hours = (total % 86400U) / 3600U;
  uint32_t minutes = (total % 3600U) / 60U;
  char buf[24];
  if (days > 0) {
    snprintf(buf, sizeof(buf), "%ud %02uh %02um", days, hours, minutes);
  } else if (hours > 0) {
    snprintf(buf, sizeof(buf), "%uh %02um", hours, minutes);
  } else {
    snprintf(buf, sizeof(buf), "%um", minutes);
  }
  return buf;
}

inline lv_coord_t network_status_scaled_px(lv_coord_t px, lv_coord_t short_side) {
  return px * short_side / 480;
}

inline void network_status_clean_obj(lv_obj_t *obj) {
  if (!obj) return;
  lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN);
  lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(obj, 0, LV_PART_MAIN);
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
}

inline void network_status_hide_modal() {
  NetworkStatusModalUi &ui = network_status_modal_ui();
  if (ui.overlay) lv_obj_del(ui.overlay);
  ui.overlay = nullptr;
  ui.panel = nullptr;
}

inline lv_obj_t *network_status_add_label(lv_obj_t *parent, const char *text,
                                          const lv_font_t *font,
                                          lv_coord_t width,
                                          uint32_t color = 0xFFFFFF) {
  lv_obj_t *label = lv_label_create(parent);
  lv_label_set_text(label, text ? text : "");
  lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
  lv_obj_set_width(label, width);
  lv_obj_set_style_text_color(label, lv_color_hex(color), LV_PART_MAIN);
  if (font) lv_obj_set_style_text_font(label, font, LV_PART_MAIN);
  return label;
}

inline void network_status_open_modal(const std::string &device_name,
                                      const std::string &ip_address,
                                      const std::string &wifi_strength,
                                      float uptime_seconds,
                                      const std::string &firmware_version,
                                      const lv_font_t *label_font,
                                      const lv_font_t *icon_font,
                                      const std::string &panel_color_hex) {
  network_status_hide_modal();
  NetworkStatusModalUi &ui = network_status_modal_ui();

  lv_disp_t *disp = lv_disp_get_default();
  lv_coord_t sw = disp ? lv_disp_get_hor_res(disp) : 480;
  lv_coord_t sh = disp ? lv_disp_get_ver_res(disp) : 480;
  lv_coord_t short_side = sw < sh ? sw : sh;
  lv_coord_t pad = network_status_scaled_px(20, short_side);
  if (pad < 14) pad = 14;
  lv_coord_t gap = network_status_scaled_px(12, short_side);
  if (gap < 8) gap = 8;
  lv_coord_t close_size = network_status_scaled_px(34, short_side);
  if (close_size < 30) close_size = 30;
  lv_coord_t panel_w = sw * 82 / 100;
  lv_coord_t max_w = network_status_scaled_px(560, short_side);
  if (panel_w > max_w) panel_w = max_w;
  if (panel_w < short_side - 48) panel_w = short_side - 48;
  if (panel_w > sw - 24) panel_w = sw - 24;
  lv_coord_t content_w = panel_w - pad * 2;

  ui.overlay = lv_obj_create(lv_layer_top());
  lv_obj_set_size(ui.overlay, lv_pct(100), lv_pct(100));
  network_status_clean_obj(ui.overlay);

  uint32_t panel_color = network_status_parse_color(panel_color_hex, 0x212121);
  ui.panel = lv_obj_create(ui.overlay);
  lv_obj_set_width(ui.panel, panel_w);
  lv_obj_set_height(ui.panel, LV_SIZE_CONTENT);
  lv_obj_set_style_bg_color(ui.panel, lv_color_hex(panel_color), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(ui.panel, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_border_width(ui.panel, 0, LV_PART_MAIN);
  lv_obj_set_style_shadow_width(ui.panel, 0, LV_PART_MAIN);
  lv_obj_set_style_radius(ui.panel, network_status_scaled_px(12, short_side), LV_PART_MAIN);
  lv_obj_set_style_pad_all(ui.panel, pad, LV_PART_MAIN);
  lv_obj_set_style_pad_row(ui.panel, gap, LV_PART_MAIN);
  lv_obj_set_layout(ui.panel, LV_LAYOUT_FLEX);
  lv_obj_set_style_flex_flow(ui.panel, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN);
  lv_obj_clear_flag(ui.panel, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *header = lv_obj_create(ui.panel);
  network_status_clean_obj(header);
  lv_obj_set_width(header, content_w);
  lv_obj_set_height(header, close_size);
  lv_obj_t *title = lv_label_create(header);
  lv_label_set_text(title, "Device Info");
  lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
  if (label_font) lv_obj_set_style_text_font(title, label_font, LV_PART_MAIN);
  lv_obj_align(title, LV_ALIGN_LEFT_MID, 0, 0);

  lv_obj_t *close_btn = lv_btn_create(header);
  lv_obj_set_size(close_btn, close_size, close_size);
  lv_obj_set_style_radius(close_btn, close_size / 2, LV_PART_MAIN);
  lv_obj_set_style_bg_opa(close_btn, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_width(close_btn, 0, LV_PART_MAIN);
  lv_obj_set_style_shadow_width(close_btn, 0, LV_PART_MAIN);
  lv_obj_align(close_btn, LV_ALIGN_RIGHT_MID, 0, 0);
  lv_obj_t *close_label = lv_label_create(close_btn);
  lv_label_set_text(close_label, "\U000F0156");
  lv_obj_set_style_text_color(close_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
  if (icon_font) lv_obj_set_style_text_font(close_label, icon_font, LV_PART_MAIN);
  lv_obj_center(close_label);
  lv_obj_add_event_cb(close_btn, [](lv_event_t *) {
    network_status_hide_modal();
  }, LV_EVENT_CLICKED, nullptr);

  std::string uptime = network_status_uptime_text(uptime_seconds);
  std::string firmware = firmware_version.empty() ? "Not available" : firmware_version;
  std::string body =
    "Device name: " + (device_name.empty() ? std::string("Not available") : device_name) +
    "\nIP address: " + ip_address +
    "\nWiFi strength: " + wifi_strength +
    "\nUptime: " + uptime +
    "\nFirmware version: " + firmware;
  network_status_add_label(ui.panel, body.c_str(), label_font, content_w, 0xEDEDED);

  lv_obj_update_layout(ui.panel);
  lv_obj_center(ui.panel);
}
