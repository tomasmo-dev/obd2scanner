#include "obd2/pid_types.h"
#include <stdbool.h>

// ---- high level decoders ----

bool is_pid_supported(double decoded_mask, uint8_t target_pid) {
    uint32_t bitmask = (uint32_t)decoded_mask;

    uint8_t relative_position = target_pid % 32;
    if (relative_position == 0) relative_position = 32;

    uint8_t shift_by = 32 - relative_position;
    uint32_t check_mask = 1U << shift_by;

    return (bitmask & check_mask) != 0;
}

obd2_dtc_status_t parse_dtc_since_cleared(double decoded_mask) {
    uint32_t raw = (uint32_t)decoded_mask;

    uint8_t byte_A = (raw >> 24) & 0xFF;
    uint8_t byte_B = (raw >> 16) & 0xFF;
    uint8_t byte_C = (raw >> 8) & 0xFF;
    uint8_t byte_D = raw & 0xFF;

    bool check_engine = (byte_A & 0b10000000) != 0;
    uint8_t dtc_cnt = (byte_A   & 0b01111111);

    // byte_b & 0b10000000 -> reserved, should be 0

    bool misfire_available_dtc = (byte_B & (1 << 0)) != 0;
    bool fuel_system_available_dtc = (byte_B & (1 << 1)) != 0;
    bool components_available_dtc = (byte_B & (1 << 2)) != 0;

    bool is_spark_engine = (byte_B & (1 << 3)) == 0; // if true then its spark engine, else compression engine

    bool vvt_test_available;
    bool oxygen_sensor_heater_test_available;
    bool oxygen_sensor_test_available;
    bool gasoline_particulate_filter_test_available;
    bool secondary_air_system_test_available;
    bool evaporative_system_test_available;
    bool heated_catalyst_test_available;
    bool catalyst_test_available;

    bool egr_system_test_available;
    bool pm_filter_monitoring_test_available;
    bool exhaust_gas_sensor_test_available;
    bool boost_pressure_test_available;
    bool nox_scr_sensor_test_available;
    bool nmhc_catalyst_test_available;


    if (is_spark_engine) {
        vvt_test_available = (byte_C & (1 << 7)) == 1;
        oxygen_sensor_heater_test_available = (byte_C & (1 << 6)) == 1;
        oxygen_sensor_test_available = (byte_C & (1 << 5)) == 1;
        gasoline_particulate_filter_test_available = (byte_C & (1 << 4)) == 1;
        secondary_air_system_test_available = (byte_C & (1 << 3)) == 1;
        evaporative_system_test_available = (byte_C & (1 << 2)) == 1;
        heated_catalyst_test_available = (byte_C & (1 << 1)) == 1;
        catalyst_test_available = (byte_C & (1 << 0)) == 1;

    } else {
        egr_system_test_available = (byte_C & (1 << 7)) == 1;
        pm_filter_monitoring_test_available = (byte_C & (1 << 6)) == 1;
        exhaust_gas_sensor_test_available = (byte_C & (1 << 5)) == 1;
        boost_pressure_test_available = (byte_C & (1 << 4)) == 1;
        nox_scr_sensor_test_available = (byte_C & (1 << 3)) == 1;
        nmhc_catalyst_test_available = (byte_C & (1 << 2)) == 1;
    }


    // unsupported tests are always 0 (true in this case)

    bool misfire_complete_dtc = (byte_B & (1 << 4)) == 0;
    bool fuel_system_complete_dtc = (byte_B & (1 << 5)) == 0;
    bool components_complete_dtc = (byte_B & (1 << 6)) == 0;

    // 7. bit is reserved and unused

    bool vvt_test_complete;
    bool oxygen_sensor_heater_test_complete;
    bool oxygen_sensor_test_complete;
    bool gasoline_particulate_filter_test_complete;
    bool secondary_air_system_test_complete;
    bool evaporative_system_test_complete;
    bool heated_catalyst_test_complete;
    bool catalyst_test_complete;

    bool egr_system_test_complete;
    bool pm_filter_monitoring_test_complete;
    bool exhaust_gas_sensor_test_complete;
    bool boost_pressure_test_complete;
    bool nox_scr_sensor_test_complete;
    bool nmhc_catalyst_test_complete;

    if (is_spark_engine) {
        vvt_test_complete = (byte_D & (1 << 7)) == 0;
        oxygen_sensor_heater_test_complete = (byte_D & (1 << 6)) == 0;
        oxygen_sensor_test_complete = (byte_D & (1 << 5)) == 0;
        gasoline_particulate_filter_test_complete = (byte_D & (1 << 4)) == 0;
        secondary_air_system_test_complete = (byte_D & (1 << 3)) == 0;
        evaporative_system_test_complete = (byte_D & (1 << 2)) == 0;
        heated_catalyst_test_complete = (byte_D & (1 << 1)) == 0;
        catalyst_test_complete = (byte_D & (1 << 0)) == 0;

    } else {
        egr_system_test_complete = (byte_D & (1 << 7)) == 0;
        pm_filter_monitoring_test_complete = (byte_D & (1 << 6)) == 0;
        exhaust_gas_sensor_test_complete = (byte_D & (1 << 5)) == 0;
        boost_pressure_test_complete = (byte_D & (1 << 4)) == 0;
        nox_scr_sensor_test_complete = (byte_D & (1 << 3)) == 0;
        nmhc_catalyst_test_complete = (byte_D & (1 << 2)) == 0;
    }

    obd2_dtc_status_t status = {
        .check_engine_on = check_engine,
        .dtc_count = dtc_cnt,

        .is_spark_engine = is_spark_engine,

        .misfire_test_available = misfire_available_dtc,
        .fuel_system_test_available = fuel_system_available_dtc,
        .components_test_available = components_available_dtc,
        
        .misfire_test_complete = misfire_complete_dtc,
        .fuel_system_test_complete = fuel_system_complete_dtc,
        .components_test_complete = components_complete_dtc,

    };

    if (is_spark_engine) {
        status.vvt_test_available = vvt_test_available;
        status.vvt_test_complete = vvt_test_complete;

        status.oxygen_sensor_heater_test_available = oxygen_sensor_heater_test_available;
        status.oxygen_sensor_heater_test_complete = oxygen_sensor_heater_test_complete;

        status.oxygen_sensor_test_available = oxygen_sensor_test_available;
        status.oxygen_sensor_test_complete = oxygen_sensor_test_complete;

        status.gasoline_particulate_filter_test_available = gasoline_particulate_filter_test_available;
        status.gasoline_particulate_filter_test_complete = gasoline_particulate_filter_test_complete;

        status.secondary_air_system_test_available = secondary_air_system_test_available;
        status.secondary_air_system_test_complete = secondary_air_system_test_complete;

        status.evaporative_system_test_available = evaporative_system_test_available;
        status.evaporative_system_test_complete = evaporative_system_test_complete;

        status.heated_catalyst_test_available = heated_catalyst_test_available;
        status.heated_catalyst_test_complete = heated_catalyst_test_complete;

        status.catalyst_test_available = catalyst_test_available;
        status.catalyst_test_complete = catalyst_test_complete;

    } else {
        status.egr_system_test_available = egr_system_test_available;
        status.egr_system_test_complete = egr_system_test_complete;

        status.pm_filter_monitoring_test_available = pm_filter_monitoring_test_available;
        status.pm_filter_monitoring_test_complete = pm_filter_monitoring_test_complete;

        status.exhaust_gas_sensor_test_available = exhaust_gas_sensor_test_available;
        status.exhaust_gas_sensor_test_complete = exhaust_gas_sensor_test_complete;

        status.boost_pressure_test_available = boost_pressure_test_available;
        status.boost_pressure_test_complete = boost_pressure_test_complete;

        status.nox_scr_sensor_test_available = nox_scr_sensor_test_available;
        status.nox_scr_sensor_test_complete = nox_scr_sensor_test_complete;

        status.nmhc_catalyst_test_available = nmhc_catalyst_test_available;
        status.nmhc_catalyst_test_complete = nmhc_catalyst_test_complete;
    }

    return status;
}

static fuel_system_status_t decode_single_fuel_system(uint8_t byte) {
    switch (byte)
    {
        case 0:
            return MOTOR_OFF;
        case 1:
            return OPEN_LOOP_INSUFFICIENT_TEMP;
        case 2:
            return CLOSED_LOOP_OXYGEN_SENSOR_FEEDBACK;
        case 4:
            return OPEN_LOOP_ENGINE_LOAD_OR_FUEL_CUT_DECEL;
        case 8:
            return OPEN_LOOP_SYSTEM_FAIL;
        case 16:
            return CLOSED_LOOP_ATLEAST_ONE_OXYGEN_SENSOR_FAULT_FEEDBACK_SYSTEM;

        default:
            return UNKNOWN;
    }
}

// ---- low level decoders ----

double decode_bitmask_4byte(uint8_t* data, size_t length) {
    if (length < 4) return 0.0;

    /* Shift and merge the 4 bytes into a single 32-bit integer */
    uint32_t bitmask = ((uint32_t)data[0] << 24) |  /* Shift 1st byte to highest 8 bits */
                       ((uint32_t)data[1] << 16) |  /* Shift 2nd byte to next 8 bits */
                       ((uint32_t)data[2] << 8)  |  /* Shift 3rd byte to next 8 bits */
                       ((uint32_t)data[3]);         /* 4th byte stays in lowest 8 bits */

    /* Cast to double so it fits our generic obd2_pid_decoder_fn signature */
    return (double)bitmask;
}

obd2_dtc_freeze_frame_t decode_freeze_frame(uint8_t* data, size_t length) {
    obd2_dtc_freeze_frame_t frame = {0};
    
    if (length < 2) return frame;


    frame.category = data[0] >> 6;
    frame.category_number = ((uint16_t)data[1] << 6) | (data[0] & 0b00111111);

    return frame;
}

fuel_system_status_state_t decode_fuel_system(uint8_t* data, size_t length) {
    fuel_system_status_state_t state = {
        .sys1 = MOTOR_OFF,
        .has_sys2 = false,
        .sys2 = MOTOR_OFF
    };

    if (length < 2) return state;

    state.sys1 = decode_single_fuel_system(data[0]);
    state.sys2 = decode_single_fuel_system(data[1]);

    state.has_sys2 = data[1] != 0; // if == 0 then probably unsupported

    return state;
}

double decode_percent(uint8_t* data, size_t length) {
    return data[0] / 2.55;
}

double decode_temp_c(uint8_t* data, size_t length) {
    return data[0] - 40;
}

double decode_fuel_trim(uint8_t* data, size_t length) {
    return (data[0] / 1.28) - 100;
}

double decode_pressure_3a(uint8_t* data, size_t length) {
    return 3 * data[0];
}

double decode_raw_1byte(uint8_t* data, size_t length) {
    return data[0];
}

double decode_rpm(uint8_t* data, size_t length) {
    if (length != 2) return 0;

    return (256.0 * data[0] + data[1]) / 4.0;
}

double decode_timing_advance(uint8_t* data, size_t length) {
    return (data[0] / 2) - 64;
}

double decode_maf(uint8_t* data, size_t length) {
    if (length != 2) return 0;

    return (256.0 * data[0] + data[1]) / 100.0;
}

double decode_o2_voltage(uint8_t* data, size_t length) {
    return data[0] / 200.0;
} 

double decode_raw_2byte(uint8_t* data, size_t length) {
    if (length != 2) return 0;
    return 256.0 * data[0] + data[1];
}

double decode_fuel_rail_rel(uint8_t* data, size_t length) {
    if (length != 2) return 0;
    return 0.079 * (256.0 * data[0] + data[1]);
}

double decode_fuel_rail_abs(uint8_t* data, size_t length) {
    if (length != 2) return 0;
    return 10.0 * (256.0 * data[0] + data[1]);
}

// higher 4 bytes is air fuel ratio, lower 4 bytes is voltage
double decode_o2_ratio(uint8_t* data, size_t length) {
    if (length != 4) return 0;
    float afr =     (float) (2.0/65536.0) * (256.0 * data[0] + data[1]);
    float voltage = (float) (8.0/65536.0) * (256.0 * data[2] + data[3]);

    uint32_t afr_bits, volt_bits;
    memcpy(&afr_bits, &afr, sizeof(float));
    memcpy(&volt_bits, &voltage, sizeof(float));

    uint64_t combined = ((uint64_t) afr_bits << 32) | volt_bits;

    double result;
    memcpy(&result, &combined, sizeof(double));

    return result;
}

double decode_evap_pressure(uint8_t* data, size_t length) {
    if (length != 2) return 0;
    return (256.0 * data[0] + data[1]) / 4.0;
}

// higher 4 bytes is air fuel equivalence ratio, lower 4 bytes is current
double decode_o2_current(uint8_t* data, size_t length) {
    if (length != 4) return 0;

    float afr =     (float) (2.0/65536.0) * (256.0 * data[0] + data[1]);
    float current = (float) ((256.0 * data[2] + data[3]) / 256.0) - 128.0;

    uint32_t afr_bits, current_bits;
    memcpy(&afr_bits, &afr, sizeof(float));
    memcpy(&current_bits, &current, sizeof(float));

    uint64_t combined = ((uint64_t) afr_bits << 32) | current_bits;

    double result;
    memcpy(&result, &combined, sizeof(double));

    return result;
}

 double decode_catalyst_temp(uint8_t* data, size_t length) {
    if (length != 2) return 0;

    return ( (256.0 * data[0] + data[1]) / 10.0 ) - 40.0;
 }

 double decode_module_voltage(uint8_t* data, size_t length) {
    if (length != 2) return 0;

    return (256.0 * data[0] + data[1]) / 100.0;
 }

double decode_abs_load(uint8_t* data, size_t length) {
    if (length != 2) return 0;

    return ( 100.0 / 255.0 ) * (256.0 * data[0] + data[1]);
}

// commanded air-fuel equivalence ratio
double decode_lambda(uint8_t* data, size_t length) {
    if (length != 2) return 0;

    return ( 2.0 / 65536.0 ) * (256.0 * data[0] + data[1]);
}

static const obd2_pid_definition_t STANDART_PIDS[] = {
    {0x00, "PIDs supported [01 - 20]", "Bitmask", decode_bitmask_4byte, 4, 0, 4294967295, ""},
    {0x01, "Monitor status since DTCs cleared", "Bitmask", decode_bitmask_4byte, 4, 0, 4294967295, ""},
    {0x02, "Freeze frame DTC", "Raw", decode_freeze_frame, 2, 0, 65535, "DTC that caused freeze frame"},
    {0x03, "Fuel system status", "Bitmask", decode_fuel_system, 2, 0, 65535, ""},
    {0x04, "Calculated engine load", "%", decode_percent, 1, 0, 100, ""},
    {0x05, "Engine coolant temperature", "°C", decode_temp_c, 1, -40, 215, ""},
    {0x06, "Short term fuel trim (Bank 1)", "%", decode_fuel_trim, 1, -100, 99.2, ""},
    {0x07, "Long term fuel trim (Bank 1)", "%", decode_fuel_trim, 1, -100, 99.2, ""},
    {0x08, "Short term fuel trim (Bank 2)", "%", decode_fuel_trim, 1, -100, 99.2, ""},
    {0x09, "Long term fuel trim (Bank 2)", "%", decode_fuel_trim, 1, -100, 99.2, ""},
    {0x0A, "Fuel pressure (gauge)", "kPa", decode_pressure_3a, 1, 0, 765, ""},
    {0x0B, "Intake manifold absolute pressure", "kPa", decode_raw_1byte, 1, 0, 255, ""},
    {0x0C, "Engine speed", "rpm", decode_rpm, 2, 0, 16383.75, ""},
    {0x0D, "Vehicle speed", "km/h", decode_raw_1byte, 1, 0, 255, ""},
    {0x0E, "Timing advance", "°", decode_timing_advance, 1, -64, 63.5, "Before TDC"},
    {0x0F, "Intake air temperature", "°C", decode_temp_c, 1, -40, 215, ""},
    {0x10, "MAF air flow rate", "g/s", decode_maf, 2, 0, 655.35, ""},
    {0x11, "Throttle position", "%", decode_percent, 1, 0, 100, ""},
    {0x12, "Commanded secondary air status", "Bitmask", decode_raw_1byte, 1, 0, 255, ""},
    {0x13, "Oxygen sensors present (2 banks)", "Bitmask", decode_raw_1byte, 1, 0, 255, ""},
    {0x14, "O2 Sensor 1 Voltage", "V", decode_o2_voltage, 2, 0, 1.275, ""},
    {0x15, "O2 Sensor 2 Voltage", "V", decode_o2_voltage, 2, 0, 1.275, ""},
    {0x16, "O2 Sensor 3 Voltage", "V", decode_o2_voltage, 2, 0, 1.275, ""},
    {0x17, "O2 Sensor 4 Voltage", "V", decode_o2_voltage, 2, 0, 1.275, ""},
    {0x18, "O2 Sensor 5 Voltage", "V", decode_o2_voltage, 2, 0, 1.275, ""},
    {0x19, "O2 Sensor 6 Voltage", "V", decode_o2_voltage, 2, 0, 1.275, ""},
    {0x1A, "O2 Sensor 7 Voltage", "V", decode_o2_voltage, 2, 0, 1.275, ""},
    {0x1B, "O2 Sensor 8 Voltage", "V", decode_o2_voltage, 2, 0, 1.275, ""},
    {0x1C, "OBD standards conforming", "Enum", decode_raw_1byte, 1, 1, 250, ""},
    {0x1D, "Oxygen sensors present (4 banks)", "Bitmask", decode_raw_1byte, 1, 0, 255, ""},
    {0x1E, "Auxiliary input status", "Bitmask", decode_raw_1byte, 1, 0, 255, "Power Take Off"},
    {0x1F, "Run time since engine start", "s", decode_raw_2byte, 2, 0, 65535, ""},
    {0x20, "PIDs supported [21 - 40]", "Bitmask", decode_bitmask_4byte, 4, 0, 4294967295, ""},
    {0x21, "Distance traveled with MIL on", "km", decode_raw_2byte, 2, 0, 65535, ""},
    {0x22, "Fuel Rail Pressure (relative)", "kPa", decode_fuel_rail_rel, 2, 0, 5177.265, ""},
    {0x23, "Fuel Rail Gauge Pressure", "kPa", decode_fuel_rail_abs, 2, 0, 655350, "Diesel or GDI"},
    {0x24, "O2 Sensor 1 (Wideband) Ratio", "Ratio/V", decode_o2_ratio, 4, 0, 2.0, ""},
    {0x25, "O2 Sensor 2 (Wideband) Ratio", "Ratio/V", decode_o2_ratio, 4, 0, 2.0, ""},
    {0x26, "O2 Sensor 3 (Wideband) Ratio", "Ratio/V", decode_o2_ratio, 4, 0, 2.0, ""},
    {0x27, "O2 Sensor 4 (Wideband) Ratio", "Ratio/V", decode_o2_ratio, 4, 0, 2.0, ""},
    {0x28, "O2 Sensor 5 (Wideband) Ratio", "Ratio/V", decode_o2_ratio, 4, 0, 2.0, ""},
    {0x29, "O2 Sensor 6 (Wideband) Ratio", "Ratio/V", decode_o2_ratio, 4, 0, 2.0, ""},
    {0x2A, "O2 Sensor 7 (Wideband) Ratio", "Ratio/V", decode_o2_ratio, 4, 0, 2.0, ""},
    {0x2B, "O2 Sensor 8 (Wideband) Ratio", "Ratio/V", decode_o2_ratio, 4, 0, 2.0, ""},
    {0x2C, "Commanded EGR", "%", decode_percent, 1, 0, 100, ""},
    {0x2D, "EGR Error", "%", decode_fuel_trim, 1, -100, 99.2, ""},
    {0x2E, "Commanded evaporative purge", "%", decode_percent, 1, 0, 100, ""},
    {0x2F, "Fuel Tank Level Input", "%", decode_percent, 1, 0, 100, ""},
    {0x30, "Warm-ups since codes cleared", "Count", decode_raw_1byte, 1, 0, 255, ""},
    {0x31, "Distance since codes cleared", "km", decode_raw_2byte, 2, 0, 65535, ""},
    {0x32, "Evap. System Vapor Pressure", "Pa", decode_evap_pressure, 2, -8192, 8191.75, ""},
    {0x33, "Absolute Barometric Pressure", "kPa", decode_raw_1byte, 1, 0, 255, ""},
    /* --- Wideband Oxygen Sensors (Current) --- */
    {0x34, "O2 Sensor 1 (Wideband) Current", "mA", decode_o2_current, 4, -128, 128, ""},
    {0x35, "O2 Sensor 2 (Wideband) Current", "mA", decode_o2_current, 4, -128, 128, ""},
    {0x36, "O2 Sensor 3 (Wideband) Current", "mA", decode_o2_current, 4, -128, 128, ""},
    {0x37, "O2 Sensor 4 (Wideband) Current", "mA", decode_o2_current, 4, -128, 128, ""},
    {0x38, "O2 Sensor 5 (Wideband) Current", "mA", decode_o2_current, 4, -128, 128, ""},
    {0x39, "O2 Sensor 6 (Wideband) Current", "mA", decode_o2_current, 4, -128, 128, ""},
    {0x3A, "O2 Sensor 7 (Wideband) Current", "mA", decode_o2_current, 4, -128, 128, ""},
    {0x3B, "O2 Sensor 8 (Wideband) Current", "mA", decode_o2_current, 4, -128, 128, ""},

    /* --- Catalyst Temperatures --- */
    {0x3C, "Catalyst Temperature: Bank 1, Sensor 1", "°C", decode_catalyst_temp, 2, -40, 6513.5, ""},
    {0x3D, "Catalyst Temperature: Bank 2, Sensor 1", "°C", decode_catalyst_temp, 2, -40, 6513.5, ""},
    {0x3E, "Catalyst Temperature: Bank 1, Sensor 2", "°C", decode_catalyst_temp, 2, -40, 6513.5, ""},
    {0x3F, "Catalyst Temperature: Bank 2, Sensor 2", "°C", decode_catalyst_temp, 2, -40, 6513.5, ""},

    /* --- Support Block 3 --- */
    {0x40, "PIDs supported [41 - 60]", "Bitmask", decode_bitmask_4byte, 4, 0, 4294967295, ""},
    
    /* --- Extended Engine & Emissions Data --- */
    {0x41, "Monitor status this drive cycle", "Bitmask", decode_bitmask_4byte, 4, 0, 4294967295, ""},
    {0x42, "Control module voltage", "V", decode_module_voltage, 2, 0, 65.535, ""},
    {0x43, "Absolute load value", "%", decode_abs_load, 2, 0, 25700, ""},
    {0x44, "Commanded Air-Fuel Equivalence Ratio", "Ratio", decode_lambda, 2, 0, 2.0, ""},
    {0x45, "Relative throttle position", "%", decode_percent, 1, 0, 100, ""},
    {0x46, "Ambient air temperature", "°C", decode_temp_c, 1, -40, 215, ""},
    {0x47, "Absolute throttle position B", "%", decode_percent, 1, 0, 100, ""},
    {0x48, "Absolute throttle position C", "%", decode_percent, 1, 0, 100, ""},
    {0x49, "Accelerator pedal position D", "%", decode_percent, 1, 0, 100, ""},
    {0x4A, "Accelerator pedal position E", "%", decode_percent, 1, 0, 100, ""},
    {0x4B, "Accelerator pedal position F", "%", decode_percent, 1, 0, 100, ""},
    {0x4C, "Commanded throttle actuator", "%", decode_percent, 1, 0, 100, ""},
    {0x4D, "Time run with MIL on", "min", decode_raw_2byte, 2, 0, 65535, ""},
    {0x4E, "Time since trouble codes cleared", "min", decode_raw_2byte, 2, 0, 65535, ""},
    {0x4F, "Maximum value for Equivalence Ratio, O2 Voltage/Current, Intake MAP", "Raw", decode_multiplexed, 4, 0, 0, "Multiplexed"},
    {0x50, "Maximum value for MAF air flow rate", "Raw", decode_multiplexed, 4, 0, 0, "Multiplexed"},
    {0x51, "Fuel Type", "Enum", decode_raw_1byte, 1, 0, 255, ""},
    {0x52, "Ethanol fuel %", "%", decode_percent, 1, 0, 100, ""},
    {0x53, "Absolute Evap system Vapor Pressure", "kPa", decode_evap_vp_abs, 2, 0, 327.675, ""},
    {0x54, "Evap system vapor pressure", "Pa", decode_raw_2byte, 2, -32767, 32768, "Uses A*256+B-32767"},
    {0x55, "Short term secondary O2 trim (Bank 1 & 3)", "Raw", decode_multiplexed, 2, 0, 0, "Multiplexed"},
    {0x56, "Long term secondary O2 trim (Bank 1 & 3)", "Raw", decode_multiplexed, 2, 0, 0, "Multiplexed"},
    {0x57, "Short term secondary O2 trim (Bank 2 & 4)", "Raw", decode_multiplexed, 2, 0, 0, "Multiplexed"},
    {0x58, "Long term secondary O2 trim (Bank 2 & 4)", "Raw", decode_multiplexed, 2, 0, 0, "Multiplexed"},
    {0x59, "Fuel rail absolute pressure", "kPa", decode_raw_2byte, 2, 0, 655350, "Uses (A*256+B)*10"},
    {0x5A, "Relative accelerator pedal position", "%", decode_percent, 1, 0, 100, ""},
    {0x5B, "Hybrid battery pack remaining life", "%", decode_percent, 1, 0, 100, ""},
    {0x5C, "Engine oil temperature", "°C", decode_temp_c, 1, -40, 215, ""},
    {0x5D, "Fuel injection timing", "°", decode_fuel_inj_timing, 2, -210, 301.99, ""},
    {0x5E, "Engine fuel rate", "L/h", decode_engine_fuel_rate, 2, 0, 3276.75, ""},
    {0x5F, "Emission requirements to which vehicle is designed", "Enum", decode_raw_1byte, 1, 0, 255, ""},

    /* --- Support Block 4 --- */
    {0x60, "PIDs supported [61 - 80]", "Bitmask", decode_bitmask_4byte, 4, 0, 4294967295, ""},
    
    /* --- Torque and Heavy/Diesel Engine Data (Highly Multiplexed) --- */
    {0x61, "Driver's demand engine - percent torque", "%", decode_torque, 1, -125, 125, ""},
    {0x62, "Actual engine - percent torque", "%", decode_torque, 1, -125, 125, ""},
    {0x63, "Engine reference torque", "Nm", decode_raw_2byte, 2, 0, 65535, ""},
    {0x64, "Engine percent torque data", "Raw", decode_multiplexed, 5, 0, 0, "Multiplexed"},
    {0x65, "Auxiliary input / output supported", "Bitmask", decode_raw_2byte, 2, 0, 65535, ""},
    {0x66, "Mass air flow sensor", "Raw", decode_multiplexed, 5, 0, 0, "Multiplexed"},
    {0x67, "Engine coolant temperature", "Raw", decode_multiplexed, 3, 0, 0, "Multiplexed"},
    {0x68, "Intake air temperature sensor", "Raw", decode_multiplexed, 7, 0, 0, "Multiplexed"},
    {0x69, "Commanded EGR and EGR Error", "Raw", decode_multiplexed, 7, 0, 0, "Multiplexed"},
    {0x6A, "Commanded Diesel intake air flow control", "Raw", decode_multiplexed, 5, 0, 0, "Multiplexed"},
    {0x6B, "Exhaust gas recirculation temperature", "Raw", decode_multiplexed, 5, 0, 0, "Multiplexed"},
    {0x6C, "Commanded throttle actuator control", "Raw", decode_multiplexed, 5, 0, 0, "Multiplexed"},
    {0x6D, "Fuel pressure control system", "Raw", decode_multiplexed, 6, 0, 0, "Multiplexed"},
    {0x6E, "Injection pressure control system", "Raw", decode_multiplexed, 6, 0, 0, "Multiplexed"},
    {0x6F, "Turbocharger compressor inlet pressure", "Raw", decode_multiplexed, 3, 0, 0, "Multiplexed"},
    {0x70, "Boost pressure control", "Raw", decode_multiplexed, 9, 0, 0, "Multiplexed"},
    {0x71, "Variable Geometry turbo (VGT) control", "Raw", decode_multiplexed, 5, 0, 0, "Multiplexed"},
    {0x72, "Wastegate control", "Raw", decode_multiplexed, 5, 0, 0, "Multiplexed"},
    {0x73, "Exhaust pressure", "Raw", decode_multiplexed, 5, 0, 0, "Multiplexed"},
    {0x74, "Turbocharger RPM", "Raw", decode_multiplexed, 5, 0, 0, "Multiplexed"},
    {0x75, "Turbocharger temperature 1", "Raw", decode_multiplexed, 7, 0, 0, "Multiplexed"},
    {0x76, "Turbocharger temperature 2", "Raw", decode_multiplexed, 7, 0, 0, "Multiplexed"},
    {0x77, "Charge air cooler temperature (CACT)", "Raw", decode_multiplexed, 5, 0, 0, "Multiplexed"},
    {0x78, "Exhaust Gas temperature (EGT) Bank 1", "Raw", decode_multiplexed, 9, 0, 0, "Multiplexed"},
    {0x79, "Exhaust Gas temperature (EGT) Bank 2", "Raw", decode_multiplexed, 9, 0, 0, "Multiplexed"},
    {0x7A, "Diesel particulate filter (DPF) 1", "Raw", decode_multiplexed, 7, 0, 0, "Multiplexed"},
    {0x7B, "Diesel particulate filter (DPF) 2", "Raw", decode_multiplexed, 7, 0, 0, "Multiplexed"},
    {0x7C, "Diesel Particulate filter (DPF) temperature", "Raw", decode_multiplexed, 9, 0, 0, "Multiplexed"},
    {0x7D, "NOx NTE control area status", "Bitmask", decode_raw_1byte, 1, 0, 255, ""},
    {0x7E, "PM NTE control area status", "Bitmask", decode_raw_1byte, 1, 0, 255, ""},
    {0x7F, "Engine run time", "Raw", decode_multiplexed, 13, 0, 0, "Multiplexed"},

    /* --- Support Block 5 --- */
    {0x80, "PIDs supported [81 - A0]", "Bitmask", decode_bitmask_4byte, 4, 0, 4294967295, ""},
    
    /* --- Advanced Emissions & AECD Diagnostics --- */
    {0x81, "Engine run time for AECD #1-#5", "Raw", decode_multiplexed, 21, 0, 0, "Multiplexed"},
    {0x82, "Engine run time for AECD #6-#10", "Raw", decode_multiplexed, 21, 0, 0, "Multiplexed"},
    {0x83, "NOx sensor", "Raw", decode_multiplexed, 5, 0, 0, "Multiplexed"},
    {0x84, "Manifold surface temperature", "°C", decode_temp_c, 1, -40, 215, ""},
    {0x85, "NOx reagent system", "Raw", decode_multiplexed, 10, 0, 0, "Multiplexed"},
    {0x86, "Particulate matter (PM) sensor", "Raw", decode_multiplexed, 5, 0, 0, "Multiplexed"},
    {0x87, "Intake manifold absolute pressure", "Raw", decode_multiplexed, 5, 0, 0, "Multiplexed"},
    {0x88, "SCR Induce System", "Raw", decode_multiplexed, 13, 0, 0, "Multiplexed"},
    {0x89, "Run Time for AECD #11-#15", "Raw", decode_multiplexed, 21, 0, 0, "Multiplexed"},
    {0x8A, "Run Time for AECD #16-#20", "Raw", decode_multiplexed, 21, 0, 0, "Multiplexed"},
    {0x8B, "Diesel Aftertreatment", "Raw", decode_multiplexed, 7, 0, 0, "Multiplexed"},
    {0x8C, "O2 Sensor (Wideband)", "Raw", decode_multiplexed, 16, 0, 0, "Multiplexed"},
    {0x8D, "Throttle Position G", "%", decode_percent, 1, 0, 100, ""},
    {0x8E, "Engine Friction - Percent Torque", "%", decode_torque, 1, -125, 125, ""},
    {0x8F, "PM Sensor Bank 1 & 2", "Raw", decode_multiplexed, 7, 0, 0, "Multiplexed"},
    {0x90, "WWH-OBD Vehicle OBD System Information 1", "Raw", decode_multiplexed, 3, 0, 0, "Multiplexed"},
    {0x91, "WWH-OBD Vehicle OBD System Information 2", "Raw", decode_multiplexed, 5, 0, 0, "Multiplexed"},
    {0x92, "Fuel System Control", "Raw", decode_multiplexed, 2, 0, 0, "Multiplexed"},
    {0x93, "WWH-OBD Vehicle OBD Counters support", "Raw", decode_multiplexed, 3, 0, 0, "Multiplexed"},
    {0x94, "NOx Warning And Deforcement System", "Raw", decode_multiplexed, 12, 0, 0, "Multiplexed"},
    /* 0x95, 0x96, 0x97 are intentionally reserved/undefined by standard */
    {0x98, "Exhaust Gas Temperature Sensor 1", "Raw", decode_multiplexed, 9, 0, 0, "Multiplexed"},
    {0x99, "Exhaust Gas Temperature Sensor 2", "Raw", decode_multiplexed, 9, 0, 0, "Multiplexed"},
    {0x9A, "Hybrid/EV Vehicle System Data, Battery, Voltage", "Raw", decode_multiplexed, 6, 0, 0, "Multiplexed"},
    {0x9B, "Diesel Exhaust Fluid Sensor Data", "Raw", decode_multiplexed, 4, 0, 0, "Multiplexed"},
    {0x9C, "O2 Sensor Data", "Raw", decode_multiplexed, 17, 0, 0, "Multiplexed"},
    {0x9D, "Engine Fuel Rate", "Raw", decode_multiplexed, 4, 0, 0, "Multiplexed"},
    {0x9E, "Engine Exhaust Flow Rate", "Raw", decode_multiplexed, 2, 0, 0, "Multiplexed"},
    {0x9F, "Fuel System Percentage Use", "Raw", decode_multiplexed, 9, 0, 0, "Multiplexed"},

    /* --- Support Block 6 --- */
    {0xA0, "PIDs supported [A1 - C0]", "Bitmask", decode_bitmask_4byte, 4, 0, 4294967295, ""},
    
    /* --- Final Standard Emissions Sensors --- */
    {0xA1, "NOx Sensor Corrected Data", "Raw", decode_multiplexed, 9, 0, 0, "Multiplexed"},
    {0xA2, "Cylinder Fuel Rate", "Raw", decode_multiplexed, 2, 0, 0, "Multiplexed"},
    {0xA3, "Evap System Vapor Pressure", "Raw", decode_multiplexed, 9, 0, 0, "Multiplexed"},
    {0xA4, "Transmission Actual Gear", "Raw", decode_multiplexed, 4, 0, 0, "Multiplexed"},
    {0xA5, "Commanded Diesel Exhaust Fluid Dosing", "Raw", decode_multiplexed, 4, 0, 0, "Multiplexed"},
    {0xA6, "Odometer", "Raw", decode_multiplexed, 4, 0, 0, "Multiplexed"},
    {0xA7, "NOx Sensor Concentration Bank 3", "Raw", decode_multiplexed, 4, 0, 0, "Multiplexed"},
    {0xA8, "NOx Sensor Concentration Bank 4", "Raw", decode_multiplexed, 4, 0, 0, "Multiplexed"},
    {0xA9, "ABS Disable Switch State", "Raw", decode_multiplexed, 4, 0, 0, "Multiplexed"},
    {0xAA, "Emissions related DTC", "Raw", decode_multiplexed, 5, 0, 0, "Multiplexed"},
    
    /* 0xAB to 0xBF are intentionally reserved/undefined by standard */

    /* --- Support Block 7 --- */
    {0xC0, "PIDs supported [C1 - E0]", "Bitmask", decode_bitmask_4byte, 4, 0, 4294967295, ""},
    
    /* --- Drive Motors & Specialized C-Block --- */
    {0xC3, "Drive Motor A Position", "Enum", decode_raw_2byte, 2, 0, 65535, ""},
    {0xC4, "Drive Motor B Position", "Enum", decode_raw_2byte, 2, 0, 65535, ""},
    {0xC8, "NOx Sensor Calibration", "Enum", decode_raw_2byte, 2, 0, 65535, ""}
};