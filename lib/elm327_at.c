#include "../include/ds_array.h"
#include "../include/com.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

char* AT_reset_elm() {
    sendCommand("AT Z");

    result = readConnection(); // reserved for possible future readline function and or correcting the output
    return result;
}

char* AT_set_echo()