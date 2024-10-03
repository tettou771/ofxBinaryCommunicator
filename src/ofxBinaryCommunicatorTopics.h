#pragma once
#include "ofxBinaryCommunicatorTopicStructMaker.h"

TOPIC_STRUCT_MAKER(MultiData, 253,
    union data {
        int16_t int16[64];
        int32_t int32[32];
        
    };
)

TOPIC_STRUCT_MAKER(DeviceInfoData, 254,
    char deviceName[32];
    char version[32];
    uint16_t deviceId;
)

TOPIC_STRUCT_MAKER(DeviceInfoRequest, 255,)
