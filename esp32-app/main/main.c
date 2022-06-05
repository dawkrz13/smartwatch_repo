#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"

#define UART_USED_NUM UART_NUM_2
#define BUF_SIZE (1024)

// Use only core 1
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

static intr_handle_t handle_console;
static SemaphoreHandle_t sem_new_uart_data = NULL;
static TaskHandle_t bluetooth_task = NULL;
static TaskHandle_t data_parse_task = NULL;
static const char *TAG = "uart_events";


static void IRAM_ATTR uart_intr_handle(void *arg)
{
  
}

void send_data_BT(void * parameter)
{

}

void parse_uart_data(void * parameter)
{
    
}

void configure_serial_port()
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
    ESP_ERROR_CHECK(uart_set_pin(UART_USED_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    //Install UART driver, and get the queue.
    ESP_ERROR_CHECK(uart_driver_install(UART_USED_NUM, BUF_SIZE * 2, 0, 0, NULL, 0));

    // release the pre registered UART handler/subroutine
    ESP_ERROR_CHECK(uart_isr_free(UART_USED_NUM));

    // register new UART subroutine
    ESP_ERROR_CHECK(uart_isr_register(UART_USED_NUM, uart_intr_handle, NULL, ESP_INTR_FLAG_IRAM, &handle_console));

    // enable RX interrupt
    ESP_ERROR_CHECK(uart_enable_rx_intr(UART_USED_NUM));
}


void app_main(void)
{   
    // Configure Serial
    configure_serial_port();

    // Create semaphore before it is used (in task or ISR)
    sem_new_uart_data = xSemaphoreCreateBinary();

    // Force reboot if we can't create the semaphore
    if (sem_new_uart_data == NULL) 
    {
        esp_restart();
    }

    // We want the done new serial data semaphore to initialize to 1
    xSemaphoreGive(sem_new_uart_data);

    xTaskCreatePinnedToCore(send_data_BT,
                            "Send data to Qt app via BT",
                            1024,
                            NULL,
                            2,
                            &bluetooth_task,
                            app_cpu);

    xTaskCreatePinnedToCore(parse_uart_data,
                            "Parse data received via UART",
                            1024,
                            NULL,
                            1,
                            &data_parse_task,
                            app_cpu);

    vTaskDelete(NULL);

    while(1)
    {
        // program should never get here
    }
}