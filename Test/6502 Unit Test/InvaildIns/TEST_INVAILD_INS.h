#ifndef _INVAILD_INS_TEST_H_
#define _INVAILD_INS_TEST_H_

#include <cstdio>
#include "gtest/gtest.h"
#include "../Src/DeviceEmulatorLayer/CPU/MOS6502/M6502.h"
#include "../TEST_ENTRY.h"
using namespace DeviceEmulatorLayer::CPU;

/*Bad Ins*/
TEST_F(M6502_TEST_INS, M6502_TEST_INS_BAD)
{
    CPU.writeMem(0xFFFC, 0x00);
    CPU.writeMem(0xFFFD, 0x00);

    Cycles cycles = CPU.exec(1);

    EXPECT_EQ(cycles, 1);
}

#endif
