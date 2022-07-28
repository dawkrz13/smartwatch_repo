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
#define WRIST (uint8_t)0x01

#define LED_BUILTIN GPIO_NUM_2
#define UART_USED_NUM UART_NUM_2
#define BUF_SIZE (1024)

static const char *TAG = "uart_events";
static intr_handle_t handle_console;
uint8_t rxbuf[10];
static QueueHandle_t uart_data_queue = NULL;

BLECharacteristic heartRateMeasurementCharacteristics(BLEUUID((uint16_t)0x2A37), BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ);
BLECharacteristic sensorPositionCharacteristic(BLEUUID((uint16_t)0x2A38), BLECharacteristic::PROPERTY_READ);
BLEDescriptor heartRateDescriptor(BLEUUID((uint16_t)0x2901));
BLEDescriptor heartRateNotificationDescriptor(BLEUUID((uint16_t)0x2902));
BLEDescriptor sensorPositionDescriptor(BLEUUID((uint16_t)0x2901));

const uint8_t notificationsEnabled = NOTIFICATIONS_ON;
byte sensorPositionVal[] = {WRIST};
byte heart[8] = { 0b00001110, 0, 0, 0, 0 , 0, 0, 0};

bool _BLEClientConnected = false;

// Required for using C++ with ESP-IDF for ESP32
extern "C" {
	void app_main(void);
}

static void IRAM_ATTR uart_intr_handle(void *arg)
{
    uint16_t rx_fifo_len = 0;
    int i = 0;
    static BaseType_t xHigherPriorityTaskWoken = pdFALSE;

#ifdef TEST_VERSION
    static int level;

    level = ((level != 0) ? 0 : 1);
    gpio_set_level(LED_BUILTIN, (uint32_t)level);
#endif

    // read data length in UART buffer 
    uart_get_buffered_data_len(UART_USED_NUM, (size_t *)&rx_fifo_len);

    // read data from UART buffer
    rx_fifo_len = uart_read_bytes(UART_USED_NUM, rxbuf, rx_fifo_len, 100);
    uart_flush(UART_USED_NUM);

    while (rx_fifo_len--)
    {
        xQueueSendToBackFromISR(uart_data_queue, &rxbuf[i], &xHigherPriorityTaskWoken);
        i++;
    } 

    uart_clear_intr_status(UART_USED_NUM, 0x7FF);
}

void configure_serial_port(void)
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
        .rx_flow_ctrl_thresh = 122,
    };
    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(UART_USED_NUM, &uart_config));

    //Set UART log level
    esp_log_level_set(TAG, ESP_LOG_INFO);

    //Set UART pins (using UART2 default pins ie no changes.)
    ESP_ERROR_CHECK(uart_set_pin(UART_USED_NUM, 4, 5, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    //Install UART driver, and get the queue.
    ESP_ERROR_CHECK(uart_driver_install(UART_USED_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 0, NULL, ESP_INTR_FLAG_IRAM));

    // release the pre registered UART handler/subroutine
    ESP_ERROR_CHECK(uart_isr_free(UART_USED_NUM));

    // register new UART subroutine
    ESP_ERROR_CHECK(uart_isr_register(UART_USED_NUM, uart_intr_handle, NULL, ESP_INTR_FLAG_IRAM, &handle_console));

    // enable RX interrupt
    ESP_ERROR_CHECK(uart_enable_rx_intr(UART_USED_NUM));
}

// Task which generates random numbers between 80-180 and sends it in notification
class bleMessageTask: public Task {
	void run(void *data) {
	
    static uint8_t bpm = 0;
    uint16_t energyUsed = 3000u;
		
    while(1) {

      xQueueReceive(uart_data_queue, &bpm, 0);

      heart[1] = (byte)bpm;
      heart[2] = (byte)(energyUsed & 0xFF);
      heart[3] = (byte)((energyUsed & 0xFF00) >> 8);
      
      #ifdef TEST_VERSION
      printf("BPM: %d\n",bpm);
      #endif

      heartRateMeasurementCharacteristics.setValue(heart, 8);
      heartRateMeasurementCharacteristics.notify();

      sensorPositionCharacteristic.setValue(sensorPositionVal, 2);

      delay(500);
		} // While 1
	} // run
}; // bleMessageTask

#ifdef TEST_VERSION
class ledBlinkTask: public Task {
	void run(void *data) {
	
		while(1) {
      
      printf("*LED blink*\n");

      delay(3000);
		} // While 1
	} // run
}; // bleMessageTask
#endif

bleMessageTask * pBpmNotifyTask;
#ifdef TEST_VERSION
ledBlinkTask * pLedStatePrintTask;
#endif

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      printf("Connected!\n");
      pBpmNotifyTask->start();
      pLedStatePrintTask->start();
    };

    void onDisconnect(BLEServer* pServer) {
      pBpmNotifyTask->stop();
      printf("Disconnected!\n");
      #ifdef TEST_VERSION
      pLedStatePrintTask->stop();
      #endif
    }
};

static void run() 
{
  pBpmNotifyTask = new bleMessageTask();
  pBpmNotifyTask->setStackSize(8000);
  pBpmNotifyTask->setPriority(2);

  #ifdef TEST_VERSION
  pLedStatePrintTask = new ledBlinkTask();
  pLedStatePrintTask->setStackSize(8000);
  pLedStatePrintTask->setPriority(1);
  #endif

  // Configure Serial
  configure_serial_port();

  uart_data_queue = xQueueCreate(10, sizeof(uint8_t));

#ifdef TEST_VERSION
  gpio_pad_select_gpio(LED_BUILTIN);
  /* Set the GPIO as a push/pull output */
  gpio_set_direction(LED_BUILTIN, GPIO_MODE_OUTPUT);

  gpio_set_level(LED_BUILTIN, 0x00);
#endif

  printf("Starting BLE setup!\n");

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

  printf("Connect using a smartphone App to this 'HRM Smartphone'\n");
  printf("Use nRFToolbox or nRFConnect App which supports Hear Rate Monitor\n");

}

void app_main()
{
  run();
}
