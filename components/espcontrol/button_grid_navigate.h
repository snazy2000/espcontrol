#pragma once

// Internal implementation detail for button_grid.h. Include button_grid.h from device YAML.

// =============================================================================
// REMOTE NAVIGATION - Label-to-screen registry for HA-triggered navigation
// =============================================================================
// grid_phase2() populates this registry at boot. Call navigate_to_page(label)
// from an ESPHome text entity on_value callback to switch screens by name.
// "home" or "" navigates back to the main page.
// =============================================================================

struct NavEntry {
  std::string label;
  lv_obj_t *screen;
};

static std::vector<NavEntry> g_nav_registry;
static lv_obj_t *g_nav_main_page = nullptr;

inline void nav_registry_reset(lv_obj_t *main_page) {
  g_nav_registry.clear();
  g_nav_main_page = main_page;
}

inline void nav_registry_add(const std::string &label, lv_obj_t *screen) {
  if (!label.empty() && screen)
    g_nav_registry.push_back({label, screen});
}

// Navigate to a subpage by label (case-insensitive), or to the home screen
// when target is "home" or empty. Wakes the display if the screensaver is active.
inline void navigate_to_page(const std::string &target) {
  lv_disp_trig_activity(nullptr);
  if (target.empty() || target == "home") {
    if (g_nav_main_page)
      lv_scr_load_anim(g_nav_main_page, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    return;
  }
  std::string t = target;
  for (char &c : t) c = (char)std::tolower((unsigned char)c);
  for (auto &entry : g_nav_registry) {
    std::string l = entry.label;
    for (char &c : l) c = (char)std::tolower((unsigned char)c);
    if (l == t) {
      lv_scr_load_anim(entry.screen, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
      return;
    }
  }
}
