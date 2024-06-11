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

#endif

#ifdef __cplusplus
}
#endif

#endif // LAUNCHPAD_H
