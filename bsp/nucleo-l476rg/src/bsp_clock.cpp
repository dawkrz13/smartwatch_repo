#include "bsp_clock.hpp"

#include "stm32l4xx_hal.h"

namespace smartwatch::bsp {

    void delay(std::chrono::milliseconds duration) {
        HAL_Delay(duration.count());
    }

} // namespace smartwatch::bsp
