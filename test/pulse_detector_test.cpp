#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>
#include <fstream>
#include <string>
#include <stdint.h>
#include <vector>

extern "C"
{
#include "pulse_detector.h"
}

using namespace std;
using ::testing::AllOf;
using ::testing::Gt;
using ::testing::Le;

static uint32_t count_average(const vector<uint32_t> pd_output)
{
    uint32_t sum_of_elems;

    for(auto& n : pd_output)
        sum_of_elems += n;

    return sum_of_elems/pd_output.size();
}

TEST(PulseDetector, common)
{
    PULSE_DETECTOR pd = {0};
    vector<uint32_t> pd_output;
    uint32_t ref_bpm = 60; // reference average bpm as calculated by model

    fstream file;
    file.open("data/ir_raw.txt", ios::in);
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
                pd_output.push_back(pd.bpm);
            }
        }
        file.close();
    }

    uint32_t average_bpm = count_average(pd_output);

    ASSERT_THAT(average_bpm, AllOf(Gt(ref_bpm-2), Le(ref_bpm+2))) << "Upss...";
}
