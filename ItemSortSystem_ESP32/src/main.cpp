#include <Arduino.h>
#include <DHT.h>
#include <WiFi.h>
#include <WiFiClient.h>

// DHT11 设置
#define DHTPIN 4         
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// WiFi 设置
const char* ssid = "test";
const char* password = "123456789";

// TCP 客户端设置
WiFiClient client;
const char* serverAddress = "192.168.147.27"; 
int serverPort = 1123;

char buffer[30];
float h;
float t;

// TCP 客户端任务
void tcpClientTask(void* pvParameters) {

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  while (true) {
    if (client.connect(serverAddress, serverPort)) {
      sprintf(buffer,"th:%.2f,%.2f", t, h);
      client.print(buffer);
      delay(5000);
      client.stop();
    } else {
      delay(5000);
    }
  }
}

// DHT11 读取任务

void dhtTask(void* pvParameters) {

  dht.begin();

  while (true) {

    h = dht.readHumidity();
    t = dht.readTemperature();
    vTaskDelay(2000 / portTICK_PERIOD_MS); // 每 2 秒读取一次
  }
}

// 主程序
void setup() {
  Serial.begin(115200);

  // 创建 TCP 客户端任务
  xTaskCreate(
    tcpClientTask,
    "TCP_Client",
    2000,
    NULL,
    1,
    NULL
  );

  // 创建 DHT11 读取任务
  xTaskCreate(
    dhtTask,
    "DHT_Task",
    2000,
    NULL,
    1,
    NULL
  );

  // 启动调度器
  vTaskStartScheduler();
}

void loop() {
  // 主循环中什么都不做，任务由 FreeRTOS 调度
}