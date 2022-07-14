extern "C"
{
#include "max30100_for_stm32_hal.h"
#include "main.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_i2c.h"
}

#include "bsp.hpp"
#include "max30100.hpp"

extern "C" UART_HandleTypeDef huart2;
extern "C" I2C_HandleTypeDef hi2c2;

namespace smartwatch::sensor
{
    void MAX30100::doInit()
    {
        /* Setup MAX30100 */
        MAX30100_Init(&hi2c2, &huart2);
        MAX30100_SetSpO2SampleRate(MAX30100_SPO2SR_DEFAULT);
        MAX30100_SetLEDPulseWidth(MAX30100_LEDPW_DEFAULT);
        MAX30100_SetLEDCurrent(MAX30100_LEDCURRENT_DEFAULT, MAX30100_LEDCURRENT_DEFAULT);
        MAX30100_SetMode(MAX30100_HRONLY_MODE);
        MAX30100_InterruptHandler();
    }

    bool MAX30100::check_status()
    {
        /* Return true if IR data buffer is almost full */
        return (pd.idx > 450);
    }

    void MAX30100::detect_pulse()
    {
        run_pulse_detector(&huart2);
    }
}
