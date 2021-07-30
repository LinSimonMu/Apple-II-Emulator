#ifndef _TEST_ENTRY_H_
#define _TEST_ENTRY_H_

#include <cstdio>
#include "gtest/gtest.h"
#include "../Src/DeviceEmulatorLayer/CPU/MOS6502/M6502.h"

using namespace DeviceEmulatorLayer::CPU;

class M6502_TEST_INS: public testing::Test
{
public:
    MOS6502::M6502 CPU;

    virtual void SetUp()
    {
        CPU.reset();
    }

    virtual void TearDown()
    {

    }
};

#endif
