#pragma once

namespace smartwatch::sensor
{
    class HeartRateSensor
    {
        public:

        void init()
        {
            return doInit();
        }

        private:

        virtual void doInit() = 0;
    };
}
