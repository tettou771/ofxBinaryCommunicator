#pragma once

////////////////////////////////////////////////////////////////////////////////
// Packing Macros for Defining Packed Structures with Unique `topicId`
// Supports MSVC, GCC, and Clang compilers
////////////////////////////////////////////////////////////////////////////////

// Check the compiler and define packing macros accordingly
#if defined(_MSC_VER)
    // Microsoft Visual C++ Compiler
    #define TOPIC_STRUCT_BEGIN(structName, topicIdValue) \
        __pragma(pack(push, 1)) \
        struct structName { \
            static const uint8_t topicId = topicIdValue;

    #define TOPIC_STRUCT_END \
        }; \
        __pragma(pack(pop))
#elif defined(__GNUC__) || defined(__clang__)
    // GCC and Clang Compilers
    #define TOPIC_STRUCT_BEGIN(structName, topicIdValue) \
        _Pragma("pack(push, 1)") \
        struct structName { \
            static const uint8_t topicId = topicIdValue;

    #define TOPIC_STRUCT_END \
        }; \
        _Pragma("pack(pop)")
#else
    #error "Unsupported compiler for TOPIC_STRUCT_MAKER macros."
#endif

////////////////////////////////////////////////////////////////////////////////
// TOPIC_STRUCT_MAKER Macro
//
// Usage:
// TOPIC_STRUCT_MAKER(StructName, topicIdValue,
//     // Struct members here
// )
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Macro to create a packed struct with a unique topicId.
 *
 * This macro simplifies the definition of packed structs by handling the
 * necessary packing pragmas and including a static `topicId` member.
 *
 * @param structName      The name of the struct to be defined.
 * @param topicIdValue    The unique topicId value (0-240) for the struct.
 * @param ...             The struct member declarations.
 *
 * @note
 * - `topicIdValue` must be unique across all packet structs.
 * - Valid `topicIdValue` range is 0 to 240. Values 241 and above are reserved.
 */
#define TOPIC_STRUCT_MAKER(structName, topicIdValue, ...) \
    TOPIC_STRUCT_BEGIN(structName, topicIdValue) \
        __VA_ARGS__ \
    TOPIC_STRUCT_END

/*
 * This header provides macros to define packed structures (structs) for serial communication.
 * It ensures consistent memory layout across different compilers by controlling structure packing
 * and alignment. Additionally, it includes a unique `topicId` for each packet structure to identify
 * the type of data being transmitted.
 *
 * **Usage Example:**
 *
 * ```cpp
 * #include "ofxBinariCommunicator.h"
 *
 * // Define a packed struct with a unique topicId of 2
 * TOPIC_STRUCT_MAKER(DataPacket, 2,
 *     int32_t index;
 *     float value[4];
 *     char msg[20];
 * )
 *
 * ofxBinaryCommunicator communicator;
 *
 * void setup() {
 *     // in openFrameworks
 *     communicator.setup(<port>, <baudRate>);
 *     // in Arduino IDE
 *     communicator.setup(<Serial instance>);
 * }
 *
 * void sendData() {
 *     DataPacket data;
 *     data.index = 12345;
 *     data.value[0] = 1.1f;
 *     data.value[1] = 2.2f;
 *     data.value[2] = 3.3f;
 *     data.value[3] = 4.4f;
 *     strncpy(data.msg, "Hello, Arduino!", sizeof(data.msg));
 *
 *     // Send with communicator
 *     communicator.send(data);
 * }
 * ```
 *
 * **Important Notes:**
 * - **Unique `topicId`:** Each packet structure must have a unique `topicId` to prevent duplication.
 * - **`topicId` Range:** Valid `topicId` values range from **0 to 240**. Values **241 and above are reserved**.
 * - **Compiler Compatibility:** The macros support Microsoft Visual C++, GCC, and Clang compilers.
 */
