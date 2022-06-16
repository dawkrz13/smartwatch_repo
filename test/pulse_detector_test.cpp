#include <iostream>
#include <fstream>
#include <string>
#include <stdint.h>

extern "C"
{
#include "pulse_detector.h"
}

using namespace std;

int main(void)
{
    PULSE_DETECTOR pd = {0};

    fstream file;
    file.open("ir_raw.txt", ios::in);
    if(file.is_open())
    {
        string ir_buffer;
        uint16_t ir;
        while(getline(file, ir_buffer))
        {
            ir = stoi(ir_buffer);
            measure_heart_rate(&pd, ir);
            if(pd.state == STATE_READY)
            {
                cout << pd.bpm << endl;
            }
        }
        file.close();
    }

    return 0;
}
