/// \file launchpadmk2.h generic launchpad mk2 driver

// (make sure to define LAUNCHPAD_IMPL in one of the source files and optionally LAUNCHPAD_LOG_ERROR and LAUNCHPAD_LOG_TRACE)

#ifndef LAUNCHPAD_H
#define LAUNCHPAD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <alsa/asoundlib.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    char* port_name; //!< [in] name of the launchpad port (containing string, can be NULL)
    char* client_name; //!< [in] name of the alsa client

    void (*on_noteon)(uint8_t button, bool state); //!< [in] noteon event callback (can be NULL)
    void (*on_controller)(uint8_t button, bool state); //!< [in] controller event callback (can be NULL)

    snd_seq_t* seq_handle; //!< sequencer handle
    int seq_in; //!< in port
    int seq_out; //!< out port
} launchpad_t; //!< launchpad device handle

typedef enum {
    LAUNCHPAD_STATUS_OK = 0, //!< success
    LAUNCHPAD_STATUS_ERROR = 1, //!< error
    LAUNCHPAD_STATUS_NO_EVENTS = -1, //!< no events
} launchpad_status; //!< launchpad status

#ifndef LAUNCHPAD_IMPL

// device functions

/// @brief connect to launchpad
/// @param launchpad launchpad device handle
/// @return ::LAUNCHPAD_SUCCESS, ::LAUNCHPAD_ERROR
launchpad_status launchpad_open(launchpad_t* launchpad);

/// @brief poll launchpad for events
/// @param launchpad launchpad device handle
/// @return ::LAUNCHPAD_SUCCESS, ::LAUNCHPAD_ERROR, ::LAUNCHPAD_NO_EVENTS
launchpad_status launchpad_poll(launchpad_t* launchpad);

/// @brief close launchpad connection
/// @param launchpad launchpad device handle
/// @return ::LAUNCHPAD_SUCCESS, ::LAUNCHPAD_ERROR
launchpad_status launchpad_close(launchpad_t* launchpad);

// main functions

/// @brief set led of launchpad
/// @param launchpad launchpad device handle
/// @param channel led channel to send to
/// @param idx led index (11 to 111)
/// @param is_controller is controller led (top row)
/// @param color led color (0 to 127)
launchpad_status launchpad_set_led(launchpad_t* launchpad, uint8_t channel, uint8_t idx, bool is_controller, uint8_t color);

/// @brief flash led of launchpad
/// @param launchpad launchpad device handle
/// @param idx led index (11 to 111)
/// @param is_controller is controller led (top row)
/// @param color led color (0 to 127)
launchpad_status launchpad_flash_led(launchpad_t* launchpad, uint8_t idx, bool is_controller, uint8_t color);

/// @brief pulase led of launchpad
/// @param launchpad launchpad device handle
/// @param idx led index (11 to 111)
/// @param is_controller is controller led (top row)
/// @param color led color (0 to 127)
launchpad_status launchpad_pulse_led(launchpad_t* launchpad, uint8_t idx, bool is_controller, uint8_t color);

// clock functions

/// @brief send midi clock signal (24 ppm, 40 to 240 bpm)
/// @param launchpad launchpad device handle
/// @return ::LAUNCHPAD_SUCCESS, ::LAUNCHPAD_ERROR
launchpad_status launchpad_send_clock(launchpad_t* launchpad);

// sysex functions

/// @brief set leds of launchpad
/// @param launchpad launchpad device handle
/// @param leds_idx leds index (11 to 111)
/// @param leds_col leds color (0 to 127)
/// @param size size of leds_idx and leds_col (up to 80)
/// @return ::LAUNCHPAD_SUCCESS, ::LAUNCHPAD_ERROR
launchpad_status launchpad_set_leds(launchpad_t* launchpad, uint8_t* leds_idx, uint8_t* leds_col, int size);

/// @brief set leds of launchpad in rgb mode
/// @param launchpad launchpad device handle
/// @param leds_idx leds index (11 to 111)
/// @param leds_col leds color (r, g, b; 0 to 63)
/// @param size size of leds_idx and leds_col (up to 80)
/// @return ::LAUNCHPAD_SUCCESS, ::LAUNCHPAD_ERROR
launchpad_status launchpad_set_leds_rgb(launchpad_t* launchpad, uint8_t* leds_idx, uint8_t* leds_col, int size);

/// @brief set leds of launchpad by column
/// @param launchpad launchpad device handle
/// @param col_idx column index (0 to 8)
/// @param col_col column color (0 to 127)
/// @param size size of col_idx and col_col (up to 9)
/// @return ::LAUNCHPAD_SUCCESS, ::LAUNCHPAD_ERROR
launchpad_status launchpad_set_leds_col(launchpad_t* launchpad, uint8_t* col_idx, uint8_t* col_col, int size);

/// @brief set leds of launchpad by row
/// @param launchpad launchpad device handle
/// @param row_idx row index (0 to 8)
/// @param row_col row color (0 to 127)
/// @param size size of row_idx and row_col (up to 9)
/// @return ::LAUNCHPAD_SUCCESS, ::LAUNCHPAD_ERROR
launchpad_status launchpad_set_leds_row(launchpad_t* launchpad, uint8_t* row_idx, uint8_t* row_col, int size);

/// @brief set all leds of launchpad
/// @param launchpad launchpad device handle
/// @param color led color (0 to 127)
/// @return ::LAUNCHPAD_SUCCESS, ::LAUNCHPAD_ERROR
launchpad_status launchpad_set_leds_all(launchpad_t* launchpad, uint8_t color);

/// @brief flash leds of launchpad
/// @param launchpad launchpad device handle
/// @param leds_idx leds index (11 to 111)
/// @param leds_col leds color (0 to 127)
/// @param size size of leds_idx and leds_col (up to 80)
/// @return ::LAUNCHPAD_SUCCESS, ::LAUNCHPAD_ERROR
launchpad_status launchpad_flash_leds(launchpad_t* launchpad, uint8_t* leds_idx, uint8_t* leds_col, int size);

/// @brief pulse leds of launchpad
/// @param launchpad launchpad device handle
/// @param leds_idx leds index (11 to 111)
/// @param leds_col leds color (0 to 127)
/// @param size size of leds_idx and leds_col (up to 80)
/// @return ::LAUNCHPAD_SUCCESS, ::LAUNCHPAD_ERROR
launchpad_status launchpad_pulse_leds(launchpad_t* launchpad, uint8_t* leds_idx, uint8_t* leds_col, int size);

/// @brief scroll text on launchpad
/// @param launchpad launchpad device handle
/// @param text ascii text to scroll (use plain 1-7 to control speed, default is 4)
/// @param color color to scroll (0 to 127)
/// @param loop should loop
/// @return ::LAUNCHPAD_SUCCESS, ::LAUNCHPAD_ERROR
launchpad_status launchpad_scroll_text(launchpad_t* launchpad, char* text, uint8_t color, bool loop);

#else

#ifdef LAUNCHPAD_LOG_ERROR
#define log_error(...) fprintf(stderr, "ERROR: "); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n");
#else
#define log_error(...)
#endif

#ifdef LAUNCHPAD_LOG_TRACE
#define log_trace(...) fprintf(stderr, "TRACE: "); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n");
#else
#define log_trace(...)
#endif

/// @brief assert return alsa status and log error
/// @param status alsa status
/// @param operation operation name
/// @param success success message
/// @return ::LAUNCHPAD_ERROR on error
#define ALSA_ASSERT(status, operation, success) \
    if (status < 0) { \
        log_error(operation " failed: %s", snd_strerror(status)); \
        return LAUNCHPAD_STATUS_ERROR; \
    } \
    log_trace(success);

// device functions

launchpad_status launchpad_open(launchpad_t* launchpad) {
    // open sequencer
    int status = snd_seq_open(&launchpad->seq_handle, "default", SND_SEQ_OPEN_DUPLEX, SND_SEQ_NONBLOCK);
    ALSA_ASSERT(status, "snd_seq_open()", "sequencer opened");
    status = snd_seq_set_client_name(launchpad->seq_handle, launchpad->client_name);
    ALSA_ASSERT(status, "snd_seq_set_client_name()", "sequencer client name set");
    launchpad->seq_in = snd_seq_create_simple_port(launchpad->seq_handle, "device:in", SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE, SND_SEQ_PORT_TYPE_APPLICATION);
    ALSA_ASSERT(status, "snd_seq_create_simple_port()", "sequencer input port created");
    launchpad->seq_out = snd_seq_create_simple_port(launchpad->seq_handle, "device:out", SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ, SND_SEQ_PORT_TYPE_APPLICATION);
    ALSA_ASSERT(status, "snd_seq_create_simple_port()", "sequencer output port created");
    log_trace("new launchpad device created");

    if (!launchpad->port_name)
        return LAUNCHPAD_STATUS_OK;

    // find launchpad client if provided
    snd_seq_client_info_t* client_info;
    snd_seq_client_info_alloca(&client_info);
    int client_id = -1;

    while (snd_seq_query_next_client(launchpad->seq_handle, client_info) >= 0) {
        if (strstr(snd_seq_client_info_get_name(client_info), launchpad->port_name)) {
            client_id = snd_seq_client_info_get_client(client_info);
            log_trace("found launchpad client");
            break;
        }
    }

    if (client_id < 0) {
        log_error("launchpad client not found");
        return LAUNCHPAD_STATUS_ERROR;
    }

    // find launchpad ports and connect
    snd_seq_port_info_t* port_info;
    snd_seq_port_info_alloca(&port_info);

    snd_seq_port_info_set_client(port_info, client_id);
    snd_seq_port_info_set_port(port_info, -1);

    while (snd_seq_query_next_port(launchpad->seq_handle, port_info) >= 0) {
        unsigned int caps = snd_seq_port_info_get_capability(port_info);

        if (caps & (SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ)) {
            status = snd_seq_connect_from(launchpad->seq_handle, launchpad->seq_in, client_id, snd_seq_port_info_get_port(port_info));
            ALSA_ASSERT(status, "snd_seq_connect_from()", "connected to launchpad input port");
        }

        if (caps & (SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE)) {
            status = snd_seq_connect_to(launchpad->seq_handle, launchpad->seq_out, client_id, snd_seq_port_info_get_port(port_info));
            ALSA_ASSERT(status, "snd_seq_connect_to()", "connected to launchpad output port");
        }
    }

    // drop input events
    usleep(20000); // wait 0.02s
    status = snd_seq_drop_input(launchpad->seq_handle);
    ALSA_ASSERT(status, "snd_seq_drop_input()", "dropped input events");
    status = snd_seq_drop_input_buffer(launchpad->seq_handle);
    ALSA_ASSERT(status, "snd_seq_drop_input_buffer()", "dropped input buffer");
    log_trace("connected to launchpad ports");

    return LAUNCHPAD_STATUS_OK;
}

launchpad_status launchpad_poll(launchpad_t* launchpad) {
    snd_seq_event_t *ev;

    // poll for events
    int status = snd_seq_event_input(launchpad->seq_handle, &ev);
    if (status < 0) {
        if (status == -EAGAIN)
            return LAUNCHPAD_STATUS_NO_EVENTS;

        log_error("snd_seq_event_input() failed: %s", snd_strerror(status));
        return LAUNCHPAD_STATUS_ERROR;
    }
    log_trace("event polled");

    // handle event
    if (ev->type == SND_SEQ_EVENT_NOTEON && launchpad->on_noteon)
        launchpad->on_noteon(ev->data.note.note, ev->data.note.velocity == 127);
    else if (ev->type == SND_SEQ_EVENT_CONTROLLER && launchpad->on_controller)
        launchpad->on_controller(ev->data.control.param, ev->data.control.value == 127);

    return LAUNCHPAD_STATUS_OK;
}

launchpad_status launchpad_close(launchpad_t* launchpad) {
    // destroy sequencer ports
    int status = snd_seq_delete_port(launchpad->seq_handle, launchpad->seq_out);
    ALSA_ASSERT(status, "snd_seq_delete_port()", "output port deleted");
    status = snd_seq_delete_port(launchpad->seq_handle, launchpad->seq_in);
    ALSA_ASSERT(status, "snd_seq_delete_port()", "input port deleted");
    status = snd_seq_close(launchpad->seq_handle);
    ALSA_ASSERT(status, "snd_seq_close()", "sequencer closed");
    log_trace("launchpad device closed");
    return LAUNCHPAD_STATUS_OK;
}


// main functions


/// @brief prepare alsa event
/// @param launchpad launchpad device handle
#define ALSA_PREPARE_EVENT(launchpad) \
    snd_seq_event_t ev; \
    snd_seq_ev_clear(&ev); \
    snd_seq_ev_set_source(&ev, launchpad->seq_out); \
    snd_seq_ev_set_subs(&ev); \
    snd_seq_ev_set_direct(&ev);

/// @brief send alsa event
/// @param launchpad launchpad device handle
#define ALSA_SEND_EVENT(launchpad, ev) \
    int status = snd_seq_event_output(launchpad->seq_handle, &ev); \
    ALSA_ASSERT(status, "snd_seq_event_output()", "event sent"); \
    status = snd_seq_drain_output(launchpad->seq_handle); \
    ALSA_ASSERT(status, "snd_seq_drain_output()", "event flushed");


launchpad_status launchpad_set_led(launchpad_t *launchpad, uint8_t channel, uint8_t idx, bool is_controller, uint8_t color) {
    ALSA_PREPARE_EVENT(launchpad);
    if (is_controller) snd_seq_ev_set_controller(&ev, channel, idx, color);
    else snd_seq_ev_set_noteon(&ev, channel, idx, color);
    ALSA_SEND_EVENT(launchpad, ev);
    return LAUNCHPAD_STATUS_OK;
}


#define LAUNCHPAD_FLASH_CH 1 //!< channel for flashing leds

launchpad_status launchpad_flash_led(launchpad_t *launchpad, uint8_t idx, bool is_controller, uint8_t color) {
    return launchpad_set_led(launchpad, LAUNCHPAD_FLASH_CH, idx, is_controller, color);
}


#define LAUNCHPAD_PULSE_CH 2 //!< channel for pulsing leds

launchpad_status launchpad_pulse_led(launchpad_t *launchpad, uint8_t idx, bool is_controller, uint8_t color) {
    return launchpad_set_led(launchpad, LAUNCHPAD_PULSE_CH, idx, is_controller, color);
}


// clock functions


launchpad_status launchpad_send_clock(launchpad_t* launchpad) {
    ALSA_PREPARE_EVENT(launchpad);
    ev.type = SND_SEQ_EVENT_CLOCK;
    ALSA_SEND_EVENT(launchpad, ev);
    return LAUNCHPAD_STATUS_OK;
}


// sysex functions


/// @brief prepare sysex message
/// @param sysex sysex message
/// @param size sysex message size
/// @param control sysex control byte
#define ALSA_PREPARE_SYSEX(sysex, size, control) \
    sysex[0] = 0xF0; \
    sysex[1] = 0x00; \
    sysex[2] = 0x20; \
    sysex[3] = 0x29; \
    sysex[4] = 0x02; \
    sysex[5] = 0x18; \
    sysex[6] = control; \
    sysex[size - 1] = 0xF7;

static launchpad_status launchpad_send_sysex(launchpad_t* launchpad, uint8_t* sysex, size_t size) {
    ALSA_PREPARE_EVENT(launchpad);
    snd_seq_ev_set_sysex(&ev, size, sysex);
    ALSA_SEND_EVENT(launchpad, ev);
    return LAUNCHPAD_STATUS_OK;
}


#define LAUNCHPAD_SETLEDS_CTRL 0x0A //!< sysex control byte for setting leds

launchpad_status launchpad_set_leds(launchpad_t* launchpad, uint8_t* leds_idx, uint8_t* leds_col, int size) {
    uint8_t sysex[128];
    int len = 8 + size * 2;
    ALSA_PREPARE_SYSEX(sysex, len, LAUNCHPAD_SETLEDS_CTRL)

    for (int i = 0; i < size; i++) {
        sysex[7 + i * 2] = leds_idx[i];
        sysex[8 + i * 2] = leds_col[i];
    }

    return launchpad_send_sysex(launchpad, sysex, len);
}


#define LAUNCHPAD_SETLEDSRGB_CTRL 0x0B //!< sysex control byte for setting leds in rgb mode

launchpad_status launchpad_set_leds_rgb(launchpad_t* launchpad, uint8_t* leds_idx, uint8_t* leds_col, int size) {
    uint8_t sysex[512];
    int len = 8 + size * 4;
    ALSA_PREPARE_SYSEX(sysex, len, LAUNCHPAD_SETLEDSRGB_CTRL)

    for (int i = 0; i < size; i++) {
        sysex[7 + i * 4] = leds_idx[i];
        sysex[8 + i * 4] = leds_col[i * 3];
        sysex[9 + i * 4] = leds_col[i * 3 + 1];
        sysex[10 + i * 4] = leds_col[i * 3 + 2];
    }

    return launchpad_send_sysex(launchpad, sysex, len);
}


#define LAUNCHPAD_SETLEDS_COL_CTRL 0x0C //!< sysex control byte for setting leds by column
#define LAUNCHPAD_SETLEDS_ROW_CTRL 0x0D //!< sysex control byte for setting leds by row

static launchpad_status launchpad_set_leds_colrow(launchpad_t* launchpad, uint8_t* col_idx, uint8_t* col_col, int size, uint8_t control) {
    uint8_t sysex[128];
    int len = 8 + size * 2;
    ALSA_PREPARE_SYSEX(sysex, len, control)

    for (int i = 0; i < size; i++) {
        sysex[7 + i * 2] = col_idx[i];
        sysex[8 + i * 2] = col_col[i];
    }

    return launchpad_send_sysex(launchpad, sysex, len);
}

launchpad_status launchpad_set_leds_col(launchpad_t* launchpad, uint8_t* col_idx, uint8_t* col_col, int size) {
    return launchpad_set_leds_colrow(launchpad, col_idx, col_col, size, LAUNCHPAD_SETLEDS_COL_CTRL);
}

launchpad_status launchpad_set_leds_row(launchpad_t* launchpad, uint8_t* row_idx, uint8_t* row_col, int size) {
    return launchpad_set_leds_colrow(launchpad, row_idx, row_col, size, LAUNCHPAD_SETLEDS_ROW_CTRL);
}


#define LAUNCHPAD_SETLEDS_ALL_CTRL 0x0E //!< sysex control byte for setting all leds

launchpad_status launchpad_set_leds_all(launchpad_t* launchpad, uint8_t color) {
    uint8_t sysex[128];
    int len = 9;
    ALSA_PREPARE_SYSEX(sysex, len, LAUNCHPAD_SETLEDS_ALL_CTRL)

    sysex[7] = color;

    return launchpad_send_sysex(launchpad, sysex, len);
}


#define LAUNCHPAD_FLASH_CTRL 0x23 //!< sysex control byte for flashing leds
#define LAUNCHPAD_PULSE_CTRL 0x28 //!< sysex control byte for pulsing leds

static launchpad_status launchpad_flashpulse_leds(launchpad_t* launchpad, uint8_t* leds_idx, uint8_t* leds_col, int size, uint8_t control) {
    uint8_t sysex[128];
    int len = 8 + size * 3;
    ALSA_PREPARE_SYSEX(sysex, len, control)

    for (int i = 0; i < size; i++) {
        sysex[7 + i * 3] = 0;
        sysex[8 + i * 3] = leds_idx[i];
        sysex[9 + i * 3] = leds_col[i];
    }

    return launchpad_send_sysex(launchpad, sysex, len);
}

launchpad_status launchpad_flash_leds(launchpad_t* launchpad, uint8_t* leds_idx, uint8_t* leds_col, int size) {
    return launchpad_flashpulse_leds(launchpad, leds_idx, leds_col, size, LAUNCHPAD_FLASH_CTRL);
}

launchpad_status launchpad_pulse_leds(launchpad_t* launchpad, uint8_t* leds_idx, uint8_t* leds_col, int size) {
    return launchpad_flashpulse_leds(launchpad, leds_idx, leds_col, size, LAUNCHPAD_PULSE_CTRL);
}


#define LAUNCHPAD_SCROLL_CTRL 0x14 //!< sysex control byte for scrolling text

launchpad_status launchpad_scroll_text(launchpad_t* launchpad, char* text, uint8_t color, bool loop) {
    uint8_t sysex[512];
    int text_len = strlen(text);
    int len = 10 + text_len;
    ALSA_PREPARE_SYSEX(sysex, len, LAUNCHPAD_SCROLL_CTRL)

    sysex[7] = color;
    sysex[8] = loop ? 1 : 0;

    for (int i = 0; i < text_len; i++) {
        sysex[9 + i] = text[i];
    }

    return launchpad_send_sysex(launchpad, sysex, len);
}

#endif

#ifdef __cplusplus
}
#endif

#endif // LAUNCHPAD_H
