// Reducing size: https://thomasbaart.nl/2018/12/01/reducing-firmware-size-in-qmk/
// Can disable RGB effects for size drop
// RGBLIGHT_ENABLE is disabled in rule.mk since case is non-transparent

#include QMK_KEYBOARD_H
#include "config.h"

// Tap Dance declarations
enum _tap_dance_actions {
  TD_PGDN_PGUP = 0,
  TD_END_HOME,
  TD_DEL_INS
};

// Tap Dance definitions
tap_dance_action_t tap_dance_actions[] = {
  [TD_PGDN_PGUP] = ACTION_TAP_DANCE_DOUBLE(KC_PGDN, KC_PGUP),
  [TD_END_HOME]  = ACTION_TAP_DANCE_DOUBLE(KC_END,  KC_HOME),
  [TD_DEL_INS]   = ACTION_TAP_DANCE_DOUBLE(KC_DEL,  KC_INS)
};

enum _layers {
  LAYER_BASE = 0,
  LAYER_MAC,
  LAYER_UTIL,
  LAYER_SYS
};

#define MKC_RA_RC_D  RALT(RCTL(KC_DEL))
#define MKC_RC_RS_E  RCTL(RSFT(KC_ESC))
// Tap and hold
// To enable such behavior, cannot use basic defined keycodes in enum, must use LT() for place holder
// https://github.com/qmk/qmk_firmware/blob/master/docs/mod_tap.md#changing-both-tap-and-hold
// If needed more change KC_
#define MKC_LYR_ULT  LT(0, KC_0)         // Toggle between BASE and MAC layer when tap, UTIL when hold
// Can just use LT(LAYER_UTIL, KC_CAPS) but has some conflict handing UTIL layer with MKC_LYR_ULT
#define MKC_CAP_ULT  LT(0, KC_1)         // Toggle CAPSLOCK when tap, UTIL when hold

enum _keycodes {
  // ==================================================
#if defined(RGB_MATRIX_ENABLE) && defined(RGBLIGHT_ENABLE)
  // RGB mode keycode for RGB matrix
  // When RGBLIGHT_ENABLE = yes, using RGB_* keycode will trigger both RGB backlight and matrix, we don't want that
  // There is a disable define in config.h that disable keycode for RGB matrix => keycode only control backlight
  // Need to implement own control keycode for matrix
  // https://github.com/qmk/qmk_firmware/blob/master/docs/feature_rgblight.md
  // https://github.com/qmk/qmk_firmware/blob/master/docs/feature_rgb_matrix.md
  MKC_RM_TOG = SAFE_RANGE, // Toggle RGB lighting on or off
  MKC_RM_MOD,              // Cycle through modes, reverse direction when Shift is held
  MKC_RM_HUI,              // Increase hue, decrease hue when Shift is held
  MKC_RM_HUD,              // Decrease hue, increase hue when Shift is held
  MKC_RM_SAI,              // Increase saturation, decrease saturation when Shift is held
  MKC_RM_SAD,              // Decrease saturation, increase saturation when Shift is held
  MKC_RM_VAI,              // Increase value (brightness), decrease value when Shift is held
  MKC_RM_VAD,              // Decrease value (brightness), increase value when Shift is held
  MKC_RM_SPI,              // Increase effect speed, decrease speed when Shift is held
  MKC_RM_SPD,              // Decrease effect speed, increase speed when Shift is held
#endif
  // ==================================================
  MKC_LISYSRQ              // Linux uses alt + print screen instead of sysrq keycode (Normal combo RALT(KC_PSCR) does not seem to work)
#if !defined(RGB_MATRIX_ENABLE) || !defined(RGBLIGHT_ENABLE)
  = SAFE_RANGE
#endif
,
  MKC_EREBOOT              // Emergency reboot
};

// Manage own base layer, can be BASE or MAC
static uint8_t layer_base         = LAYER_BASE;
static uint8_t layer_current      = LAYER_BASE;
static uint8_t layer_prev         = LAYER_BASE;

layer_state_t layer_state_set_user(layer_state_t state) {
  layer_prev = layer_current;
  layer_current = get_highest_layer(state);
  switch (layer_current) {
  case LAYER_BASE:
    layer_base = LAYER_BASE;
    break;
  case LAYER_MAC:
    layer_base = LAYER_MAC;
    break;
  case LAYER_UTIL:
    break;
  case LAYER_SYS:
    break;
  default: //  for any other layers, or the default layer
    break;
  }
  return state;
}

bool process_detected_host_os_user(os_variant_t detected_os) {
  switch (detected_os) {
    case OS_MACOS:
    case OS_IOS:
      layer_move(LAYER_MAC);
      break;
    case OS_WINDOWS:
    case OS_LINUX:
    case OS_UNSURE:
      layer_move(LAYER_BASE);
      break;
  }
  return false;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
#if defined(RGB_MATRIX_ENABLE)
  #if defined(RGBLIGHT_ENABLE) 
    case MKC_RM_TOG:
  #else
    case RGB_TOG:
  #endif
      if (record->event.pressed) {
        switch (rgb_matrix_get_flags()) {
          case LED_FLAG_ALL:
            rgb_matrix_set_flags(LED_FLAG_NONE);
            rgb_matrix_set_color_all(0, 0, 0);
            break;
          default:
            rgb_matrix_set_flags(LED_FLAG_ALL);
            break;
        }
      }
      if (!rgb_matrix_is_enabled()) {
        rgb_matrix_set_flags(LED_FLAG_ALL);
        rgb_matrix_enable();
      }
      return false;
#endif
#if defined(RGB_MATRIX_ENABLE) && defined(RGBLIGHT_ENABLE)
    case MKC_RM_MOD:
      if (record->event.pressed) {
        rgb_matrix_step();
      }
      return false;
    case MKC_RM_HUI:
      if (record->event.pressed) {
        rgb_matrix_increase_hue();
      }
      return false;
    case MKC_RM_HUD:
      if (record->event.pressed) {
        rgb_matrix_decrease_hue();
      }
      return false;
    case MKC_RM_SAI:
      if (record->event.pressed) {
        rgb_matrix_increase_sat();
      }
      return false;
    case MKC_RM_SAD:
      if (record->event.pressed) {
        rgb_matrix_decrease_sat();
      }
      return false;
    case MKC_RM_VAI:
      if (record->event.pressed) {
        rgb_matrix_increase_val();
      }
      return false;
    case MKC_RM_VAD:
      if (record->event.pressed) {
        rgb_matrix_decrease_val();
      }
      return false;
    case MKC_RM_SPI:
      if (record->event.pressed) {
        rgb_matrix_increase_speed();
      }
      return false;
    case MKC_RM_SPD:
      if (record->event.pressed) {
        rgb_matrix_decrease_speed();
      }
      return false;
#endif
    case MKC_LYR_ULT:
      if (record->tap.count) {
        // Key being tap
        if (record->event.pressed) {
          // key tap down event
          uint8_t layer_tmp = get_highest_layer(layer_state);
          if (layer_tmp == LAYER_BASE) {
            // default_layer_set(1UL << 1); // Mac  == layer 2, this does not properly set layer info
            layer_move(LAYER_MAC);
          }
          else if (layer_tmp == LAYER_MAC) {
            // default_layer_set(1UL << 0); // Base == layer 1, this does not properly set layer info
            layer_move(LAYER_BASE);
          }
          else {
            // Update layer_base manually so util layers can return to it later
            layer_base = ((layer_base == LAYER_BASE) ? LAYER_MAC : LAYER_BASE);
          }
        }
      }
      else {
        // Key being held
        if (record->event.pressed) {
          // key held down event
          if (get_highest_layer(layer_state) != LAYER_UTIL)
            layer_move(LAYER_UTIL);
        }
        else {
          // key held up event
          if (get_highest_layer(layer_state) == LAYER_UTIL)
            layer_move(layer_base);
        }
      }
      return false;
    case MKC_CAP_ULT:
      if (record->tap.count) {
        // Key being tap
        if (record->event.pressed) {
          // key tap down event
          tap_code(KC_CAPS);
        }
      }
      else {
        // Key being held
        if (record->event.pressed) {
          // key held down event
          if (get_highest_layer(layer_state) != LAYER_UTIL)
            layer_move(LAYER_UTIL);
        }
        else {
          // key held up event
          if (get_highest_layer(layer_state) == LAYER_UTIL)
            layer_move(layer_base);
        }
      }
      return false;
//  Macro keycode has X_ for mat instead of KC_
//  https://github.com/qmk/qmk_firmware/blob/master/quantum/send_string/send_string_keycodes.h
//  https://github.com/qmk/qmk_firmware/blob/master/docs/feature_macros.md
    case MKC_LISYSRQ:
      if (record->event.pressed) {
        // key down
        SEND_STRING(SS_DOWN(X_RALT)SS_DOWN(X_PSCR));
      }
      else {
        // key up
        SEND_STRING(SS_UP(X_PSCR)SS_UP(X_RALT));
      }
      return false;
    case MKC_EREBOOT:
      if (record->event.pressed) {
        // *** The language of the slaves ***
        // Linux sysrq panic reboot
        SEND_STRING(SS_RALT(SS_DOWN(X_PSCR)"reisub"SS_UP(X_PSCR)));
        // Windows force crash, need to add registry
        // https://learn.microsoft.com/en-us/windows-hardware/drivers/debugger/forcing-a-system-crash-from-the-keyboard
        SEND_STRING(SS_RCTL(SS_TAP(X_SCRL)SS_TAP(X_SCRL)));
        // Mac force restart - untested
        // https://support.apple.com/en-us/HT201236
        // There are 2 power codes in qmk X_PWR & X_KB_POWER
        // https://github.com/qmk/qmk_firmware/blob/master/docs/faq_keymap.md
        SEND_STRING(SS_RCTL(SS_RCMD(SS_TAP(X_KB_POWER))));
      }
      return false;
    default:
      return true;
  }
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [LAYER_BASE] = LAYOUT_65_ansi (
    KC_GRV,          KC_1,       KC_2,       KC_3,       KC_4,       KC_5,       KC_6,       KC_7,       KC_8,       KC_9,       KC_0,        KC_MINS, KC_EQL,  KC_BSPC, LT(LAYER_SYS, KC_ESC),
    KC_TAB,          KC_Q,       KC_W,       KC_E,       KC_R,       KC_T,       KC_Y,       KC_U,       KC_I,       KC_O,       KC_P,        KC_LBRC, KC_RBRC, KC_BSLS, TD(TD_DEL_INS),
    MKC_CAP_ULT,     KC_A,       KC_S,       KC_D,       KC_F,       KC_G,       KC_H,       KC_J,       KC_K,       KC_L,       KC_SCLN,     KC_QUOT,          KC_ENT,  TD(TD_PGDN_PGUP),
    KC_LSFT,                     KC_Z,       KC_X,       KC_C,       KC_V,       KC_B,       KC_N,       KC_M,       KC_COMM,    KC_DOT,      KC_SLSH, KC_RSFT, KC_UP,   TD(TD_END_HOME),
    KC_LCTL,         KC_LGUI,    KC_LALT,                                        KC_SPC,                             KC_RALT,    MKC_LYR_ULT, KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT
  ),
  [LAYER_MAC] = LAYOUT_65_ansi (
    KC_GRV,          KC_1,       KC_2,       KC_3,       KC_4,       KC_5,       KC_6,       KC_7,       KC_8,       KC_9,       KC_0,        KC_MINS, KC_EQL,  KC_BSPC, LT(LAYER_SYS, KC_ESC),
    KC_TAB,          KC_Q,       KC_W,       KC_E,       KC_R,       KC_T,       KC_Y,       KC_U,       KC_I,       KC_O,       KC_P,        KC_LBRC, KC_RBRC, KC_BSLS, TD(TD_DEL_INS),
    MKC_CAP_ULT,     KC_A,       KC_S,       KC_D,       KC_F,       KC_G,       KC_H,       KC_J,       KC_K,       KC_L,       KC_SCLN,     KC_QUOT,          KC_ENT,  TD(TD_PGDN_PGUP),
    KC_LSFT,                     KC_Z,       KC_X,       KC_C,       KC_V,       KC_B,       KC_N,       KC_M,       KC_COMM,    KC_DOT,      KC_SLSH, KC_RSFT, KC_UP,   TD(TD_END_HOME),
    KC_LCTL,         KC_LOPT,    KC_LCMD,                                        KC_SPC,                             KC_RCMD,    MKC_LYR_ULT, KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT
  ),
  [LAYER_UTIL] = LAYOUT_65_ansi (
    KC_ESC,      KC_F1,      KC_F2,      KC_F3,      KC_F4,      KC_F5,      KC_F6,      KC_F7,      KC_F8,      KC_F9,      KC_F10,      KC_F11,  KC_F12,  KC_F13,  KC_PSCR,
#if defined(RGBLIGHT_ENABLE)
    XXXXXXX,     RGB_TOG,    RGB_MOD,    RGB_HUI,    RGB_HUD,    RGB_SAI,    RGB_SAD,    RGB_VAI,    RGB_VAD,    RGB_SPI,    RGB_SPD,     XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  #if defined(RGB_MATRIX_ENABLE)
    _______,     MKC_RM_TOG, MKC_RM_MOD, MKC_RM_HUI, MKC_RM_HUD, MKC_RM_SAI, MKC_RM_SAD, MKC_RM_VAI, MKC_RM_VAD, MKC_RM_SPI, MKC_RM_SPD,  XXXXXXX,          XXXXXXX, XXXXXXX,
  #else
    _______,     XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,     XXXXXXX,          XXXXXXX, XXXXXXX,
  #endif
#else
  #if defined(RGB_MATRIX_ENABLE)
    XXXXXXX,     RGB_TOG,    RGB_MOD,    RGB_HUI,    RGB_HUD,    RGB_SAI,    RGB_SAD,    RGB_VAI,    RGB_VAD,    RGB_SPI,    RGB_SPD,     XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  #else
    XXXXXXX,     XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,     XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  #endif
    _______,     XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,     XXXXXXX,          XXXXXXX, XXXXXXX,
#endif
    _______,                 KC_VOLD,    KC_VOLU,    KC_MUTE,    XXXXXXX,    XXXXXXX,    NK_TOGG,    XXXXXXX,    XXXXXXX,    XXXXXXX,     XXXXXXX, _______, XXXXXXX, XXXXXXX,
    _______,     _______,    _______,                                        XXXXXXX,                            _______,    _______,     _______, XXXXXXX, XXXXXXX, XXXXXXX
  ),
  [LAYER_SYS] = LAYOUT_65_ansi (
    MKC_EREBOOT, _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,     XXXXXXX, XXXXXXX, XXXXXXX, _______,
    QK_BOOT,     _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,     XXXXXXX, XXXXXXX, XXXXXXX, MKC_LISYSRQ,
    XXXXXXX,     _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    XXXXXXX,     XXXXXXX,          XXXXXXX, XXXXXXX,
    XXXXXXX,                 _______,    _______,    _______,    _______,    _______,    _______,    _______,    XXXXXXX,    XXXXXXX,     XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    XXXXXXX,     XXXXXXX,    XXXXXXX,                                        XXXXXXX,                            XXXXXXX,    XXXXXXX,     XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX
  )
};

// Indicators
// https://github.com/qmk/qmk_firmware/blob/master/docs/feature_rgb_matrix.md#indicator-examples-idindicator-examples
// COLOR https://github.com/qmk/qmk_firmware/blob/master/docs/feature_rgb_matrix.md#colors-idcolors
// LED index is in board level rev2.c

// Set color of all key to black once after layer changes back to base to prevent leftover
// as some effect won't set color for all keys
// NOTE: Tried many ways of cleaning up, set all keys to black once does not seem to be enough to clear all left over
// cleanup few times after layer change seem to work
// NOTE2: Doc says rgb_matrix_indicators_advanced_user might not set all led every cycle -> lead to leftover

// TODO FLAG? 
//      Indicators without RGB Matrix Effect https://github.com/qmk/qmk_firmware/blob/master/docs/feature_rgb_matrix.md#indicators-without-rgb-matrix-effect

static uint8_t led_last_layer    = LAYER_BASE;
static uint8_t led_cleanup_count = 0;

bool rgb_matrix_indicators_user() {
  return false; // To override rgb_matrix_indicators_kb in rev2.c
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
  bool layer_changed = (layer_current != led_last_layer);
  bool layer_cleanup = (led_cleanup_count > 0);
  if (layer_cleanup)
      led_cleanup_count--;
  if (layer_changed) {
      led_last_layer    = layer_current;
      led_cleanup_count = 5; // 5 times seems enough
  }
  // Base color for special layers
  for (uint8_t row = 0; row < MATRIX_ROWS; ++row) {
    for (uint8_t col = 0; col < MATRIX_COLS; ++col) {
      uint8_t index = g_led_config.matrix_co[row][col];
      if ((index >= led_min) && (index < led_max) && (index != NO_LED)) { // Led valid
        // Clear all the key if needed
        // Faster than comparing individual key as div takes time
        if (layer_changed || layer_cleanup)
          rgb_matrix_set_color(index, RGB_BLACK);
        // If this layer has a base color for valid keycode, set those
        // Skip layer base and mac because those use led effect
        // All keys with invalid keycode are turned off 
        if (keymap_key_to_keycode(layer_current, (keypos_t){col,row}) >= KC_TRNS) { // keycode valid
          switch (layer_current) {
            // We need to clear led because of these 2
            // If led effect not modify keys on the 2 layers then leftover will happend
            case LAYER_MAC:
            case LAYER_BASE:
              break;
            case LAYER_UTIL:
              rgb_matrix_set_color(index, RGB_ORANGE);
              break;
            case LAYER_SYS:
              rgb_matrix_set_color(index, RGB_CYAN);
              break;
          }
        }
        else // keycode invalid
          rgb_matrix_set_color(index, RGB_BLACK);
      }
    }
  }
  // If led flag is not NONE then leave it to effect
  // else set to black as default
  if (rgb_matrix_get_flags() == LED_FLAG_NONE)
    rgb_matrix_set_color(30, RGB_BLACK);
  // Special keys for each layer
  switch(layer_current) {
    case LAYER_MAC:
      rgb_matrix_set_color(63, RGB_WHITE); // FN
      // Fall through
    case LAYER_BASE:
      // Caps indicator
      if (host_keyboard_led_state().caps_lock)
        rgb_matrix_set_color(30, RGB_RED);
      break;
    case LAYER_UTIL:
      break;
    case LAYER_SYS:
      rgb_matrix_set_color(0 , RGB_RED);   // MKC_EREBOOT
      rgb_matrix_set_color(15, RGB_ORANGE);// QK_BOOT
      rgb_matrix_set_color(29, RGB_WHITE); // MKC_LISYSRQ
      break;
  }
  return false;
}
