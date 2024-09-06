# ofxBinaryCommunicator

ofxBinaryCommunicatorは、ArduinoとopenFrameworksの間で効率的なバイナリ通信を行うためのライブラリです。カスタム定義された構造体データをシリアル通信で送受信し、データの整合性を検証する機能を備えています。

## 特徴

- カスタム定義された構造体の直接送受信
- Fletcherのチェックサムを使用したデータ整合性の検証
- エラーハンドリング機能
- ArduinoとopenFrameworksの両方に対応
- トピックIDを使用した複数のデータ型のサポート
- 信頼性の高いデータ転送のためのエスケープシーケンス処理

## インストール方法

### openFrameworks

1. このリポジトリを`openFrameworks/addons/`フォルダにクローンします。
2. openFrameworksのプロジェクトジェネレーターを使用して、ofxBinaryCommunicatorをプロジェクトに追加します。

### Arduino

1. このリポジトリをZIPファイルとしてダウンロードします。
2. Arduino IDEで、"スケッチ" → "ライブラリをインクルード" → "ZIP形式のライブラリをインストール..." を選択し、ダウンロードしたZIPファイルを選択します。

## 使用方法

1. 送受信したいデータの構造体を定義します：

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

2. ofxBinaryCommunicatorのインスタンスを作成します：

```cpp
ofxBinaryCommunicator communicator;
```

3. コミュニケーターをセットアップします：

```cpp
// openFrameworks
communicator.setup("/dev/ttyUSB0", 115200);

// Arduino
communicator.setup(Serial);
```

4. データを送信します：

```cpp
SampleSensorData data;
data.timestamp = ofGetElapsedTimeMillis(); // または millis()
data.sensorValue = analogRead(A0);
communicator.send(data);
```

5. データを受信します：

```cpp
// openFrameworks
ofAddListener(communicator.onReceived, this, &YourClass::onMessageReceived);

void YourClass::onMessageReceived(const ofxBinaryPacket& packet) {
    switch (packet.topicId) {
        case SampleSensorData::topicId: {
            SampleSensorData sensorData;
            if (packet.unpack(sensorData)) {
                // 受信データを処理
            }
            break;
        }
        // 他のトピックIDを処理...
    }
}

// Arduino
communicator.setReceivedCallback(onMessageReceived);

void onMessageReceived(const ofxBinaryPacket& packet) {
    switch (packet.topicId) {
        case SampleMouseData::topicId: {
            SampleMouseData mouseData;
            if (packet.unpack(mouseData)) {
                // 受信データを処理
            }
            break;
        }
        // 他のトピックIDを処理...
    }
}
```

6. エラーハンドリングをセットアップします：

```cpp
// openFrameworks
ofAddListener(communicator.onError, this, &YourClass::onError);

void YourClass::onError(ofxBinaryCommunicator::ErrorType& errorType) {
    // エラーを処理
}

// Arduino
communicator.setErrorCallback(onError);

void onError(ofxBinaryCommunicator::ErrorType errorType) {
    // エラーを処理
}
```

7. データを処理するために定期的に`update()`を呼び出します：

```cpp
void update() { // Arduinoの場合は void loop()
    communicator.update();
}
```

## サンプル

リポジトリにはArduino用（`ofxBinaryCommunicatorExample.ino`）とopenFrameworks用（`ofApp.cpp`）のサンプルコードが含まれています。これらのサンプルは、コミュニケーターのセットアップ方法、様々な種類のデータの送信方法、受信メッセージの処理方法を示しています。

## カスタマイズ

ライブラリをインクルードする前に`MAX_PACKET_SIZE`を定義することで、最大パケットサイズを調整できます：

```cpp
#define MAX_PACKET_SIZE 512
#include "ofxBinaryCommunicator.h"
```

## ライセンス

このライブラリはMITライセンスの下で公開されています。

## 作者

tettou771

## 貢献

バグ報告、機能リクエスト、プルリクエストはGitHub Issuesで歓迎します。
