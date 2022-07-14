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

        virtual bool check_status() = 0;

        virtual void detect_pulse() = 0;
    };
}
