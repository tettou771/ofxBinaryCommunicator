# ofxBinaryCommunicator

ofxBinaryCommunicator is a library for efficient binary communication between Arduino and openFrameworks. It allows sending and receiving custom-defined struct data over serial communication with data integrity verification.

The purpose is to be able to send and receive packages containing several types, like OSC or MQTT.
Since only predefined structures can be used for communication, flexibility is sacrificed but the overhead is small.
It is also suitable when there is a limit to the number of bytes that can be sent, such as when a microcontroller sends and receives data several hundred times per second.

## Features

- Direct sending and receiving of custom-defined structures
- Compatible with both Arduino and openFrameworks
- Support for multiple data types using topicId
- Verification of data integrity using checksum
- Error handling function

## Installation

### openFrameworks

1. Clone this repository into the `openFrameworks/addons/` folder.
2. Use the openFrameworks project generator to add ofxBinaryCommunicator to your project.

### Arduino

1. Download this repository as a ZIP file.
2. In the Arduino IDE, go to "Sketch" → "Include Library" → "Add .ZIP Library..." and select the downloaded ZIP file.

## Usage

1. Define structs for the data you want to send and receive:

```cpp
// struct definitions
TOPIC_STRUCT_MAKER(SampleSensorData, 0, 
    int32_t timestamp;
    int sensorValue;
)

TOPIC_STRUCT_MAKER(SampleMouseData, 1,
    int32_t timestamp;
    int x;
    int y;
)
```

2. Create an instance of ofxBinaryCommunicator:

```cpp
ofxBinaryCommunicator communicator;
```

3. Set up the communicator:

```cpp
// openFrameworks
communicator.setup("COM3", 115200); // Please set your COM port

// Arduino
communicator.setup(Serial);
```

4. Send data:

```cpp
SampleSensorData data;
data.timestamp = ofGetElapsedTimeMillis(); // or millis()
data.sensorValue = analogRead(A0);
communicator.send(data);
```

5. Receive data:

```cpp
// openFrameworks
ofAddListener(communicator.onReceived, this, &YourClass::onMessageReceived);

void YourClass::onMessageReceived(const ofxBinaryPacket& packet) {
    switch (packet.topicId) {
        case SampleSensorData::topicId: {
            SampleSensorData sensorData;
            if (packet.unpack(sensorData)) {
                // Process received data
            }
            break;
        }
        // Handle other topic IDs...
    }
}

// Arduino
communicator.setReceivedCallback(onMessageReceived);

void onMessageReceived(const ofxBinaryPacket& packet) {
    switch (packet.topicId) {
        case SampleMouseData::topicId: {
            SampleMouseData mouseData;
            if (packet.unpack(mouseData)) {
                // Process received data
            }
            break;
        }
        // Handle other topic IDs...
    }
}
```

6. Set up error handling:

```cpp
// openFrameworks
ofAddListener(communicator.onError, this, &YourClass::onError);

void YourClass::onError(ofxBinaryCommunicator::ErrorType& errorType) {
    // Handle errors
}

// Arduino
communicator.setErrorCallback(onError);

void onError(ofxBinaryCommunicator::ErrorType errorType) {
    // Handle errors
}
```

7. Call `update()` regularly to process data:

```cpp
void update() { // or void loop() for Arduino
    communicator.update();
}
```

## Examples

The repository contains three types of samples. Sample code for Arduino (`ofxBinaryCommunicatorExample-xxx.ino`) and openFrameworks (`example-openFrameworks-xxx`) is included. These samples show how to set up, send various types of data, and handle received messages.

After writing the corresponding source code on the Arduino side and building the source code on the openFrameworks side, communication will be possible.

### basic

This explains the basic usage.

This is a sample of how to define a structure that corresponds to the content you want to communicate, serialize it, and communicate.

### OscLikeMessage

This is a method of sending and receiving data using a predefined structure called OscLikeMessage.

Although it has a large overhead, it allows you to easily send and receive data in a manner similar to OSC.

However, because the structure is fixed length, it cannot handle types such as string. You can include int32, float, Color (custom definition), etc. in the data.

### DeviceInfoRequest

When many devices are connected to a PC, it can be difficult to identify devices using only the COM port number.

Therefore, it is useful to assign names and IDs to devices in advance and make requests, as this will allow you to identify which devices are connected.

This sample is intended for such use.

## Customization

You can adjust the maximum packet size by defining `MAX_PACKET_SIZE` before including the library.

If the structure to be sent or received is large, set the size larger; if you want to reduce memory usage, set the size smaller.

```cpp
#define MAX_PACKET_SIZE 512
#include "ofxBinaryCommunicator.h"
```

## License

This library is released under the MIT License.

## Author

tettou771

## Contributing

Bug reports, feature requests, and pull requests are welcome on GitHub Issues.
