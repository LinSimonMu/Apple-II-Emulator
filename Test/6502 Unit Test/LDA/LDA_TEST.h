#ifndef _LDA_TEST_H_
#define _LDA_TEST_H_

#include <cstdio>
#include "gtest/gtest.h"
#include "../Src/DeviceEmulatorLayer/CPU/MOS6502/M6502.h"
using namespace DeviceEmulatorLayer::CPU;

#define CPU_TEST

/*M6502 LDA INSTRUCTION Test*/

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

/*INS_LDA_IM*/
TEST_F(M6502_TEST_INS, M6502_TEST_INS_LDA_IM)
{
    CPU.writeMem(0xFFFC, INS_LDA_IM);
    CPU.writeMem(0xFFFD, 0x84);

    MOS6502::M6502 CPU_Pre;
    CPU_Pre = CPU;
    Cycles cycles = CPU.exec(2);

    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(CPU.GetRegA(), 0x84);
    EXPECT_FALSE(CPU.GetRegP_Z());
    EXPECT_TRUE(CPU.GetRegP_N());

    EXPECT_EQ(CPU.GetRegP_C(), CPU_Pre.GetRegP_C());
    EXPECT_EQ(CPU.GetRegP_I(), CPU_Pre.GetRegP_I());
    EXPECT_EQ(CPU.GetRegP_D(), CPU_Pre.GetRegP_D());
    EXPECT_EQ(CPU.GetRegP_B(), CPU_Pre.GetRegP_B());
    EXPECT_EQ(CPU.GetRegP_V(), CPU_Pre.GetRegP_V());
}

/*INS_LDA_ZP*/
TEST_F(M6502_TEST_INS, M6502_TEST_INS_LDA_ZP)
{
    CPU.writeMem(0xFFFC, INS_LDA_ZP);
    CPU.writeMem(0xFFFD, 0x42);
    CPU.writeMem(0x0042, 0x37);

    MOS6502::M6502 CPU_Pre;
    CPU_Pre = CPU;

    Cycles cycles = CPU.exec(3);

    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(CPU.GetRegA(), 0x37);
    EXPECT_FALSE(CPU.GetRegP_Z());
    EXPECT_FALSE(CPU.GetRegP_N());

    EXPECT_EQ(CPU.GetRegP_C(), CPU_Pre.GetRegP_C());
    EXPECT_EQ(CPU.GetRegP_I(), CPU_Pre.GetRegP_I());
    EXPECT_EQ(CPU.GetRegP_D(), CPU_Pre.GetRegP_D());
    EXPECT_EQ(CPU.GetRegP_B(), CPU_Pre.GetRegP_B());
    EXPECT_EQ(CPU.GetRegP_V(), CPU_Pre.GetRegP_V());
}

/*INS_LDA_ZPX*/
TEST_F(M6502_TEST_INS, M6502_TEST_INS_LDA_ZPX)
{
    CPU.WriteRegX(5);
    CPU.writeMem(0xFFFC, INS_LDA_ZPX);
    CPU.writeMem(0xFFFD, 0x42);
    CPU.writeMem(0x0047, 0x37);

    MOS6502::M6502 CPU_Pre;
    CPU_Pre = CPU;

    Cycles cycles = CPU.exec(4);

    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(CPU.GetRegA(), 0x37);

    EXPECT_FALSE(CPU.GetRegP_Z());
    EXPECT_FALSE(CPU.GetRegP_N());

    EXPECT_EQ(CPU.GetRegP_C(), CPU_Pre.GetRegP_C());
    EXPECT_EQ(CPU.GetRegP_I(), CPU_Pre.GetRegP_I());
    EXPECT_EQ(CPU.GetRegP_D(), CPU_Pre.GetRegP_D());
    EXPECT_EQ(CPU.GetRegP_B(), CPU_Pre.GetRegP_B());
    EXPECT_EQ(CPU.GetRegP_V(), CPU_Pre.GetRegP_V());
}

#if 0
class M6502Test1 : public testing::Test
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


TEST_F(M6502Test1, test)
{
    CPU.writeMem(0xFFFC, INS_JSR);
    CPU.writeMem(0xFFFD, 0x42);
    CPU.writeMem(0xFFFE, 0x42);
    CPU.writeMem(0x4242, INS_LDA_IM);
    CPU.writeMem(0x4243, 0x84);
    CPU.exec(9);

    EXPECT_EQ(CPU.GetRegA(), 0x84);
}
#endif

#endif
