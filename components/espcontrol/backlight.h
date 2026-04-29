// =============================================================================
// BACKLIGHT - Brightness scheduling, sunrise/sunset, and UI helpers
// =============================================================================
// Shared C++ utilities for backlight schedule logic and temperature label
// management. Extracted from YAML lambdas so the logic is testable and
// syntax-highlighted, while YAML retains only thin id() wiring.
// =============================================================================
#pragma once
#include <string>
#include <cstdio>
#include <cmath>
#include "sun_calc.h"
#include "temperature_unit.h"

#ifdef USE_ESP32
#include <esp_sleep.h>
#include <esp_system.h>
#endif

// ── Sunrise/sunset recalculation ─────────────────────────────────────

struct SunCalcResult {
  int rise_h, rise_m, set_h, set_m;
  bool valid;
  char sunrise_str[16];
  char sunset_str[16];
};

inline SunCalcResult recalc_sunrise_sunset(
    int year, int month, int day,
    const std::string &tz_option, bool use_12h = true) {
  SunCalcResult r = {};

  std::string tz_id = timezone_id_from_option(tz_option);
  float tz_offset = utc_offset_hours_for_date(year, month, day, tz_option);

  float lat, lon;
  if (!lookup_tz_coords(tz_id, lat, lon)) {
    ESP_LOGW("backlight", "No coordinates for timezone %s", tz_id.c_str());
    r.valid = false;
    return r;
  }

  calc_sunrise_sunset(year, month, day, lat, lon, tz_offset,
                      r.rise_h, r.rise_m, r.set_h, r.set_m);
  r.valid = true;

  int rh = r.rise_h, rm = r.rise_m;
  if (use_12h) {
    snprintf(r.sunrise_str, sizeof(r.sunrise_str), "%d:%02d AM",
             (rh == 0) ? 12 : (rh > 12 ? rh - 12 : rh), rm);
    if (rh >= 12)
      snprintf(r.sunrise_str, sizeof(r.sunrise_str), "%d:%02d PM",
               (rh == 12) ? 12 : rh - 12, rm);
  } else {
    snprintf(r.sunrise_str, sizeof(r.sunrise_str), "%02d:%02d", rh, rm);
  }

  int sh = r.set_h, sm = r.set_m;
  if (use_12h) {
    snprintf(r.sunset_str, sizeof(r.sunset_str), "%d:%02d PM",
             (sh == 12) ? 12 : (sh > 12 ? sh - 12 : sh), sm);
    if (sh < 12)
      snprintf(r.sunset_str, sizeof(r.sunset_str), "%d:%02d AM",
               (sh == 0) ? 12 : sh, sm);
  } else {
    snprintf(r.sunset_str, sizeof(r.sunset_str), "%02d:%02d", sh, sm);
  }

  int lat_c = (int)((lat >= 0 ? lat : -lat) * 100.0f + 0.5f);
  int lon_c = (int)((lon >= 0 ? lon : -lon) * 100.0f + 0.5f);
  int tz_c = (int)((tz_offset >= 0 ? tz_offset : -tz_offset) * 10.0f + 0.5f);
  ESP_LOGI("backlight",
           "Sunrise %02d:%02d, Sunset %02d:%02d "
           "(lat=%s%d.%02d lon=%s%d.%02d tz=%s%d.%d)",
           rh, rm, sh, sm,
           lat < 0 ? "-" : "", lat_c / 100, lat_c % 100,
           lon < 0 ? "-" : "", lon_c / 100, lon_c % 100,
           tz_offset < 0 ? "-" : "", tz_c / 10, tz_c % 10);

  return r;
}

// ── Brightness calculation ───────────────────────────────────────────

inline float calc_brightness_pct(
    bool sunrise_valid, int rise_h, int rise_m, int set_h, int set_m,
    int now_h, int now_m, bool *is_daytime,
    float day_pct, float night_pct) {
  if (!sunrise_valid) return day_pct;
  int now_min = now_h * 60 + now_m;
  int rise_min = rise_h * 60 + rise_m;
  int set_min = set_h * 60 + set_m;
  *is_daytime = (now_min >= rise_min && now_min < set_min);
  return *is_daytime ? day_pct : night_pct;
}

// ── Daylight transition detection ────────────────────────────────────

inline bool check_daylight_transition(
    bool sunrise_valid, int rise_h, int rise_m, int set_h, int set_m,
    int now_h, int now_m, bool last_is_day) {
  if (!sunrise_valid) return false;
  int now_min = now_h * 60 + now_m;
  bool is_day = (now_min >= rise_h * 60 + rise_m) &&
                (now_min < set_h * 60 + set_m);
  return is_day != last_is_day;
}

// ── Screen schedule helpers ───────────────────────────────────────────

inline bool screen_schedule_in_window(int now_h, int on_hour, int off_hour) {
  if (on_hour < 0) on_hour = 0;
  if (on_hour > 23) on_hour = 23;
  if (off_hour < 0) off_hour = 0;
  if (off_hour > 23) off_hour = 23;
  if (on_hour < off_hour) return now_h >= on_hour && now_h < off_hour;
  if (on_hour > off_hour) return now_h >= on_hour || now_h < off_hour;
  return true;
}

inline bool screen_schedule_always_on_mode(const std::string &mode) {
  return mode == "Screen Dimmed" || mode == "screen_dimmed" ||
         mode == "Always On" || mode == "always_on";
}

inline bool screen_schedule_clock_mode(const std::string &mode) {
  return mode == "Clock" || mode == "clock";
}

// ── Temperature label visibility ─────────────────────────────────────

inline void update_temp_label(lv_obj_t *label, lv_obj_t *main_page_obj,
                              bool this_enabled, bool other_enabled) {
  char one[12];
  char both[24];
  snprintf(one, sizeof(one), "-%s", display_temperature_unit_symbol());
  snprintf(both, sizeof(both), "-%s / -%s",
           display_temperature_unit_symbol(), display_temperature_unit_symbol());
  if (this_enabled) {
    if (lv_scr_act() == main_page_obj)
      lv_obj_clear_flag(label, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(label, other_enabled ? both : one);
  } else if (!other_enabled) {
    lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
  } else {
    lv_label_set_text(label, one);
  }
}

inline void refresh_temp_label_values(lv_obj_t *label, lv_obj_t *main_page_obj,
                                      bool clock_bar_enabled,
                                      bool indoor_enabled, bool outdoor_enabled,
                                      float indoor, float outdoor) {
  if (!clock_bar_enabled || (!indoor_enabled && !outdoor_enabled)) {
    lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
    return;
  }

  if (lv_scr_act() == main_page_obj) lv_obj_clear_flag(label, LV_OBJ_FLAG_HIDDEN);

  char indoor_buf[16];
  char outdoor_buf[16];
  const char *unit = display_temperature_unit_symbol();
  if (indoor_enabled) {
    if (std::isnan(indoor)) snprintf(indoor_buf, sizeof(indoor_buf), "-%s", unit);
    else snprintf(indoor_buf, sizeof(indoor_buf), "%.0f%s", indoor, unit);
  }
  if (outdoor_enabled) {
    if (std::isnan(outdoor)) snprintf(outdoor_buf, sizeof(outdoor_buf), "-%s", unit);
    else snprintf(outdoor_buf, sizeof(outdoor_buf), "%.0f%s", outdoor, unit);
  }

  char buf[40];
  if (indoor_enabled && outdoor_enabled) {
    snprintf(buf, sizeof(buf), "%s / %s", outdoor_buf, indoor_buf);
  } else if (outdoor_enabled) {
    snprintf(buf, sizeof(buf), "%s", outdoor_buf);
  } else {
    snprintf(buf, sizeof(buf), "%s", indoor_buf);
  }
  lv_label_set_text(label, buf);
}

// ── Firmware update interval ─────────────────────────────────────────

inline bool should_check_update(int counter, const std::string &freq) {
  int threshold = 24;
  if (freq == "Hourly") threshold = 1;
  else if (freq == "Weekly") threshold = 168;
  else if (freq == "Monthly") threshold = 720;
  return counter % threshold == 0;
}
