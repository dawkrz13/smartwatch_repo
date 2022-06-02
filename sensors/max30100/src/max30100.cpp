extern "C"
{
#include "max30100_for_stm32_hal.h"
//#include "main.h"
}

#include "main.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_i2c.h"

#include "bsp.hpp"
#include "max30100.hpp"

extern "C" UART_HandleTypeDef huart2;
extern "C" I2C_HandleTypeDef hi2c2;

//extern "C" void MAX30100_Init(I2C_HandleTypeDef *ui2c, UART_HandleTypeDef *uuart);

namespace smartwatch::sensor
{
    void MAX30100::doInit()
    {
        MAX30100_Init(&hi2c2, &huart2);
        MAX30100_SetSpO2SampleRate(MAX30100_SPO2SR_DEFAULT);
        MAX30100_SetLEDPulseWidth(MAX30100_LEDPW_DEFAULT);
        MAX30100_SetLEDCurrent(MAX30100_LEDCURRENT_DEFAULT, MAX30100_LEDCURRENT_DEFAULT);
        MAX30100_SetMode(MAX30100_SPO2_MODE);
        MAX30100_InterruptHandler();
    }
}
