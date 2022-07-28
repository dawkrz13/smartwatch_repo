#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define EX_UART_NUM UART_NUM_2
#define BUF_SIZE (1024)

// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

static intr_handle_t handle_console;
static SemaphoreHandle_t sem_new_serial_data = NULL;
static const char *TAG = "uart_events";

void send_data_BT(void *parameter)
{
  while(1)
  {
    Serial.print("Time: ");
    Serial.print(millis()/1000);
    Serial.println("send_data_BT task");
    vTaskDelay(100);
  }
}


void parse_serial_data(void *parameter)
{
  while(1)
  {
    Serial.print("Time: ");
    Serial.print(millis());
    Serial.println("parse_serial_data task");
    vTaskDelay(100);
  }
}

static void IRAM_ATTR uart_intr_handle(void *arg)
{
  
}

void configureSerialPort()
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
  ESP_ERROR_CHECK(uart_param_config(EX_UART_NUM, &uart_config));
  
  //Set UART log level
  esp_log_level_set(TAG, ESP_LOG_INFO);

  //Set UART pins (using UART0 default pins ie no changes.)
  ESP_ERROR_CHECK(uart_set_pin(EX_UART_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

  //Install UART driver, and get the queue.
  ESP_ERROR_CHECK(uart_driver_install(EX_UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0));

  // release the pre registered UART handler/subroutine
  ESP_ERROR_CHECK(uart_isr_free(EX_UART_NUM));

  // register new UART subroutine
  ESP_ERROR_CHECK(uart_isr_register(EX_UART_NUM, uart_intr_handle, NULL, ESP_INTR_FLAG_IRAM, &handle_console));

  // enable RX interrupt
  ESP_ERROR_CHECK(uart_enable_rx_intr(EX_UART_NUM))
}


void setup() {

  // Configure Serial
  Serial.begin(115200);

  // Wait a moment to start (so we don't miss Serial output)
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("---Smartwatch prototype project---");

  configureSerialPort();

  // Create semaphore before it is used (in task or ISR)
  sem_new_serial_data = xSemaphoreCreateBinary();

  // Force reboot if we can't create the semaphore
  if (sem_new_serial_data == NULL) {
    Serial.println("Could not create a semaphore");
    ESP.restart();
  }

  // We want the done new serial data semaphore to initialize to 1
  xSemaphoreGive(sem_new_serial_data);

  xTaskCreatePinnedToCore(send_data_BT,
                          "Send data to Qt app via BT",
                          1024,
                          NULL,
                          2,
                          &bluetooth_task,
                          app_cpu);

  xTaskCreatePinnedToCore(parse_serial_data,
                          "Parse data received via UART",
                          1024,
                          NULL,
                          1,
                          &data_parse_task,
                          app_cpu);

  // Delete "setup and loop" task
  vTaskDelete(NULL);
}

void loop() {
  // Execution should never get here
}
