#include "bsp.hpp"
#include "max30100.hpp"
#include "sensor_interface.hpp"

#include <cstdio>

using namespace std;

int main()
{
    using namespace ::smartwatch;
    using namespace std::literals::chrono_literals;
    bsp::init();

    bsp::write("Smartwatch App v1.0\r\n");

    /* Initialize and configure MAX30100 heart rate sensor */
    smartwatch::sensor::MAX30100 heart_rate_sensor;

    while(true)
    {
        /* Check sensor state - when IR data buffer is full, run pulse detection algorithm */
        if(heart_rate_sensor.check_status())
        {
            heart_rate_sensor.detect_pulse();
        }
    }

}
