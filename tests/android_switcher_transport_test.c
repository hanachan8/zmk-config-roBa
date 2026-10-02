/* Exercise the production driver callbacks and actual HID publication boundaries. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#define ROBA_SWITCHER_HOST_TEST
#include "../src/behavior_android_switcher.c"

enum { LEFT = 1, RIGHT = 2, ALT = 4, TAB_BIT = 8 };
static struct android_switcher_state state;
static const struct switcher_config config = {
    .plain_tab = {.behavior_dev = "lt", .param1 = 2, .param2 = TAB},
    .number_layer = 5,
};
static const struct device device = {.config = &config, .data = &state};
static unsigned hid, reports[32], report_count, plain_downs, plain_ups;
static bool number_layer;

const struct device *zmk_behavior_get_binding(const char *name) {
    assert(strcmp(name, "android_switcher") == 0);
    return &device;
}
int zmk_keymap_layer_activate(uint8_t layer) { assert(layer == 5); number_layer = true; return 0; }
int zmk_keymap_layer_deactivate(uint8_t layer) { assert(layer == 5); number_layer = false; return 0; }
static unsigned bit(uint32_t key) {
    switch (key) {
    case LGUI: return LEFT;
    case RGUI: return RIGHT;
    case LALT: return ALT;
    case TAB: return TAB_BIT;
    default: assert(false); return 0;
    }
}
int zmk_hid_press(uint32_t key) { assert(!(hid & bit(key))); hid |= bit(key); return 0; }
int zmk_hid_release(uint32_t key) { assert(hid & bit(key)); hid &= ~bit(key); return 0; }
int zmk_endpoints_send_report(uint16_t page) {
    assert(page == HID_USAGE_KEY && report_count < 32);
    reports[report_count++] = hid;
    return 0;
}
int zmk_behavior_invoke_binding(const struct zmk_behavior_binding *binding,
                                struct zmk_behavior_binding_event event, bool down) {
    assert(strcmp(binding->behavior_dev, "lt") == 0);
    assert(binding->param1 == 2 && binding->param2 == TAB);
    assert(event.position == 39);
    if (down) { plain_downs++; } else { plain_ups++; }
    return 0;
}
static void key(enum android_switcher_role role, bool down) {
    struct zmk_behavior_binding binding = {.behavior_dev = "android_switcher", .param1 = role};
    const unsigned positions[] = {36, 16, 39};
    struct zmk_behavior_binding_event event = {.position = positions[role], .timestamp = 100};
    int err = down ? switcher_api.binding_pressed(&binding, event)
                   : switcher_api.binding_released(&binding, event);
    assert(err == 0);
}
static void reset(void) {
    assert(hid == 0 && !state.history && !number_layer);
    report_count = plain_downs = plain_ups = 0;
    memset(&state, 0, sizeof(state));
}

int main(void) {
    for (unsigned win = ANDROID_LEFT_WIN; win <= ANDROID_RIGHT_WIN; win++) {
        for (unsigned tab_first = 0; tab_first < 2; tab_first++) {
            reset();
            key(win, true);
            assert(number_layer == (win == ANDROID_LEFT_WIN));
            key(ANDROID_TAB, true);
            // No empty GUI-release report or standalone Alt report may intervene.
            assert(report_count == 2);
            assert(reports[0] == (win == ANDROID_LEFT_WIN ? LEFT : RIGHT));
            assert(reports[1] == (ALT | TAB_BIT));
            key(tab_first ? ANDROID_TAB : win, false);
            assert(hid == (tab_first ? ALT : (ALT | TAB_BIT)));
            key(tab_first ? win : ANDROID_TAB, false);
            assert(reports[report_count - 1] == 0 && hid == 0);
            assert(plain_downs == 0 && plain_ups == 0);
        }
    }
    reset();
    key(ANDROID_RIGHT_WIN, true);
    for (unsigned i = 0; i < 3; i++) {
        key(ANDROID_TAB, true);
        assert(hid == (ALT | TAB_BIT));
        key(ANDROID_TAB, false);
        assert(hid == ALT);
    }
    key(ANDROID_RIGHT_WIN, false);
    assert(report_count == 8 && reports[7] == 0);
    key(ANDROID_TAB, true);
    key(ANDROID_TAB, false);
    assert(plain_downs == 1 && plain_ups == 1 && report_count == 8);
    reset();
    puts("PASS: driver callbacks, atomic HID reports, both release orders, repeated Tab and fallback");
    return 0;
}
