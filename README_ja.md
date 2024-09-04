# ofxBinaryCommunicator

ofxBinaryCommunicatorは、ArduinoとopenFrameworks間でカスタム定義した構造体データを簡単に送受信するためのライブラリです。シリアル通信を使用して、バイナリデータを効率的にやり取りすることができます。

## 特徴

- カスタム定義した構造体をそのまま送受信可能
- チェックサムによるデータ整合性の確認
- エラーハンドリング機能
- ArduinoとopenFrameworksの両方に対応

## インストール方法

### openFrameworks

1. このリポジトリをクローンまたはダウンロードし、`openFrameworks/addons/`フォルダに配置します。
2. openFrameworksのプロジェクトジェネレーターを使用して、プロジェクトにofxBinaryCommunicatorを追加します。

### Arduino

1. このリポジトリをZIPファイルとしてダウンロードします。
2. Arduino IDEで、「スケッチ」→「ライブラリをインクルード」→「.ZIP形式のライブラリをインストール...」を選択し、ダウンロードしたZIPファイルを選択します。

## 使用方法

1. 送受信したいデータの構造体を定義します：

```cpp
struct SampleData {
    int32_t timestamp;
    int x;
    int y;
    char message[20];
};
```

2. ofxBinaryCommunicatorのインスタンスを作成します：

```cpp
ofxBinaryCommunicator<SampleData> communicator;
```

3. セットアップを行います：

```cpp
// openFrameworks
communicator.setup("/dev/ttyUSB0", 115200);

// Arduino
communicator.setup(Serial, 115200);
```

4. データを送信します：

```cpp
SampleData data;
data.timestamp = ofGetElapsedTimeMillis(); // または millis()
data.x = 100;
data.y = 200;
strncpy(data.message, "Hello", sizeof(data.message));

ofxBinaryPacket<SampleData> packet;
packet.packet = data;
communicator.sendPacket(packet);
```

5. データを受信します：

```cpp
communicator.onReceived = [](const ofxBinaryPacket<SampleData>& packet, size_t size) {
    // 受信したデータを処理
};
```

6. エラーハンドリングを設定します：

```cpp
communicator.onError = [](ofxBinaryCommunicator<SampleData>::ErrorType errorType, const uint8_t* data, size_t length) {
    // エラーを処理
};
```

7. 定期的に`update()`を呼び出してデータを処理します：

```cpp
void loop() { // または void update()
    communicator.update();
}
```

## 可変長データの扱い方

構造体に可変長のデータを含める場合は、以下のように実装できます：

1. 構造体の最後に可変長配列を定義します：

```cpp
struct VariableLengthData {
    int32_t timestamp;
    int dataSize;
    uint8_t data[];
};
```

2. 送信時には、実際のデータサイズを指定してsendPacketを呼び出します：

```cpp
size_t totalSize = sizeof(VariableLengthData) + actualDataSize;
VariableLengthData* vdata = (VariableLengthData*)malloc(totalSize);
// データを設定
communicator.sendPacket(reinterpret_cast<ofxBinaryPacket<VariableLengthData>&>(*vdata), totalSize);
free(vdata);
```

3. 受信時には、受信したサイズを使用してデータを適切に処理します：

```cpp
communicator.onReceived = [](const ofxBinaryPacket<VariableLengthData>& packet, size_t size) {
    size_t dataSize = size - sizeof(VariableLengthData);
    // データを処理
};
```

注意：可変長データを使用する場合は、バッファオーバーフローを防ぐために適切なエラーチェックを行ってください。

## ライセンス

このライブラリはMITライセンスの下で公開されています。

## 作者

tettou771

## 貢献

バグ報告や機能リクエストは、GitHubのIssueで受け付けています。プルリクエストも歓迎いたします。
