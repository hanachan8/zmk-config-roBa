/* Minimal host substitutes for the ZMK transport, not the behavior under test. */
#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>

#define DT_HAS_COMPAT_STATUS_OKAY(compat) 1
#define DT_INST_FOREACH_STATUS_OKAY(fn)
#define LGUI 0x700e3
#define RGUI 0x700e7
#define LALT 0x700e2
#define TAB 0x7002b
#define HID_USAGE_KEY 7

struct device { const void *config; void *data; };
struct zmk_behavior_binding { const char *behavior_dev; uint32_t param1; uint32_t param2; };
struct zmk_behavior_binding_event { uint32_t position; int64_t timestamp; };
struct behavior_driver_api {
    int (*binding_pressed)(struct zmk_behavior_binding *, struct zmk_behavior_binding_event);
    int (*binding_released)(struct zmk_behavior_binding *, struct zmk_behavior_binding_event);
};
const struct device *zmk_behavior_get_binding(const char *name);
int zmk_keymap_layer_activate(uint8_t layer);
int zmk_keymap_layer_deactivate(uint8_t layer);
int zmk_hid_press(uint32_t key);
int zmk_hid_release(uint32_t key);
int zmk_endpoints_send_report(uint16_t page);
int zmk_behavior_invoke_binding(const struct zmk_behavior_binding *binding,
                                struct zmk_behavior_binding_event event, bool pressed);
