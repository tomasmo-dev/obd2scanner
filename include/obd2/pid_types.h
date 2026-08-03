#ifndef OBD2_PID_TYPES_H
#define OBD2_PID_TYPES_H

#include <stdint.h>
#include <stddef.h>

/*
 * Allows this C header to be safely included in C++ projects
 * without name mangling issues.
 */
#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Function pointer for decoding raw OBD2 payload bytes.
     *
     * @param payload Pointer to the raw data bytes returned by the ECU.
     * @param length  Number of bytes in the payload.
     * @return The calculated physical value as a double.
     */
    typedef double (*obd2_pid_decoder_fn)(const uint8_t *payload, size_t length);

    /**
     * @brief Defines the metadata and calculation logic for a single OBD2 PID.
     */
    typedef struct
    {
        uint8_t pid;      /* The PID hex code (e.g., 0x0C) */
        const char *name; /* e.g., "Engine RPM" */
        const char *unit; /* e.g., "rpm", "km/h" */

        obd2_pid_decoder_fn decoder; /* Function pointer to calculate the value */

        uint8_t expected_bytes;  /* Expected payload length for validation */
        double min_value;        /* Bounding minimum value */
        double max_value;        /* Bounding maximum value */
        const char *description; /* Detailed description */
    } obd2_pid_definition_t;

    /**
     * @brief Looks up a standard PID definition by its hex code.
     *
     * @param pid The PID code to look up (e.g., 0x0C).
     * @return Pointer to the PID definition, or NULL if not supported/found.
     */
    const obd2_pid_definition_t *obd2_get_pid_definition(uint8_t pid);

#ifdef __cplusplus
}
#endif

#endif /* OBD2_PID_TYPES_H */