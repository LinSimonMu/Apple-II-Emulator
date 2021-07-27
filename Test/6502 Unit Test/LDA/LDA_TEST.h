#ifndef _LDA_TEST_H_
#define _LDA_TEST_H_

#include <cstdio>
#include "gtest/gtest.h"
#include "../Src/DeviceEmulatorLayer/CPU/MOS6502/M6502.h"
using namespace DeviceEmulatorLayer::CPU;
void VerfityUnmodifiedFlagsFromLDA(MOS6502::M6502& CPU, MOS6502::M6502& CPU_Pre);

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
/*Bad Ins*/
TEST_F(M6502_TEST_INS, M6502_TEST_INS_BAD)
{
    CPU.writeMem(0xFFFC, 0x00);
    CPU.writeMem(0xFFFD, 0x00);

    Cycles cycles = CPU.exec(1);

    EXPECT_EQ(cycles, 1);
}

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

    VerfityUnmodifiedFlagsFromLDA(CPU, CPU_Pre);
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

    VerfityUnmodifiedFlagsFromLDA(CPU, CPU_Pre);
}

/*INS_LDA_ZP_X*/
TEST_F(M6502_TEST_INS, M6502_TEST_INS_LDA_ZP_X)
{
    CPU.WriteRegX(5);
    CPU.writeMem(0xFFFC, INS_LDA_ZP_X);
    CPU.writeMem(0xFFFD, 0x42);
    CPU.writeMem(0x0047, 0x37);

    MOS6502::M6502 CPU_Pre;
    CPU_Pre = CPU;

    Cycles cycles = CPU.exec(4);

    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(CPU.GetRegA(), 0x37);

    EXPECT_FALSE(CPU.GetRegP_Z());
    EXPECT_FALSE(CPU.GetRegP_N());
    VerfityUnmodifiedFlagsFromLDA(CPU, CPU_Pre);
}

/*INS_LDA_ABS*/
TEST_F(M6502_TEST_INS, M6502_TEST_INS_LDA_ABS)
{
    CPU.writeMem(0xFFFC, INS_LDA_ABS);
    CPU.writeMem(0xFFFD, 0x80);
    CPU.writeMem(0xFFFE, 0x44);
    CPU.writeMem(0x4480, 0x37); //[0x4480] = 0x37

    MOS6502::M6502 CPU_Pre;
    CPU_Pre = CPU;

    Cycles cycles = CPU.exec(4);

    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(CPU.GetRegA(), 0x37);

    EXPECT_FALSE(CPU.GetRegP_Z());
    EXPECT_FALSE(CPU.GetRegP_N());
    VerfityUnmodifiedFlagsFromLDA(CPU, CPU_Pre);
}

/*INS_LDA_ABS_X*/
TEST_F(M6502_TEST_INS, M6502_TEST_INS_LDA_ABS_X)
{
    //CPU.WriteRegX(5);
    CPU.WriteRegX(0xFF);
    CPU.writeMem(0xFFFC, INS_LDA_ABS_X);
    CPU.writeMem(0xFFFD, 0x80);
    CPU.writeMem(0xFFFE, 0x44);
    CPU.writeMem(0x4480, 0x36); //[0x4480] = 0x36
    //CPU.writeMem(0x4485, 0x37); //[0x4485] = 0x37
    CPU.writeMem(0x457F, 0x37);

    MOS6502::M6502 CPU_Pre;
    CPU_Pre = CPU;

    //Cycles cycles = CPU.exec(4);
    Cycles cycles = CPU.exec(5);

    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(CPU.GetRegA(), 0x37);

    EXPECT_FALSE(CPU.GetRegP_Z());
    EXPECT_FALSE(CPU.GetRegP_N());
    VerfityUnmodifiedFlagsFromLDA(CPU, CPU_Pre);
}

/*INS_LDA_ABS_Y*/
TEST_F(M6502_TEST_INS, M6502_TEST_INS_LDA_ABS_Y)
{
    CPU.WriteRegY(4);
    CPU.writeMem(0xFFFC, INS_LDA_ABS_Y);
    CPU.writeMem(0xFFFD, 0x80);
    CPU.writeMem(0xFFFE, 0x44);
    CPU.writeMem(0x4484, 0x37); //[0x4484] = 0x37

    MOS6502::M6502 CPU_Pre;
    CPU_Pre = CPU;

    Cycles cycles = CPU.exec(4);

    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(CPU.GetRegA(), 0x37);

    EXPECT_FALSE(CPU.GetRegP_Z());
    EXPECT_FALSE(CPU.GetRegP_N());
    VerfityUnmodifiedFlagsFromLDA(CPU, CPU_Pre);
}

/*INS_LDA_IND_X*/
TEST_F(M6502_TEST_INS, M6502_TEST_INS_LDA_IND_X)
{
    CPU.WriteRegX(4);
    CPU.writeMem(0xFFFC, INS_LDA_IND_X);
    CPU.writeMem(0xFFFD, 0x02);
    CPU.writeMem(0x0006, 0x00);
    CPU.writeMem(0x0007, 0x80);
    CPU.writeMem(0x8000, 0x37); //[0x8000] = 0x37

    MOS6502::M6502 CPU_Pre;
    CPU_Pre = CPU;

    Cycles cycles = CPU.exec(6);

    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(CPU.GetRegA(), 0x37);

    EXPECT_FALSE(CPU.GetRegP_Z());
    EXPECT_FALSE(CPU.GetRegP_N());
    VerfityUnmodifiedFlagsFromLDA(CPU, CPU_Pre);
}

/*INS_LDA_IND_Y*/
TEST_F(M6502_TEST_INS, M6502_TEST_INS_LDA_IND_Y)
{
    CPU.WriteRegY(4);
    CPU.writeMem(0xFFFC, INS_LDA_IND_Y);
    CPU.writeMem(0xFFFD, 0x02);
    CPU.writeMem(0x0002, 0x00);
    CPU.writeMem(0x0003, 0x80);
    CPU.writeMem(0x8004, 0x37); //[0x8004] = 0x37

    MOS6502::M6502 CPU_Pre;
    CPU_Pre = CPU;

    Cycles cycles = CPU.exec(5);

    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(CPU.GetRegA(), 0x37);

    EXPECT_FALSE(CPU.GetRegP_Z());
    EXPECT_FALSE(CPU.GetRegP_N());
    VerfityUnmodifiedFlagsFromLDA(CPU, CPU_Pre);
}

/* */

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

void VerfityUnmodifiedFlagsFromLDA(MOS6502::M6502 &CPU, MOS6502::M6502 &CPU_Pre)
{
    EXPECT_EQ(CPU.GetRegP_C(), CPU_Pre.GetRegP_C());
    EXPECT_EQ(CPU.GetRegP_I(), CPU_Pre.GetRegP_I());
    EXPECT_EQ(CPU.GetRegP_D(), CPU_Pre.GetRegP_D());
    EXPECT_EQ(CPU.GetRegP_B(), CPU_Pre.GetRegP_B());
    EXPECT_EQ(CPU.GetRegP_V(), CPU_Pre.GetRegP_V());
}
