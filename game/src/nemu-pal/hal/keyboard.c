#include "hal.h"
#include "x86.h"

#define NR_KEYS 18
#define I8042_DATA_PORT 0x60

enum {KEY_STATE_RELEASED, KEY_STATE_PRESS, KEY_STATE_PRESSED, KEY_STATE_PRESS_RELEASE, KEY_STATE_RELEASE};

/* Only the following keys are used in NEMU-PAL. */
static const int keycode_array[] = {
    K_UP, K_DOWN, K_LEFT, K_RIGHT, K_ESCAPE,
    K_RETURN, K_SPACE, K_PAGEUP, K_PAGEDOWN, K_r,
    K_a, K_d, K_e, K_w, K_q,
    K_s, K_f, K_p
};

static int key_state[NR_KEYS];

void
keyboard_event(void) {
    uint8_t scancode = in_byte(I8042_DATA_PORT);
    bool release = !!(scancode & 0x80);
    scancode = scancode & ~(0x80);
    int index = -1;
    for (size_t i = 0; i < NR_KEYS; i++) {
        if (scancode == keycode_array[i]) {
            index = i;
            break;
        }
    }
    if (index == -1) return;
    if (!release) {
        if (key_state[index] == KEY_STATE_RELEASE || key_state[index] == KEY_STATE_PRESSED) {
            key_state[index] = KEY_STATE_PRESSED;
        } else {
            key_state[index] = KEY_STATE_PRESS;
        }
    } else {
        if (key_state[index] == KEY_STATE_PRESS) {
            key_state[index] = KEY_STATE_PRESS_RELEASE;
        } else if (key_state[index] == KEY_STATE_PRESSED) {
            key_state[index] = KEY_STATE_RELEASE;
        } else Log("warning: unexpected key state!");
    }
}

bool 
process_keys(void (*key_press_callback)(int), void (*key_release_callback)(int)) {
    cli();
    for (int index = 0; index < NR_KEYS; index++) {
        if (key_state[index] == KEY_STATE_PRESS) {
            key_press_callback(keycode_array[index]);
            key_state[index] = KEY_STATE_PRESSED;
        } else if (key_state[index] == KEY_STATE_PRESS_RELEASE) {
            key_press_callback(keycode_array[index]);
            key_state[index] = KEY_STATE_RELEASE;
        } else if (key_state[index] == KEY_STATE_RELEASE) {
            key_release_callback(keycode_array[index]);
            key_state[index] = KEY_STATE_RELEASED;
        }
    }
    sti();
    return false;
}
