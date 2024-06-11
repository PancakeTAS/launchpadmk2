/// \file launchpadmk2.h generic launchpad mk2 driver

// (make sure to define LAUNCHPAD_IMPL in one of the source files and optionally LAUNCHPAD_LOG_ERROR and LAUNCHPAD_LOG_TRACE)

#ifndef LAUNCHPAD_H
#define LAUNCHPAD_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {

} launchpad_t; //!< launchpad device handle

#ifndef LAUNCHPAD_IMPL

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

#endif

#ifdef __cplusplus
}
#endif

#endif // LAUNCHPAD_H
