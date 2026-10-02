/* SPDX-License-Identifier: MIT */
#pragma once

#include <stdbool.h>
#include <stdint.h>

enum android_switcher_role { ANDROID_LEFT_WIN, ANDROID_RIGHT_WIN, ANDROID_TAB };
enum android_switcher_output { OUTPUT_LEFT_GUI, OUTPUT_RIGHT_GUI, OUTPUT_ALT, OUTPUT_TAB,
                               OUTPUT_NUMBER_LAYER };

struct android_switcher_state {
    uint8_t wins;
    bool tab;
    bool history;
    bool outputs[5];
};

typedef void (*android_switcher_emit)(void *context, enum android_switcher_output output,
                                      bool pressed);

static inline void android_switcher_set(struct android_switcher_state *s,
                                        enum android_switcher_output output, bool pressed,
                                        android_switcher_emit emit, void *context) {
    if (s->outputs[output] != pressed) {
        s->outputs[output] = pressed;
        emit(context, output, pressed);
    }
}

/* False delegates a plain Tab to the existing layer-tap behavior. GUI is actually
 * released during history, rather than globally masking other keys' modifiers. */
static inline bool android_switcher_update(struct android_switcher_state *s,
                                           enum android_switcher_role role, bool pressed,
                                           android_switcher_emit emit, void *context) {
    if (role > ANDROID_TAB) {
        return false;
    }
    if (role == ANDROID_TAB) {
        if ((pressed && !s->wins && !s->history) || (!pressed && !s->tab)) {
            return false;
        }
        s->tab = pressed;
        if (pressed) {
            s->history = true;
        }
    } else {
        uint8_t bit = (uint8_t)(1u << role);
        s->wins = pressed ? (s->wins | bit) : (s->wins & ~bit);
    }
    if (!s->wins && !s->tab) {
        s->history = false;
    }

    /* Release Tab before Alt. Release GUI before starting Alt+Tab. */
    if (!s->tab) {
        android_switcher_set(s, OUTPUT_TAB, false, emit, context);
    }
    android_switcher_set(s, OUTPUT_LEFT_GUI, !s->history && (s->wins & 1), emit, context);
    android_switcher_set(s, OUTPUT_RIGHT_GUI, !s->history && (s->wins & 2), emit, context);
    android_switcher_set(s, OUTPUT_ALT, s->history, emit, context);
    if (s->tab) {
        android_switcher_set(s, OUTPUT_TAB, true, emit, context);
    }
    android_switcher_set(s, OUTPUT_NUMBER_LAYER, s->wins & 1, emit, context);
    return true;
}
