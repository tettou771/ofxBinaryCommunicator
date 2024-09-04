# ofxBinaryCommunicator

ofxBinaryCommunicator is a library for easily sending and receiving custom-defined struct data between Arduino and openFrameworks. It uses serial communication to efficiently exchange binary data.

## Features

- Send and receive custom-defined structs directly
- Data integrity verification using checksums
- Error handling capabilities
- Compatible with both Arduino and openFrameworks

## Installation

### openFrameworks

1. Clone or download this repository and place it in the `openFrameworks/addons/` folder.
2. Use the openFrameworks project generator to add ofxBinaryCommunicator to your project.

### Arduino

1. Download this repository as a ZIP file.
2. In the Arduino IDE, go to "Sketch" → "Include Library" → "Add .ZIP Library..." and select the downloaded ZIP file.

## Usage

1. Define the struct for the data you want to send and receive:

```cpp
struct SampleData {
    int32_t timestamp;
    int x;
    int y;
    char message[20];
};
```

2. Create an instance of ofxBinaryCommunicator:

```cpp
ofxBinaryCommunicator<SampleData> communicator;
```

3. Set up the communicator:

```cpp
// openFrameworks
communicator.setup("/dev/ttyUSB0", 115200);

// Arduino
communicator.setup(Serial, 115200);
```

4. Send data:

```cpp
SampleData data;
data.timestamp = ofGetElapsedTimeMillis(); // or millis()
data.x = 100;
data.y = 200;
strncpy(data.message, "Hello", sizeof(data.message));

ofxBinaryPacket<SampleData> packet;
packet.packet = data;
communicator.sendPacket(packet);
```

5. Receive data:

```cpp
communicator.onReceived = [](const ofxBinaryPacket<SampleData>& packet, size_t size) {
    // Process received data
};
```

6. Set up error handling:

```cpp
communicator.onError = [](ofxBinaryCommunicator<SampleData>::ErrorType errorType, const uint8_t* data, size_t length) {
    // Handle errors
};
```

7. Call `update()` regularly to process data:

```cpp
void loop() { // or void update()
    communicator.update();
}
```

## Handling Variable-Length Data

To include variable-length data in your struct, you can implement it as follows:

1. Define a variable-length array at the end of your struct:

```cpp
struct VariableLengthData {
    int32_t timestamp;
    int dataSize;
    uint8_t data[];
};
```

2. When sending, specify the actual data size when calling sendPacket:

```cpp
size_t totalSize = sizeof(VariableLengthData) + actualDataSize;
VariableLengthData* vdata = (VariableLengthData*)malloc(totalSize);
// Set up your data
communicator.sendPacket(reinterpret_cast<ofxBinaryPacket<VariableLengthData>&>(*vdata), totalSize);
free(vdata);
```

3. When receiving, use the received size to process the data appropriately:

```cpp
communicator.onReceived = [](const ofxBinaryPacket<VariableLengthData>& packet, size_t size) {
    size_t dataSize = size - sizeof(VariableLengthData);
    // Process the data
};
```

Note: When using variable-length data, make sure to implement proper error checking to prevent buffer overflows.

## License

This library is released under the MIT License.

## Author

tettou771

## Contributing

Bug reports and feature requests are welcome on GitHub Issues. Pull requests are also welcome.
