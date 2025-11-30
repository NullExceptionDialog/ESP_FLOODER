#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

// Compact configuration - Reduced to 8 of each to fit memory
const char* softAP_ssids[] = {
  "MUSTAFA WIFI",
  "FiberHGW_ZTED5A",
  "TurkTelekom_TPB83C5", 
  "Redmi",
  "VODAFONE_1399_5G",
  "Natasha's iPhone",
  "TP-Link_AF52",
  "HUAWEI-B535"
};

const char* ble_device_names[] = {
  "Mustafa's AirPods",
  "Natasha's AirPods",
  "Redmi",
  "SONY-XM5",
  "BSK V3 X HS",
  "iPhone",
  "Samsung Buds",
  "JBL Flip 5"
};

// Single subnet to save memory
IPAddress local_IP(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

// BLE UUIDs - generated sequentially to save space
const char* ble_service_uuid = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
const char* ble_char_uuid = "beb5483e-36e1-4688-b7f5-ea07361b26a8";

// Global BLE objects - minimal
BLEServer* pServer = NULL;
BLEService* pService = NULL;
BLECharacteristic* pCharacteristic = NULL;

class SimpleServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      Serial.println("BLE Connected");
    }
    void onDisconnect(BLEServer* pServer) {
      Serial.println("BLE Disconnected - Restarting adv");
      BLEDevice::startAdvertising();
    }
};

class SimpleCharCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      Serial.print("BLE Received: ");
      Serial.println(pCharacteristic->getValue().c_str());
    }
};

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("Starting ESP32 Multi-Hotspot");
  Serial.println("Memory Optimized Version");
  Serial.println("=========================");

  // Setup WiFi in AP mode
  WiFi.mode(WIFI_AP);
  
  // Setup SoftAPs sequentially
  Serial.println("Setting up WiFi APs...");
  for (int i = 0; i < 8; i++) {
    WiFi.softAPConfig(local_IP, gateway, subnet);
    if (WiFi.softAP(softAP_ssids[i], NULL, (i % 13) + 1)) {
      Serial.print("AP ");
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.println(softAP_ssids[i]);
    }
    delay(200);
  }

  // Setup BLE - Single server with cycling names
  Serial.println("Setting up BLE...");
  BLEDevice::init("ESP32-BLE");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new SimpleServerCallbacks());
  pService = pServer->createService(ble_service_uuid);
  pCharacteristic = pService->createCharacteristic(
    ble_char_uuid,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_WRITE
  );
  pCharacteristic->setCallbacks(new SimpleCharCallbacks());
  pCharacteristic->setValue("Hello from ESP32");
  pService->start();
  
  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(ble_service_uuid);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->start();

  Serial.println("Setup Complete!");
  Serial.print("Free Heap: ");
  Serial.println(ESP.getFreeHeap());
  Serial.println("Scan for MUSTAFA WIFI and other networks!");
}

unsigned long lastBleUpdate = 0;
int currentBleIndex = 0;

void loop() {
  // Cycle BLE device names every 5 seconds
  if (millis() - lastBleUpdate > 5000) {
    lastBleUpdate = millis();
    
    // Stop current advertising
    BLEDevice::stopAdvertising();
    
    // Re-initialize with new name
    BLEDevice::deinit();
    BLEDevice::init(ble_device_names[currentBleIndex]);
    
    // Recreate server and service
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new SimpleServerCallbacks());
    pService = pServer->createService(ble_service_uuid);
    pCharacteristic = pService->createCharacteristic(
      ble_char_uuid,
      BLECharacteristic::PROPERTY_READ |
      BLECharacteristic::PROPERTY_WRITE
    );
    pCharacteristic->setCallbacks(new SimpleCharCallbacks());
    pCharacteristic->setValue(ble_device_names[currentBleIndex]);
    pService->start();
    
    // Restart advertising
    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(ble_service_uuid);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);
    pAdvertising->start();
    
    Serial.print("BLE now advertising as: ");
    Serial.println(ble_device_names[currentBleIndex]);
    
    currentBleIndex = (currentBleIndex + 1) % 8;
  }

  // Print status every 30 seconds
  static unsigned long lastStatus = 0;
  if (millis() - lastStatus > 30000) {
    lastStatus = millis();
    Serial.println("=== STATUS ===");
    Serial.print("WiFi Clients: ");
    Serial.println(WiFi.softAPgetStationNum());
    Serial.print("Free Heap: ");
    Serial.println(ESP.getFreeHeap());
    Serial.println("==============");
  }
  
  delay(1000);
}
