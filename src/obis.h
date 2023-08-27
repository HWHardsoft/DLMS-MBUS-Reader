#include <inttypes.h>

// Scaling
#define SCALE_TENTHS                0xFF
#define SCALE_HUNDREDTHS            0xFE
#define SCALE_THOUSANDS             0xFD

// Data types as per specification
#define DATA_NULL                   0x00
#define DATA_OCTET_STRING           0x09
#define DATA_LONG_UNSIGNED          0x12
#define DATA_LONG_DOUBLE_UNSIGNED   0x06

#define TYPE_UNKNOWN                0
#define TYPE_VOLTAGE_L1             1
#define TYPE_VOLTAGE_L2             2
#define TYPE_VOLTAGE_L3             3
#define TYPE_CURRENT_L1             4
#define TYPE_CURRENT_L2             5
#define TYPE_CURRENT_L3             6
#define TYPE_POWER_FACTOR           7
#define TYPE_ACTIVE_POWER_PLUS      8
#define TYPE_ACTIVE_POWER_MINUS     9
#define TYPE_ACTIVE_ENERGY_PLUS     10
#define TYPE_ACTIVE_ENERGY_MINUS    11

// Data structure
#define DECODER_START_OFFSET 20 // Offset for start of OBIS decoding
#define OBIS_TYPE_OFFSET   0
#define OBIS_LENGTH_OFFSET 1
#define OBIS_CODE_OFFSET   2

// A.B. C.D.E.  F
// 1.0.13.7.0.255
#define OBIS_A 0
#define OBIS_B 1
#define OBIS_C 2
#define OBIS_D 3
#define OBIS_E 4
#define OBIS_F 5

// cos(phi)
static const uint8_t OBIS_POWER_FACTOR[] {
    0x0D, 0x07 // 1.0.13.7.0.255
};
// Spannung
static const uint8_t OBIS_VOLTAGE_L1[] {
    0x20, 0x07 // 1.0.32.7.0.255
};
static const uint8_t OBIS_VOLTAGE_L2[] {
    0x34, 0x07 // 1.0.52.7.0.255
};
static const uint8_t OBIS_VOLTAGE_L3[] {
    0x48, 0x07 // 1.0.72.7.0.255
};
// Strom
static const uint8_t OBIS_CURRENT_L1[] {
    0x1F, 0x07 // 1.0.31.7.0.255
};
static const uint8_t OBIS_CURRENT_L2[] {
    0x33, 0x07 // 1.0.51.7.0.255
};
static const uint8_t OBIS_CURRENT_L3[] {
    0x47, 0x07 // 1.0.71.7.0.255
};
// Momentanleistung
static const uint8_t OBIS_ACTIVE_POWER_PLUS[] {
    0x01, 0x07 // 1.0.1.7.0.255
};
static const uint8_t OBIS_ACTIVE_POWER_MINUS[] {
    0x02, 0x07 // 1.0.2.7.0.255
};
// Wirkenergie
static const uint8_t OBIS_ACTIVE_ENERGY_PLUS[] {
    0x01, 0x08 // 1.0.1.8.0.255
};
static const uint8_t OBIS_ACTIVE_ENERGY_MINUS[] {
    0x02, 0x08 // 1.0.2.8.0.255
};
