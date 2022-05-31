#include "bsp_com.hpp"
#include "main.h"
#include "stm32l4xx_hal.h"

extern "C" UART_HandleTypeDef huart2;

namespace smartwatch::bsp
{
    std::span<char const> write(std::span<char const> message)
    {
        HAL_UART_Transmit(&huart2, (uint8_t*)message.data(), message.size(), HAL_MAX_DELAY);
        return {};
    }

    std::span<char> read(std::span<char> buffer)
    {
        HAL_UART_Receive(&huart2, (uint8_t*)buffer.data(), 1, HAL_MAX_DELAY);
        return buffer;
    }
}
