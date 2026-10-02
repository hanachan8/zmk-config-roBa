/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/android_switcher_state.h"

struct log {
    bool held[5];
    unsigned presses[5];
    unsigned releases[5];
};

static void emit(void *context, enum android_switcher_output output, bool pressed) {
    struct log *log = context;
    assert(log->held[output] != pressed); /* No duplicate downs or unmatched ups. */
    log->held[output] = pressed;
    if (pressed) {
        log->presses[output]++;
    } else {
        log->releases[output]++;
    }
    /* History must never be sent as GUI+Alt+Tab. */
    assert(!log->held[OUTPUT_ALT] ||
           (!log->held[OUTPUT_LEFT_GUI] && !log->held[OUTPUT_RIGHT_GUI]));
    if (output == OUTPUT_TAB && pressed) {
        assert(log->held[OUTPUT_ALT]);
    }
    if (output == OUTPUT_ALT && !pressed) {
        assert(!log->held[OUTPUT_TAB]);
    }
}

static void clean(struct android_switcher_state *s, struct log *log) {
    assert(!s->wins && !s->tab && !s->history);
    for (unsigned i = 0; i < 5; i++) {
        assert(!log->held[i]);
        assert(log->presses[i] == log->releases[i]);
    }
}

static void release_order(bool tab_first, enum android_switcher_role win) {
    struct android_switcher_state s = {0};
    struct log log = {0};
    assert(android_switcher_update(&s, win, true, emit, &log));
    assert(log.held[win == ANDROID_LEFT_WIN ? OUTPUT_LEFT_GUI : OUTPUT_RIGHT_GUI]);
    assert(log.held[OUTPUT_NUMBER_LAYER] == (win == ANDROID_LEFT_WIN));
    assert(android_switcher_update(&s, ANDROID_TAB, true, emit, &log));
    assert(log.held[OUTPUT_ALT] && log.held[OUTPUT_TAB]);
    android_switcher_update(&s, tab_first ? ANDROID_TAB : win, false, emit, &log);
    assert(log.held[OUTPUT_ALT]);
    assert(log.held[OUTPUT_TAB] == !tab_first);
    android_switcher_update(&s, tab_first ? win : ANDROID_TAB, false, emit, &log);
    clean(&s, &log);
}

static void repeated_tab(void) {
    struct android_switcher_state s = {0};
    struct log log = {0};
    android_switcher_update(&s, ANDROID_LEFT_WIN, true, emit, &log);
    for (unsigned i = 0; i < 4; i++) {
        assert(android_switcher_update(&s, ANDROID_TAB, true, emit, &log));
        android_switcher_update(&s, ANDROID_TAB, false, emit, &log);
        assert(log.held[OUTPUT_ALT] && !log.held[OUTPUT_TAB]);
    }
    assert(log.presses[OUTPUT_ALT] == 1 && log.presses[OUTPUT_TAB] == 4);
    android_switcher_update(&s, ANDROID_LEFT_WIN, false, emit, &log);
    clean(&s, &log);
    /* The next Win chord must work as GUI again. */
    android_switcher_update(&s, ANDROID_RIGHT_WIN, true, emit, &log);
    assert(log.held[OUTPUT_RIGHT_GUI] && !log.held[OUTPUT_ALT]);
    android_switcher_update(&s, ANDROID_RIGHT_WIN, false, emit, &log);
    clean(&s, &log);
}

/* Enumerate physically valid press/release sequences. At each prefix also release
 * all keys, checking that no modifier can remain stuck. */
static unsigned explored;
static void explore(struct android_switcher_state s, struct log log, unsigned physical, int depth) {
    explored++;
    struct android_switcher_state end = s;
    struct log end_log = log;
    for (unsigned role = 0; role < 3; role++) {
        if (physical & (1u << role)) {
            android_switcher_update(&end, role, false, emit, &end_log);
        }
    }
    clean(&end, &end_log);
    if (depth == 0) {
        return;
    }
    for (unsigned role = 0; role < 3; role++) {
        struct android_switcher_state next = s;
        struct log next_log = log;
        bool pressed = !(physical & (1u << role));
        android_switcher_update(&next, role, pressed, emit, &next_log);
        explore(next, next_log, physical ^ (1u << role), depth - 1);
    }
}

int main(void) {
    release_order(true, ANDROID_LEFT_WIN);
    release_order(false, ANDROID_LEFT_WIN);
    release_order(true, ANDROID_RIGHT_WIN);
    release_order(false, ANDROID_RIGHT_WIN);
    repeated_tab();
    struct android_switcher_state s = {0};
    struct log log = {0};
    assert(!android_switcher_update(&s, ANDROID_TAB, true, emit, &log));
    assert(!android_switcher_update(&s, ANDROID_TAB, false, emit, &log));
    clean(&s, &log);
    explore(s, log, 0, 9);
    printf("PASS: release orders, repeated Tab, plain Tab, and %u input-sequence prefixes\n", explored);
    return 0;
}
