#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "soc/uart_reg.h"

#define TEST_VERSION
#define UART_USED_NUM UART_NUM_0
#define BUF_SIZE (1024)
#define LED_BUILTIN GPIO_NUM_2

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
static QueueHandle_t uart_data_queue = NULL;
static const char *TAG = "uart_events";

uint8_t rxbuf[BUF_SIZE/4];

static void IRAM_ATTR uart_intr_handle(void *arg)
{
#ifdef TEST_VERSION
    int level;

    level = gpio_get_level(LED_BUILTIN);    
    level = ((level != 0) ? 0 : 1);
    gpio_set_level(LED_BUILTIN, (uint32_t)level);
    uart_flush(UART_USED_NUM);
#else
    uint16_t rx_fifo_len;
    static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    int i = 0;

    // read data length in UART buffer 
    ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_USED_NUM, (size_t *)&rx_fifo_len));
    // read data from UART buffer
    rx_fifo_len = uart_read_bytes(UART_USED_NUM, rxbuf, rx_fifo_len, 100);
    // clear interrupt status
    uart_clear_intr_status(UART_USED_NUM, UART_RXFIFO_FULL_INT_CLR | UART_RXFIFO_TOUT_INT_CLR);

    xSemaphoreGiveFromISR(sem_new_uart_data, &xHigherPriorityTaskWoken);
    
    while (rx_fifo_len--)
    {
        xQueueSendToBackFromISR(uart_data_queue, rxbuf[i], &xHigherPriorityTaskWoken);
        i++;
    }    

    if (xHigherPriorityTaskWoken)
    {
        portYIELD_FROM_ISR();
    }
#endif
}

void send_data_BT(void * parameter)
{
    while (1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void parse_uart_data(void * parameter)
{
    while (1)
    {
        xSemaphoreTake(sem_new_uart_data, portMAX_DELAY);
    }
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

#ifdef TEST_VERSION
    gpio_pad_select_gpio(LED_BUILTIN);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(LED_BUILTIN, GPIO_MODE_OUTPUT);

    gpio_set_level(LED_BUILTIN, 0x00);
#endif

    // Create semaphore before it is used (in task or ISR)
    sem_new_uart_data = xSemaphoreCreateBinary();

    // Force reboot if we can't create the semaphore
    if (sem_new_uart_data == NULL) 
    {
        esp_restart();
    }

    // We want the done new serial data semaphore to initialize to 1
    // xSemaphoreGive(sem_new_uart_data);
    xSemaphoreTake(sem_new_uart_data, 0);   

    uart_data_queue = xQueueCreate(BUF_SIZE/2);

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

    while (1)
    {
        // program should never get here
    }
}