/* SPDX-License-Identifier: MIT */
#define DT_DRV_COMPAT roba_behavior_android_switcher

#ifdef ROBA_SWITCHER_HOST_TEST
#include "../tests/switcher_zmk_shim.h"
#else
#include <errno.h>
#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zmk/behavior.h>
#include <zmk/keymap.h>
#include <zmk/hid.h>
#include <zmk/endpoints.h>
#include <dt-bindings/zmk/keys.h>
#endif

#include "android_switcher_state.h"

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

struct switcher_config {
    struct zmk_behavior_binding plain_tab;
    uint8_t number_layer;
};

struct switcher_context {
    const struct switcher_config *config;
    bool report_changed;
    int error;
};

static void emit_output(void *context, enum android_switcher_output output, bool pressed) {
    struct switcher_context *ctx = context;
    int err;
    if (output == OUTPUT_NUMBER_LAYER) {
        err = pressed ? zmk_keymap_layer_activate(ctx->config->number_layer)
                      : zmk_keymap_layer_deactivate(ctx->config->number_layer);
    } else {
        static const uint32_t keys[] = {LGUI, RGUI, LALT, TAB};
        // Apply all changes first. Publishing GUI-up, Alt-down and Tab-down as
        // separate reports exposes a standalone GUI tap to the host launcher.
        err = pressed ? zmk_hid_press(keys[output]) : zmk_hid_release(keys[output]);
        if (err >= 0) {
            ctx->report_changed = true;
        }
    }
    if (err < 0 && ctx->error == 0) {
        ctx->error = err;
    }
}

static int update(struct zmk_behavior_binding *binding,
                  struct zmk_behavior_binding_event event, bool pressed) {
    if (binding->param1 > ANDROID_TAB) {
        return -EINVAL;
    }
    const struct device *dev = zmk_behavior_get_binding(binding->behavior_dev);
    const struct switcher_config *config = dev->config;
    struct android_switcher_state *state = dev->data;
    struct switcher_context context = {.config = config};
    if (!android_switcher_update(state, binding->param1, pressed, emit_output, &context)) {
        return zmk_behavior_invoke_binding(&config->plain_tab, event, pressed);
    }
    if (context.report_changed) {
        int err = zmk_endpoints_send_report(HID_USAGE_KEY);
        if (err < 0 && context.error == 0) {
            context.error = err;
        }
    }
    return context.error;
}

static int pressed(struct zmk_behavior_binding *binding, struct zmk_behavior_binding_event event) {
    return update(binding, event, true);
}

static int released(struct zmk_behavior_binding *binding, struct zmk_behavior_binding_event event) {
    return update(binding, event, false);
}

static const struct behavior_driver_api switcher_api = {
    .binding_pressed = pressed,
    .binding_released = released,
};

#define SWITCHER_INST(n)                                                                            \
    static struct android_switcher_state switcher_state_##n;                                        \
    static const struct switcher_config switcher_config_##n = {                                     \
        .plain_tab = ZMK_KEYMAP_EXTRACT_BINDING(0, DT_DRV_INST(n)),                                  \
        .number_layer = DT_INST_PROP(n, number_layer),                                              \
    };                                                                                             \
    BEHAVIOR_DT_INST_DEFINE(n, NULL, NULL, &switcher_state_##n, &switcher_config_##n,                 \
                            POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &switcher_api);

DT_INST_FOREACH_STATUS_OKAY(SWITCHER_INST)
#endif
