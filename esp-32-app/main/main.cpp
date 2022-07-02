/*
	HEART RATE MONITOR BLE Service with BLE Arduino library as ESP-IDF component
	- by Sukesh Ashok Kumar


    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updates by chegewara
*/
#include <stdio.h>
#include <esp_log.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "BLE2902.h"
#include "Task.h"
#include "math.h"
#include "soc/uart_reg.h"
#include "driver/uart.h"
#include "driver/gpio.h"

#define TEST_VERSION

#define byte uint8_t
#define heartRateService BLEUUID((uint16_t)0x180D)
#define NOTIFICATIONS_ON 0x01
#define NOTIFICATIONS_OFF 0x00

#define UART_USED_NUM UART_NUM_0
#define BUF_SIZE 256
#define AVG_SAMPLE_MIN_LEN 10

enum {
  OTHER,
  CHEST,
  WRIST,
  FINGER,
  HAND,
  EAR_LOBE,
  FOOT
};

static const char *TAG = "uart_events";
static intr_handle_t handle_console;
static QueueHandle_t uart_data_queue = NULL;
static uart_event_t event;

uint8_t rxbuf[10];
static uint8_t bpm = 0;

BLECharacteristic heartRateMeasurementCharacteristics(BLEUUID((uint16_t)0x2A37), BLECharacteristic::PROPERTY_NOTIFY);
BLECharacteristic sensorPositionCharacteristic(BLEUUID((uint16_t)0x2A38), BLECharacteristic::PROPERTY_READ);
BLEDescriptor heartRateDescriptor(BLEUUID((uint16_t)0x2901));
BLEDescriptor heartRateNotificationDescriptor(BLEUUID((uint16_t)0x2902));
BLEDescriptor sensorPositionDescriptor(BLEUUID((uint16_t)0x2901));

const uint8_t notificationsEnabled = NOTIFICATIONS_ON;
byte sensorPositionVal[] = {FINGER};
#ifdef ENERGY_EXP_SUPPORTED
byte heart[8] = { 0b00001110, 0, 0, 0, 0, 0, 0, 0};
#else
byte heart[8] = { 0b00000110, 0, 0, 0, 0, 0, 0, 0};
#endif

bool _BLEClientConnected = false;

// Required for using C++ with ESP-IDF for ESP32
extern "C" {
	void app_main(void);
}


void configure_serial_port(void)
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(UART_USED_NUM, &uart_config));

    //Set UART log level
    esp_log_level_set(TAG, ESP_LOG_INFO);

    //Set UART pins (using UART0 default pins ie no changes.)
    ESP_ERROR_CHECK(uart_set_pin(UART_USED_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    //Install UART driver, and get the queue.
    ESP_ERROR_CHECK(uart_driver_install(UART_USED_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 10, &uart_data_queue, 0));
}

// Task which handles BLE communication
class bleMessageTask: public Task {
	void run(void *data) {
	
    uint16_t energyUsed = 3000u;
		
    while(1) {

      heart[1] = (byte)bpm;

      #ifdef ENERGY_EXP_SUPPORTED
      heart[2] = (byte)(energyUsed & 0xFF);
      heart[3] = (byte)((energyUsed & 0xFF00) >> 8);
      #endif

      heartRateMeasurementCharacteristics.setValue(heart, 8);
      heartRateMeasurementCharacteristics.notify();

      sensorPositionCharacteristic.setValue(sensorPositionVal, 1);

      delay(500);
		} // While 1
	} // run
}; // bleMessageTask

// Task which reads and stores data received via UART interface 
class uartTask: public Task {
	void run(void *data) {
	
    static uint8_t data_in[BUF_SIZE] = {0};
		static uint8_t sample_cnt_total = 0;
    static uint8_t sample_cnt_temp = 0;
    static uint8_t bpm_samples[AVG_SAMPLE_MIN_LEN] = {0};
    static uint8_t sample_num = 0;

    while(1) {

      if (xQueueReceive(uart_data_queue, (void *)&event, 0))
      {
        #ifdef TEST_VERSION
        ESP_LOGI(TAG, "uart_%d event", UART_USED_NUM);
        #endif
        memset(data_in, 0, BUF_SIZE);

        switch(event.type)
        {
          case UART_DATA:
            sample_cnt_temp = uart_read_bytes(UART_USED_NUM, data_in, event.size, (portTickType)(0 / portTICK_PERIOD_MS));
          #ifdef TEST_VERSION
            ESP_LOGI(TAG, "UART DATA EVENT");
            ESP_LOGI(TAG, "UART DATA SIZE: %d", event.size);
            uart_write_bytes(UART_USED_NUM, (const uint8_t *)data_in, event.size);
          #endif
            break;

          default:
          #ifdef TEST_VERSION
            ESP_LOGI(TAG, "UART EVENT: %d", event.type);
          #endif
            uart_flush_input(UART_USED_NUM);
            break;
        }

        // read all samples that were received, replace oldest ones with newer
        for (int i = 0; i < sample_cnt_temp; i++)
        {
          bpm_samples[sample_num] = data_in[i];
          printf("bpm_samples[%u] = data_in[%d] = %u\n", sample_num, i, data_in[i]);
          sample_num = ((sample_num + 1) % AVG_SAMPLE_MIN_LEN);
        }

        printf("sample_cnt_total = %u\n", sample_cnt_total);

        // wait till at least 10 first samples are read and then calculate sum
        if (sample_cnt_total >= AVG_SAMPLE_MIN_LEN)
        {
          uint16_t sample_sum = 0;
          
          for (int i = 0; i < AVG_SAMPLE_MIN_LEN; i++)
          {
            sample_sum += bpm_samples[i];
            printf("bpm_samples[%d] = %u, ", i, bpm_samples[i]);
          }

          printf("\n");

          // calculate new mean value of last ten samples
          bpm = (uint8_t)(sample_sum / AVG_SAMPLE_MIN_LEN);

          printf("New bpm: %u\n", bpm);
        }
        else
        {
          sample_cnt_total += sample_cnt_temp;
          printf("Sample count: %u\n", sample_cnt_total);
        }
      }

      delay(200);
		} // While 1
	} // run
}; // uartTask

bleMessageTask * pBpmNotifyTask;
uartTask * pUartReceiveTask;

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      printf("Connected!\n");
      pUartReceiveTask->start();
    };

    void onDisconnect(BLEServer* pServer) {
      pBpmNotifyTask->stop();
      printf("Disconnected!\n");
    }
};

static void run() 
{
  pBpmNotifyTask = new bleMessageTask();
  pBpmNotifyTask->setStackSize(8000);
  pBpmNotifyTask->setPriority(2);

  pUartReceiveTask = new uartTask();
  pUartReceiveTask->setStackSize(8000);
  pUartReceiveTask->setPriority(1);

  // Configure Serial
  configure_serial_port();

  #ifdef TEST_VERSION
  printf("Starting BLE setup!\n");
  #endif

  // BLE Device Name - Look for this name from the App
  BLEDevice::init("HRM Smartwatch");

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pHeart = pServer->createService(heartRateService);

  // Create HRM BLE Characteristics
  pHeart->addCharacteristic(&heartRateMeasurementCharacteristics);

  heartRateDescriptor.setValue("Rate from MAX30100 sensor");
  heartRateMeasurementCharacteristics.addDescriptor(&heartRateDescriptor);
  
  heartRateNotificationDescriptor.setValue(&notificationsEnabled, sizeof(uint8_t));
  heartRateMeasurementCharacteristics.addDescriptor(&heartRateNotificationDescriptor);

  pHeart->addCharacteristic(&sensorPositionCharacteristic);
  sensorPositionDescriptor.setValue("Position 0 - 6");
  sensorPositionCharacteristic.addDescriptor(&sensorPositionDescriptor);
  sensorPositionCharacteristic.setValue(sensorPositionVal, 1);
  pServer->getAdvertising()->addServiceUUID(heartRateService);

  pHeart->start();

  // Start advertising
  pServer->getAdvertising()->start();

  #ifdef TEST_VERSION
  printf("Connect using a smartphone App to this 'HRM Smartphone'\n");
  printf("Use nRFToolbox or nRFConnect App which supports Hear Rate Monitor\n");
  #endif

  pBpmNotifyTask->start();

  vTaskDelete(NULL);
}

void app_main()
{
  run();
}
