#include "main.h"

extern "C" void SystemClock_Config(void);
extern "C" void MX_GPIO_Init(void);

namespace smartwatch::bsp {

    void init()
    {
        HAL_Init();
        SystemClock_Config();
        MX_GPIO_Init();
    }

} // namespace smartwatch::bsp
