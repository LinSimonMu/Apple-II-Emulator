#ifndef _LDY_TEST_H_
#define _LDY_TEST_H_

#include <cstdio>
#include "gtest/gtest.h"
#include "../Src/DeviceEmulatorLayer/CPU/MOS6502/M6502.h"
#include "../TEST_ENTRY.h"
using namespace DeviceEmulatorLayer::CPU;
void VerfityUnmodifiedFlagsFromLDY(MOS6502::M6502& CPU, MOS6502::M6502& CPU_Pre);

/*M6502 LDY INSTRUCTION Test*/

/*INS_LDX_IM*/
TEST_F(M6502_TEST_INS, M6502_TEST_INS_LDY_IM)
{
    CPU.writeMem(0xFFFC, MOS6502::INS_LDY_IM);
    CPU.writeMem(0xFFFD, 0x84);

    MOS6502::M6502 CPU_Pre;
    CPU_Pre = CPU;
    Cycles cycles = CPU.exec(2);

    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(CPU.GetRegY(), 0x84);
    EXPECT_FALSE(CPU.GetRegP_Z());
    EXPECT_TRUE(CPU.GetRegP_N());

    VerfityUnmodifiedFlagsFromLDY(CPU, CPU_Pre);
}

/*INS_LDY_ZP*/
TEST_F(M6502_TEST_INS, M6502_TEST_INS_LDY_ZP)
{
    CPU.writeMem(0xFFFC, MOS6502::INS_LDY_ZP);
    CPU.writeMem(0xFFFD, 0x42);
    CPU.writeMem(0x0042, 0x37);

    MOS6502::M6502 CPU_Pre;
    CPU_Pre = CPU;

    Cycles cycles = CPU.exec(3);

    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(CPU.GetRegY(), 0x37);
    EXPECT_FALSE(CPU.GetRegP_Z());
    EXPECT_FALSE(CPU.GetRegP_N());

    VerfityUnmodifiedFlagsFromLDY(CPU, CPU_Pre);
}

/*INS_LDY_ZP_X*/
TEST_F(M6502_TEST_INS, M6502_TEST_INS_LDY_ZP_X)
{
    CPU.WriteRegX(5);
    CPU.writeMem(0xFFFC, MOS6502::INS_LDY_ZP_X);
    CPU.writeMem(0xFFFD, 0x42);
    CPU.writeMem(0x0047, 0x37);

    MOS6502::M6502 CPU_Pre;
    CPU_Pre = CPU;

    Cycles cycles = CPU.exec(4);

    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(CPU.GetRegY(), 0x37);

    EXPECT_FALSE(CPU.GetRegP_Z());
    EXPECT_FALSE(CPU.GetRegP_N());
    VerfityUnmodifiedFlagsFromLDY(CPU, CPU_Pre);
}

/*INS_LDY_ABS*/
TEST_F(M6502_TEST_INS, M6502_TEST_INS_LDY_ABS)
{
    CPU.writeMem(0xFFFC, MOS6502::INS_LDY_ABS);
    CPU.writeMem(0xFFFD, 0x80);
    CPU.writeMem(0xFFFE, 0x44);
    CPU.writeMem(0x4480, 0x37); //[0x4480] = 0x37

    MOS6502::M6502 CPU_Pre;
    CPU_Pre = CPU;

    Cycles cycles = CPU.exec(4);

    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(CPU.GetRegY(), 0x37);

    EXPECT_FALSE(CPU.GetRegP_Z());
    EXPECT_FALSE(CPU.GetRegP_N());
    VerfityUnmodifiedFlagsFromLDY(CPU, CPU_Pre);
}

/*INS_LDY_ABS_X*/
TEST_F(M6502_TEST_INS, M6502_TEST_INS_LDY_ABS_X)
{
    //CPU.WriteRegX(5);
    CPU.WriteRegX(0xFF);
    CPU.writeMem(0xFFFC, MOS6502::INS_LDY_ABS_X);
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
    EXPECT_EQ(CPU.GetRegY(), 0x37);

    EXPECT_FALSE(CPU.GetRegP_Z());
    EXPECT_FALSE(CPU.GetRegP_N());
    VerfityUnmodifiedFlagsFromLDY(CPU, CPU_Pre);
}


void VerfityUnmodifiedFlagsFromLDY(MOS6502::M6502& CPU, MOS6502::M6502& CPU_Pre)
{
    EXPECT_EQ(CPU.GetRegP_C(), CPU_Pre.GetRegP_C());
    EXPECT_EQ(CPU.GetRegP_I(), CPU_Pre.GetRegP_I());
    EXPECT_EQ(CPU.GetRegP_D(), CPU_Pre.GetRegP_D());
    EXPECT_EQ(CPU.GetRegP_B(), CPU_Pre.GetRegP_B());
    EXPECT_EQ(CPU.GetRegP_V(), CPU_Pre.GetRegP_V());
}

#endif
