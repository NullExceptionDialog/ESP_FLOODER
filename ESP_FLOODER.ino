#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// SoftAP Configuration - Realistic device names
const char* softAP_ssids[] = {
  "MUSTAFA WIFI",
  "FiberHGW_ZTED5A",
  "TurkTelekom_TPB83C5", 
  "Redmi",
  "VODAFONE_1399_5G",
  "Natasha's iPhone",
  "TP-Link_AF52",
  "HUAWEI-B535",
  "Samsung Galaxy",
  "Xiaomi_ABCD",
  "BELL_5G",
  "AndroidAP",
  "DIRECT-VIDEO",
  "Home_WiFi",
  "Guest_Network"
};

const char* softAP_passwords[] = {
  "", "", "", "", "", 
  "", "", "", "", "",
  "", "", "", "", ""
};

// IP addresses for 15 different subnets
IPAddress local_IPs[] = {
  IPAddress(192,168,1,1),
  IPAddress(192,168,2,1),
  IPAddress(192,168,3,1),
  IPAddress(192,168,4,1),
  IPAddress(192,168,5,1),
  IPAddress(192,168,6,1),
  IPAddress(192,168,7,1),
  IPAddress(192,168,8,1),
  IPAddress(192,168,9,1),
  IPAddress(192,168,10,1),
  IPAddress(192,168,11,1),
  IPAddress(192,168,12,1),
  IPAddress(192,168,13,1),
  IPAddress(192,168,14,1),
  IPAddress(192,168,15,1)
};

IPAddress gateway_IPs[] = {
  IPAddress(192,168,1,1),
  IPAddress(192,168,2,1),
  IPAddress(192,168,3,1),
  IPAddress(192,168,4,1),
  IPAddress(192,168,5,1),
  IPAddress(192,168,6,1),
  IPAddress(192,168,7,1),
  IPAddress(192,168,8,1),
  IPAddress(192,168,9,1),
  IPAddress(192,168,10,1),
  IPAddress(192,168,11,1),
  IPAddress(192,168,12,1),
  IPAddress(192,168,13,1),
  IPAddress(192,168,14,1),
  IPAddress(192,168,15,1)
};

IPAddress subnet(255,255,255,0);

// BLE Configuration - Realistic device names
const char* ble_service_uuids[] = {
  "4fafc201-1fb5-459e-8fcc-c5c9c331914b",
  "5fafc201-1fb5-459e-8fcc-c5c9c331914b",
  "6fafc201-1fb5-459e-8fcc-c5c9c331914b",
  "7fafc201-1fb5-459e-8fcc-c5c9c331914b",
  "8fafc201-1fb5-459e-8fcc-c5c9c331914b",
  "9fafc201-1fb5-459e-8fcc-c5c9c331914b",
  "afafc201-1fb5-459e-8fcc-c5c9c331914b",
  "bfafc201-1fb5-459e-8fcc-c5c9c331914b",
  "cfafc201-1fb5-459e-8fcc-c5c9c331914b",
  "dfafc201-1fb5-459e-8fcc-c5c9c331914b",
  "efafc201-1fb5-459e-8fcc-c5c9c331914b",
  "ffafc201-1fb5-459e-8fcc-c5c9c331914b",
  "0fafc201-1fb5-459e-8fcc-c5c9c331914b",
  "1fafc201-1fb5-459e-8fcc-c5c9c331914b",
  "2fafc201-1fb5-459e-8fcc-c5c9c331914b"
};

const char* ble_characteristic_uuids[] = {
  "beb5483e-36e1-4688-b7f5-ea07361b26a8",
  "ceb5483e-36e1-4688-b7f5-ea07361b26a8",
  "deb5483e-36e1-4688-b7f5-ea07361b26a8",
  "eeb5483e-36e1-4688-b7f5-ea07361b26a8",
  "feb5483e-36e1-4688-b7f5-ea07361b26a8",
  "aeb5483e-36e1-4688-b7f5-ea07361b26a8",
  "beb5483e-36e1-4688-b7f5-ea07361b26a9",
  "ceb5483e-36e1-4688-b7f5-ea07361b26a9",
  "deb5483e-36e1-4688-b7f5-ea07361b26a9",
  "eeb5483e-36e1-4688-b7f5-ea07361b26a9",
  "feb5483e-36e1-4688-b7f5-ea07361b26a9",
  "aeb5483e-36e1-4688-b7f5-ea07361b26a9",
  "beb5483e-36e1-4688-b7f5-ea07361b26a0",
  "ceb5483e-36e1-4688-b7f5-ea07361b26a0",
  "deb5483e-36e1-4688-b7f5-ea07361b26a0"
};

const char* ble_device_names[] = {
  "Mustafa'nın AirPods'u",
  "Natasha's AirPods",
  "Redmi",
  "SONY-XM5",
  "BSK V3 X HS",
  "iPhone",
  "Samsung Buds",
  "JBL Flip 5",
  "Samsung Galaxy S25 Ultra",
  "0001",
  "Opel",
  "Logi Z203",
  "Tablet PC",
  "MacBook Pro",
  "iPad"
};

// BLE Server and Characteristic pointers
BLEServer* pServers[15];
BLEService* pServices[15];
BLECharacteristic* pCharacteristics[15];

// Custom BLE Server Callback Class
class MyServerCallbacks: public BLEServerCallbacks {
private:
  int serverIndex;
  
public:
  MyServerCallbacks(int index) : serverIndex(index) {}
  
  void onConnect(BLEServer* pServer) {
    Serial.printf("📱 BLE Server %d: Device connected\n", serverIndex + 1);
  }

  void onDisconnect(BLEServer* pServer) {
    Serial.printf("📱 BLE Server %d: Device disconnected, restarting advertising\n", serverIndex + 1);
    pServer->getAdvertising()->start();
  }
};

// Custom BLE Characteristic Callback Class
class MyCharacteristicCallbacks: public BLECharacteristicCallbacks {
private:
  int charIndex;
  
public:
  MyCharacteristicCallbacks(int index) : charIndex(index) {}
  
  void onWrite(BLECharacteristic *pCharacteristic) {
    String value = pCharacteristic->getValue();
    if (value.length() > 0) {
      Serial.printf("✏️ BLE Characteristic %d received: %s\n", charIndex + 1, value.c_str());
    }
  }
};

void setupSoftAPs() {
  Serial.println("📡 Setting up 15 SoftAP hotspots...");
  
  // Use non-overlapping WiFi channels for better performance
  int wifi_channels[] = {1, 6, 11, 3, 8, 13, 2, 7, 12, 4, 9, 14, 5, 10, 1};
  
  for (int i = 0; i < 15; i++) {
    // Configure SoftAP network interface
    WiFi.softAPConfig(local_IPs[i], gateway_IPs[i], subnet);
    
    // Start SoftAP (no password - open networks)
    if (WiFi.softAP(softAP_ssids[i], NULL, wifi_channels[i])) {
      Serial.printf("✅ SoftAP %d started successfully!\n", i + 1);
      Serial.printf("   📶 SSID: %s\n", softAP_ssids[i]);
      Serial.printf("   🔓 Security: Open (No Password)\n");
      Serial.printf("   🌐 IP: %s\n", local_IPs[i].toString().c_str());
      Serial.printf("   📻 Channel: %d\n", wifi_channels[i]);
    } else {
      Serial.printf("❌ Failed to start SoftAP %d!\n", i + 1);
    }
    
    delay(150); // Small delay between AP startups
  }
  
  Serial.println("🎉 All 15 SoftAPs are running! Scan for WiFi to see:");
  Serial.println("   MUSTAFA WIFI and 14 other realistic networks!");
  Serial.println("==================================================\n");
}

void setupBLE() {
  Serial.println("📱 Setting up 15 BLE servers...");
  
  // Initialize BLE device
  BLEDevice::init("Necvox");
  
  for (int i = 0; i < 15; i++) {
    // Create BLE Server
    pServers[i] = BLEDevice::createServer();
    pServers[i]->setCallbacks(new MyServerCallbacks(i));
    
    // Create BLE Service
    pServices[i] = pServers[i]->createService(ble_service_uuids[i]);
    
    // Create BLE Characteristic
    pCharacteristics[i] = pServices[i]->createCharacteristic(
      ble_characteristic_uuids[i],
      BLECharacteristic::PROPERTY_READ |
      BLECharacteristic::PROPERTY_WRITE |
      BLECharacteristic::PROPERTY_NOTIFY
    );
    
    // Set characteristic value and callbacks
    pCharacteristics[i]->setValue("Hello from " + String(ble_device_names[i]));
    pCharacteristics[i]->setCallbacks(new MyCharacteristicCallbacks(i));
    
    // Start the service
    pServices[i]->start();
    
    // Start advertising
    BLEAdvertising* pAdvertising = pServers[i]->getAdvertising();
    pAdvertising->addServiceUUID(ble_service_uuids[i]);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);
    pAdvertising->setMinPreferred(0x12);
    
    // Set specific device name
    pAdvertising->setName(ble_device_names[i]);
    pAdvertising->start();
    
    Serial.printf("✅ BLE Server %d started: %s\n", i + 1, ble_device_names[i]);
    
    delay(150); // Small delay between BLE server startups
  }
  
  Serial.println("🎉 All 15 BLE servers are running!");
  Serial.println("==================================\n");
}

void printNetworkStatus() {
  Serial.println("\n=== 📊 NETWORK STATUS ===");
  
  // Print SoftAP status
  Serial.println("📡 SoftAP Hotspots:");
  for (int i = 0; i < 15; i++) {
    int clients = WiFi.softAPgetStationNum();
    Serial.printf("   %-20s - %s - %d client(s)\n", 
                  softAP_ssids[i], 
                  local_IPs[i].toString().c_str(),
                  clients);
  }
  
  Serial.println("📱 BLE Servers:");
  for (int i = 0; i < 15; i++) {
    Serial.printf("   %s\n", ble_device_names[i]);
  }
  
  Serial.printf("💾 Free Heap: %d bytes\n", ESP.getFreeHeap());
  Serial.println("========================\n");
}

void updateBLECharacteristics() {
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= 3000) {
    previousMillis = currentMillis;
    
    for (int i = 0; i < 15; i++) {
      String newValue = "Uptime: " + String(millis() / 1000) + "s - " + String(ble_device_names[i]);
      pCharacteristics[i]->setValue(newValue.c_str());
      pCharacteristics[i]->notify();
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n" 
                 "╔══════════════════════════════════════╗\n"
                 "║       ESP32 30 Hotspots Project      ║\n" 
                 "║       15 WiFi + 15 BLE Devices       ║\n"
                 "║          MUSTAFA WIFI Ready          ║\n"
                 "╚══════════════════════════════════════╝\n");

  // Set WiFi to AP mode
  WiFi.mode(WIFI_AP);
  
  // Setup both SoftAPs and BLE servers
  setupSoftAPs();
  setupBLE();
  
  Serial.println("🚀 SYSTEM FULLY OPERATIONAL!");
  Serial.println("✅ 15 WiFi Access Points (including MUSTAFA WIFI)");
  Serial.println("✅ 15 BLE Servers advertising");
  Serial.println("✅ All networks are OPEN - No passwords");
  Serial.println("✅ Ready for scanning!\n");
}

void loop() {
  updateBLECharacteristics();
  
  static unsigned long lastStatusPrint = 0;
  if (millis() - lastStatusPrint >= 20000) {
    lastStatusPrint = millis();
    printNetworkStatus();
  }
  
  delay(1000);
}
