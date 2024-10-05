#pragma once
#include "ofxBinaryCommunicatorTopicStructMaker.h"

TOPIC_STRUCT_MAKER(DeviceInfoRequest, 255,)

TOPIC_STRUCT_MAKER(DeviceInfoResponse, 254,
    char deviceName[32];
    char version[32];
    uint16_t deviceId;
)

TOPIC_STRUCT_MAKER(SetDeviceIdRequest, 253,
    uint16_t deviceId;
)

TOPIC_STRUCT_MAKER(SetDeviceIdResponse, 252,
    uint16_t deviceId;
    bool succeeded;
)

TOPIC_STRUCT_MAKER(ErrorResponse, 251,
    uint32_t timestamp;
    char msg[32];
    ofxBinaryCommunicator::ErrorType e;
)
