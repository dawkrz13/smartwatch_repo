extern "C"
{
    #include "main.h"
}

extern "C" void SystemClock_Config(void);
extern "C" void MX_GPIO_Init(void);
extern "C" void MX_USART2_UART_Init(void);
extern "C" void MX_I2C2_Init(void);

namespace smartwatch::bsp {

    void init()
    {
        HAL_Init();
        SystemClock_Config();
        MX_GPIO_Init();
        MX_USART2_UART_Init();
        MX_I2C2_Init();
    }

} // namespace smartwatch::bsp
