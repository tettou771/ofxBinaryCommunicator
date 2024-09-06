# ofxBinaryCommunicator

ofxBinaryCommunicator is a library for efficient binary communication between Arduino and openFrameworks. It allows sending and receiving custom-defined struct data over serial communication with data integrity verification.

## Features

- Send and receive custom-defined structs directly
- Data integrity verification using Fletcher's Checksum
- Error handling capabilities
- Compatible with both Arduino and openFrameworks
- Support for multiple data types with topic IDs
- Escape sequence handling for reliable data transmission

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
struct SampleSensorData {
    static const uint8_t topicId = 0;
    int32_t timestamp;
    int sensorValue;
};

struct SampleMouseData {
    static const uint8_t topicId = 1;
    int32_t timestamp;
    int x;
    int y;
};
```

2. Create an instance of ofxBinaryCommunicator:

```cpp
ofxBinaryCommunicator communicator;
```

3. Set up the communicator:

```cpp
// openFrameworks
communicator.setup("/dev/ttyUSB0", 115200);

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

## Example

The repository includes example code for both Arduino (`ofxBinaryCommunicatorExample.ino`) and openFrameworks (`ofApp.cpp`). These examples demonstrate how to set up the communicator, send various types of data, and handle received messages.

## Customization

You can adjust the maximum packet size by defining `MAX_PACKET_SIZE` before including the library:

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
