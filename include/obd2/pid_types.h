#ifndef OBD2_PID_TYPES_H
#define OBD2_PID_TYPES_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

/*
 * Allows this C header to be safely included in C++ projects
 * without name mangling issues.
 */
#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        bool check_engine_on;
        uint8_t dtc_count;

        bool misfire_test_available;
        bool fuel_system_test_available;
        bool components_test_available;

        bool is_spark_engine;

        bool misfire_test_complete;
        bool fuel_system_test_complete;
        bool components_test_complete;

        // spark engine specific

        bool vvt_test_available;
        bool vvt_test_complete;

        bool oxygen_sensor_heater_test_available;
        bool oxygen_sensor_heater_test_complete;

        bool oxygen_sensor_test_available;
        bool oxygen_sensor_test_complete;

        bool gasoline_particulate_filter_test_available;
        bool gasoline_particulate_filter_test_complete;

        bool secondary_air_system_test_available;
        bool secondary_air_system_test_complete;

        bool evaporative_system_test_available;
        bool evaporative_system_test_complete;

        bool heated_catalyst_test_available;
        bool heated_catalyst_test_complete;

        bool catalyst_test_available;
        bool catalyst_test_complete;

        // compression engine specific

        bool egr_system_test_available;
        bool egr_system_test_complete;

        bool pm_filter_monitoring_test_available;
        bool pm_filter_monitoring_test_complete;

        bool exhaust_gas_sensor_test_available;
        bool exhaust_gas_sensor_test_complete;

        // reserved bits

        bool boost_pressure_test_available;
        bool boost_pressure_test_complete;

        // reserved bits

        bool nox_scr_sensor_test_available;
        bool nox_scr_sensor_test_complete;

        bool nmhc_catalyst_test_available;
        bool nmhc_catalyst_test_complete;

    } obd2_dtc_status_t;
    
    typedef struct
    {
        uint8_t category;
        uint16_t category_number;
    } obd2_dtc_freeze_frame_t;

    typedef enum {
        MOTOR_OFF,
        OPEN_LOOP_INSUFFICIENT_TEMP,
        CLOSED_LOOP_OXYGEN_SENSOR_FEEDBACK,
        OPEN_LOOP_ENGINE_LOAD_OR_FUEL_CUT_DECEL,
        OPEN_LOOP_SYSTEM_FAIL,
        CLOSED_LOOP_ATLEAST_ONE_OXYGEN_SENSOR_FAULT_FEEDBACK_SYSTEM,
        UNKNOWN
    } fuel_system_status_t;

    typedef struct
    {
        fuel_system_status_t sys1;
        
        bool has_sys2;
        fuel_system_status_t sys2;

    } fuel_system_status_state_t;

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