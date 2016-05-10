#include "hal.h"
#include "x86.h"

#define NR_KEYS 18
#define I8042_DATA_PORT 0x60

enum {KEY_STATE_RELEASED, KEY_STATE_PRESSED};

/* Only the following keys are used in NEMU-PAL. */
static const int keycode_array[] = {
    K_UP, K_DOWN, K_LEFT, K_RIGHT, K_ESCAPE,
    K_RETURN, K_SPACE, K_PAGEUP, K_PAGEDOWN, K_r,
    K_a, K_d, K_e, K_w, K_q,
    K_s, K_f, K_p
};

static int real_key_state[NR_KEYS], prog_key_state[NR_KEYS];

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
    real_key_state[index] = release ? KEY_STATE_RELEASED : KEY_STATE_PRESSED;
}

bool
process_keys(void (*key_press_callback)(int), void (*key_release_callback)(int)) {
    cli();
    for (int index = 0; index < NR_KEYS; index++) {
        if (real_key_state[index] == prog_key_state[index]) continue;
        if (real_key_state[index] == KEY_STATE_PRESSED) {
            key_press_callback(keycode_array[index]);
        } else if (real_key_state[index] == KEY_STATE_RELEASED) {
            key_release_callback(keycode_array[index]);
        }
        prog_key_state[index] = real_key_state[index];
    }
    sti();
    return false;
}
