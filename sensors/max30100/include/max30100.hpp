#pragma once

#include "sensor_interface.hpp"

namespace smartwatch::sensor
{
    class MAX30100 : public HeartRateSensor
    {
        public:

        MAX30100()
        {
            init();
        }

        void init()
        {
            return doInit();
        }
        void doInit() override;

        void check_status();
    };
}
