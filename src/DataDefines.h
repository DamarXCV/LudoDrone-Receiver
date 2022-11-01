#pragma once

#include <stdint.h>

enum StatusTypes : uint8_t {
    STOP = 0,
    FLYING = 1,
};

struct TelemetryData {
    uint32_t Time = 0;

    uint8_t Height = 0;
    uint8_t Yaw = 0;
    uint8_t Pitch = 0;
    uint8_t Roll = 0;
};

struct ControlAndConfigData {
    uint64_t Time = 0;

    uint8_t Status = StatusTypes::STOP;
    uint32_t LastControlInputTimeDelta = 0;

    uint8_t Height = 0;
    uint8_t Yaw = 0;
    uint8_t Pitch = 0;
    uint8_t Roll = 0;

    uint16_t RollP = 0;
    uint16_t RollI = 0;
    uint16_t RollD = 0;
    uint16_t PitchP = 0;
    uint16_t PitchI = 0;
    uint16_t PitchD = 0;
    uint16_t YawP = 0;
    uint16_t YawI = 0;
    uint16_t YawD = 0;
    uint16_t HeightP = 0;
    uint16_t HeightI = 0;
    uint16_t HeightD = 0;

    uint16_t RollTrim = 0;
    uint16_t PitchTrim = 0;
    uint16_t YawTrim = 0;
};
