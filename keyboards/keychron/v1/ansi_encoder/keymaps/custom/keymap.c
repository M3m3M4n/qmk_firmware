#include QMK_KEYBOARD_H
#include "config.h"

// clang-format off

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
    LAYER_BASE_MAC = 0,
    LAYER_UTIL_MAC,
    LAYER_BASE_WIN,
    LAYER_UTIL_WIN
};

#ifdef DIP_SWITCH_ENABLE
bool dip_switch_update_user(uint8_t index, bool active) {
    // This default implemetation does not properly move the layer, causing rgb matrix error
    // if (index == 0) {
    //     default_layer_set(1UL << (active ? LAYER_BASE_WIN : LAYER_BASE_MAC));
    // }
    if (index == 0) {
        if (active)
            layer_move(LAYER_BASE_WIN);
        else
            layer_move(LAYER_BASE_MAC);
    }
    return false;
}
#endif

#define MKC_TASK LGUI(KC_TAB)
#define MKC_FLXP LGUI(KC_E)

enum _keycodes {
    MKC_EREBOOT = SAFE_RANGE  // Emergency reboot
};

// Manage own base layer, can be BASE or MAC
static uint8_t layer_base         = 0xff;
static uint8_t layer_current      = 0xff;
static uint8_t layer_prev         = 0xff;

layer_state_t layer_state_set_user(layer_state_t state) {
    layer_prev = layer_current;
    layer_current = get_highest_layer(state);
    switch (layer_current) {
        case LAYER_BASE_MAC:
            layer_base = LAYER_BASE_MAC;
            break;
        case LAYER_BASE_WIN:
            layer_base = LAYER_BASE_WIN;
            break;
        case LAYER_UTIL_MAC:
        case LAYER_UTIL_WIN:
            break;
        default: //  for any other layers, or the default layer
            break;
    }
    return state;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
    //  Macro keycode has X_ for mat instead of KC_
    //  https://github.com/qmk/qmk_firmware/blob/master/quantum/send_string/send_string_keycodes.h
    //  https://github.com/qmk/qmk_firmware/blob/master/docs/feature_macros.md
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
            SEND_STRING(SS_RCTL(SS_RCMD(SS_TAP(X_PWR))));
        }
        return false;
    default:
        return true;
    }
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LAYER_BASE_MAC] = LAYOUT_ansi_82(
        KC_ESC,      KC_BRID,  KC_BRIU, KC_NO,    KC_NO,    RGB_VAD, RGB_VAI, KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE, KC_VOLD,            KC_VOLU, KC_PSCR,          KC_MUTE,
        KC_GRV,      KC_1,     KC_2,    KC_3,     KC_4,     KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS,            KC_EQL,  KC_BSPC,          TD(TD_DEL_INS),
        KC_TAB,      KC_Q,     KC_W,    KC_E,     KC_R,     KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC,            KC_RBRC, KC_BSLS,          TD(TD_PGDN_PGUP),
        KC_CAPS,     KC_A,     KC_S,    KC_D,     KC_F,     KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,                     KC_ENT,           TD(TD_END_HOME),
        KC_LSFT,               KC_Z,    KC_X,     KC_C,     KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,                     KC_RSFT, KC_UP,
        KC_LCTL,     KC_LOPT,  KC_LCMD,                              KC_SPC,                             KC_RCMD, MO(LAYER_UTIL_MAC), KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT
        ),
    [LAYER_UTIL_MAC] = LAYOUT_ansi_82(
        MKC_EREBOOT, KC_F1,    KC_F2,   KC_F3,    KC_F4,    KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,             KC_F12,  XXXXXXX,          _______,
        QK_BOOT,     XXXXXXX,  XXXXXXX, XXXXXXX,  XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,            XXXXXXX, XXXXXXX,          XXXXXXX,
        RGB_TOG,     RGB_MOD,  RGB_VAI, RGB_HUI,  RGB_SAI,  RGB_SPI, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,            XXXXXXX, XXXXXXX,          XXXXXXX,
        XXXXXXX,     RGB_RMOD, RGB_VAD, RGB_HUD,  RGB_SAD,  RGB_SPD, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                     XXXXXXX,          XXXXXXX,
        XXXXXXX,               XXXXXXX, XXXXXXX,  XXXXXXX,  XXXXXXX, XXXXXXX, NK_TOGG, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                     XXXXXXX, XXXXXXX,
        XXXXXXX,     XXXXXXX,  XXXXXXX,                              XXXXXXX,                            XXXXXXX, XXXXXXX,            XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX
        ),
    [LAYER_BASE_WIN] = LAYOUT_ansi_82(
        KC_ESC,      KC_F1,    KC_F2,   KC_F3,    KC_F4,    KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,             KC_F12,  KC_PSCR,          KC_MUTE,
        KC_GRV,      KC_1,     KC_2,    KC_3,     KC_4,     KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS,            KC_EQL,  KC_BSPC,          TD(TD_DEL_INS),
        KC_TAB,      KC_Q,     KC_W,    KC_E,     KC_R,     KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC,            KC_RBRC, KC_BSLS,          TD(TD_PGDN_PGUP),
        KC_CAPS,     KC_A,     KC_S,    KC_D,     KC_F,     KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,                     KC_ENT,           TD(TD_END_HOME),
        KC_LSFT,               KC_Z,    KC_X,     KC_C,     KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,                     KC_RSFT, KC_UP,
        KC_LCTL,     KC_LCMD,  KC_LALT,                              KC_SPC,                             KC_RALT, MO(LAYER_UTIL_WIN), KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT
        ),
    [LAYER_UTIL_WIN] = LAYOUT_ansi_82(
        MKC_EREBOOT, KC_BRID,  KC_BRIU, MKC_TASK, MKC_FLXP, RGB_VAD, RGB_VAI, KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE, KC_VOLD,            KC_VOLU, XXXXXXX,          _______,
        QK_BOOT,     XXXXXXX,  XXXXXXX, XXXXXXX,  XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,            XXXXXXX, XXXXXXX,          XXXXXXX,
        RGB_TOG,     RGB_MOD,  RGB_VAI, RGB_HUI,  RGB_SAI,  RGB_SPI, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,            XXXXXXX, XXXXXXX,          XXXXXXX,
        XXXXXXX,     RGB_RMOD, RGB_VAD, RGB_HUD,  RGB_SAD,  RGB_SPD, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                     XXXXXXX,          XXXXXXX,
        XXXXXXX,               XXXXXXX, XXXXXXX,  XXXXXXX,  XXXXXXX, XXXXXXX, NK_TOGG, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                     XXXXXXX, XXXXXXX,
        XXXXXXX,     XXXXXXX,  XXXXXXX,                              XXXXXXX,                            XXXXXXX, XXXXXXX,            XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX
        )
};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [LAYER_BASE_MAC] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [LAYER_UTIL_MAC] = { ENCODER_CCW_CW(RGB_VAD, RGB_VAI)},
    [LAYER_BASE_WIN] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [LAYER_UTIL_WIN] = { ENCODER_CCW_CW(RGB_VAD, RGB_VAI)}
};
#endif // ENCODER_MAP_ENABLE

// Indicators
// https://github.com/qmk/qmk_firmware/blob/master/docs/feature_rgb_matrix.md#indicator-examples-idindicator-examples
// COLOR https://github.com/qmk/qmk_firmware/blob/master/docs/feature_rgb_matrix.md#colors-idcolors
// LED index is in board level ansi_encoder.c

// Set color of all key to black once after layer changes back to base to prevent leftover
// as some effect won't set color for all keys
// NOTE: Tried many ways of cleaning up, set all keys to black once does not seem to be enough to clear all left over
// cleanup few times after layer change seem to work

static uint8_t led_last_layer    = 0xff;
static uint8_t led_cleanup_count = 0;

bool rgb_matrix_indicators_user() {
  return false; // To override rgb_matrix_indicators_kb
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    uint8_t layer = get_highest_layer(layer_state);
    bool layer_changed = (layer_current != led_last_layer);
    bool layer_cleanup = (led_cleanup_count > 0);
    if (layer_cleanup)
        led_cleanup_count--;
    if (layer_changed) {
        led_last_layer    = layer;
        led_cleanup_count = 5;  // 5 times seems enough
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
                if (keymap_key_to_keycode(layer, (keypos_t){col,row}) >= KC_TRNS) { // keycode valid
                    switch (layer_current) {
                        // We need to clear led because of these 2
                        // If led effect not modify keys on the 2 layers then leftover will happend
                        case LAYER_BASE_MAC:
                        case LAYER_BASE_WIN:
                            break;
                        case LAYER_UTIL_MAC:
                        case LAYER_UTIL_WIN:
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
        rgb_matrix_set_color(45, RGB_BLACK);
    // Special keys for each layer
    switch(layer) {
        case LAYER_BASE_MAC:
            // Fall through
        case LAYER_BASE_WIN:
            rgb_matrix_set_color(73, RGB_RED); // WIN
            // Caps indicator
            if (host_keyboard_led_state().caps_lock)
                rgb_matrix_set_color(45, RGB_RED);
            break;
        case LAYER_UTIL_MAC:
        case LAYER_UTIL_WIN:
            rgb_matrix_set_color(0 , RGB_RED);     // EMERG reboot - esc
            rgb_matrix_set_color(15 , RGB_ORANGE); // QK_BOOT - grave
            break;
    }
    return false;
}
