#include QMK_KEYBOARD_H
#include "raw_hid.h"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(KC_GRV, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS, KC_EQL, KC_BSPC, KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LBRC, KC_RBRC, KC_BSLS, KC_CAPS, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT, KC_ENT, KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_RSFT, KC_UP, KC_SLSH, KC_LCTL, KC_LGUI, KC_LALT, KC_SPC, KC_RALT, MO(1), KC_LEFT, KC_DOWN, KC_RGHT),
    [1] = LAYOUT(KC_ESC, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, KC_TRNS, KC_TRNS, RGB_TOG, RGB_MOD, RGB_HUI, RGB_HUD, RGB_SAI, RGB_SAD, RGB_VAI, RGB_VAD, KC_TRNS, KC_TRNS, KC_VOLD, KC_VOLU, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_PSCR, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_INS, KC_HOME, KC_PGUP, KC_TRNS, RESET, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_DEL, KC_END, KC_PGDN)
};


#define RAW_EPSIZE 32
#define UNDERGLOW_END 69

#ifdef RAW_ENABLE
void rgb_matrix_set_color_override_from_data(int data_start, const uint8_t * data) {
    uint8_t index = data[data_start];
    uint8_t color = data[data_start + 1];
    if(0 <= index && index < UNDERGLOW_END) {
        rgb_matrix_set_color_override(index, color);
    }
}

void raw_hid_receive(uint8_t *data, uint8_t length) {
    switch (data[0]) {
        // First byte - message type
        // 0 - Light single rgb
        case 0: {
            // 2 byte - index
            // 3 byte - RRRGGGBB bit encoded color
            rgb_matrix_set_color_override_from_data(1, data);
        }
        break;
        // 1-2(old) - Bulk updates of given leds
        case 1: {
            // 2 byte - number of items (<= 14)
            uint8_t max_len = data[1] >= 14 ? 14 : data[1];
            // 2..28 - Pairs of index and packed color
            for(int i = 0; i < max_len; i++) {
                rgb_matrix_set_color_override_from_data(2 + i * 2, data);
            }
        }
        break;
        // Clear every override
        case 2: {
            for(int i = 0; i < UNDERGLOW_END; i++) {
                rgb_matrix_set_color_override(i, 0);
            }
        }
        break;
        case 3: {
            // Override animation
            // Override index
            uint8_t override = data[1];
            // Animation type
            uint8_t animation = data[2];
            switch (animation) {
                // Override disabled
                case 0:
                    rgb_matrix_set_animation_override_disabled(override);
                    break;
                // Blinking animation
                case 1:
                    rgb_matrix_set_animation_override_blink(override, data[3], data[4], data[5], data[6], data[7]);
                    break;
                // Hue cycle animation
                case 2:
                    rgb_matrix_set_animation_override_hue(override, data[3], data[4], data[5]);
                    break;
            }
        }
        break;
    }
}
#endif

