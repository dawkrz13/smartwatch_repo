#include "bsp_led.hpp"
#include "stm32l4xx_hal.h"
#include "main.h"

namespace smartwatch::bsp
{
    void toggle_led()
    {
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    }
} // namespace smartwatch::bsp
