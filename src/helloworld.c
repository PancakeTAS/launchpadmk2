#include <stdio.h>
#include <signal.h>

#define LAUNCHPAD_IMPL
#define LAUNCHPAD_LOG_ERROR
//#define LAUNCHPAD_LOG_TRACE // uncomment to enable trace logging
#include "launchpadmk2.h"

bool should_run = true; //!< whether the main loop should continue running

/// @brief signal handler for ctrl+c
void handle_sigint(int) {
    should_run = false;
}

/// @brief handle button presses on the main 8x8 grid and right side buttons
/// @param button button number (depends on mode)
/// @param state button state
void on_noteon(uint8_t button, bool state) {
    printf("Noteon: button=%d, state=%d\n", button, state);
}

/// @brief handle button presses on the top row
/// @param button button number
/// @param state button state
void on_controller(uint8_t button, bool state) {
    printf("Controller: button=%d, state=%d\n", button, state);
}

/// @brief main function
/// @return 0 on success, 1 on failure
int main() {
    signal(SIGINT, handle_sigint); // register signal handler for ctrl+c

    // open launchpad
    launchpad_t launchpad = {
        .client_name = "alsamidi",
        .port_name = "Launchpad MK2",
        .on_noteon = on_noteon,
        .on_controller = on_controller
    };
    launchpad_status status = launchpad_open(&launchpad);
    if (status != LAUNCHPAD_STATUS_OK) {
        fprintf(stderr, "failed to open launchpad: %d\n", status);
        return 1;
    }

    // set launchpad to user 1 mode
    status = launchpad_set_mode(&launchpad, LAUNCHPAD_MODE_USER1);
    if (status != LAUNCHPAD_STATUS_OK) {
        fprintf(stderr, "failed to set mode: %d\n", status);
        return 1;
    }

    // loop until ctrl+c or error
    srand(time(NULL));
    while (should_run && status != LAUNCHPAD_STATUS_ERROR) {
        // sleep for 100ms
        usleep(100000); // 100ms

        // light a random led with a random velocity
        uint8_t led = (rand() % 80);
        bool is_controller = led >= 72; // top row uses controller messages
        uint8_t channel = 5; // user1 mode uses ch=5 (customizable through bootloader)
        uint8_t note = is_controller ? (led - 72 + 104) : (led + 36); // main grid uses notes on user1: 36-107, top row always uses notes: 104-111
        uint8_t velocity = (rand() % 127) + 1; // colors range from 1 to 127, 0 turns off the led
        status = launchpad_set_led(&launchpad, channel, note, is_controller, velocity);

        // poll launchpad
        status = launchpad_poll(&launchpad); // non-blocking
    }

    // close launchpad
    status = launchpad_close(&launchpad);
    if (status != LAUNCHPAD_STATUS_OK) {
        fprintf(stderr, "failed to close launchpad: %d\n", status);
        return 1;
    }

    snd_config_update_free_global();

    return 0;
}
