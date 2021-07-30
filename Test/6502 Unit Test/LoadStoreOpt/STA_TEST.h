#ifndef _STA_TEST_H_
#define _STA_TEST_H_

#include <cstdio>
#include "gtest/gtest.h"
#include "../Src/DeviceEmulatorLayer/CPU/MOS6502/M6502.h"
#include "../TEST_ENTRY.h"
using namespace DeviceEmulatorLayer::CPU;
void VerfityUnmodifiedFlagsFromSTA(MOS6502::M6502& CPU, MOS6502::M6502& CPU_Pre);

/*M6502 LDA INSTRUCTION Test*/

/*INS_STA_ZP*/
TEST_F(M6502_TEST_INS, M6502_TEST_INS_STA_ZP)
{
    CPU.WriteRegA(0x2F);
    CPU.writeMem(0x0080, 0x00);

    CPU.writeMem(0xFFFC, MOS6502::INS_STA_ZP);
    CPU.writeMem(0xFFFD, 0x80);
    

    MOS6502::M6502 CPU_Pre;
    CPU_Pre = CPU;

    Cycles cycles = CPU.exec(3);

    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(CPU.readMem(0x0080), 0x2F);


    VerfityUnmodifiedFlagsFromSTA(CPU, CPU_Pre);
}

/*INS_STA_ZP_X*/
TEST_F(M6502_TEST_INS, M6502_TEST_INS_STA_ZP_X)
{
    CPU.WriteRegA(0x2F);
    CPU.WriteRegX(0x05);
    CPU.writeMem(0x0085, 0x00);

    CPU.writeMem(0xFFFC, MOS6502::INS_STA_ZP_X);
    CPU.writeMem(0xFFFD, 0x80);

    MOS6502::M6502 CPU_Pre;
    CPU_Pre = CPU;

    Cycles cycles = CPU.exec(4);

    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(CPU.readMem(0x0085), 0x2F);

    VerfityUnmodifiedFlagsFromSTA(CPU, CPU_Pre);
}

/*INS_STA_ABS*/
TEST_F(M6502_TEST_INS, M6502_TEST_INS_STA_ABS)
{
    CPU.WriteRegA(0x2F);
    CPU.writeMem(0x0080, 0x00);

    CPU.writeMem(0xFFFC, MOS6502::INS_STA_ABS);
    CPU.writeMem(0xFFFD, 0x80);
    CPU.writeMem(0xFFFE, 0x00);

    MOS6502::M6502 CPU_Pre;
    CPU_Pre = CPU;

    Cycles cycles = CPU.exec(4);

    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(CPU.readMem(0x0080), 0x2F);

    VerfityUnmodifiedFlagsFromSTA(CPU, CPU_Pre);
}

/*INS_STA_ABS_X*/
TEST_F(M6502_TEST_INS, M6502_TEST_INS_STA_ABS_X)
{
    CPU.WriteRegA(0x2F);
    CPU.WriteRegX(0x05);
    CPU.writeMem(0x0085, 0x00);

    CPU.writeMem(0xFFFC, MOS6502::INS_STA_ABS_X);
    CPU.writeMem(0xFFFD, 0x80);
    CPU.writeMem(0xFFFE, 0x00);

    MOS6502::M6502 CPU_Pre;
    CPU_Pre = CPU;

    Cycles cycles = CPU.exec(5);

    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(CPU.readMem(0x0085), 0x2F);

    VerfityUnmodifiedFlagsFromSTA(CPU, CPU_Pre);
}

/*INS_STA_ABS_Y*/
TEST_F(M6502_TEST_INS, M6502_TEST_INS_STA_ABS_Y)
{
    CPU.WriteRegA(0x2F);
    CPU.WriteRegY(0x05);
    CPU.writeMem(0x0085, 0x00);

    CPU.writeMem(0xFFFC, MOS6502::INS_STA_ABS_Y);
    CPU.writeMem(0xFFFD, 0x80);
    CPU.writeMem(0xFFFE, 0x00);

    MOS6502::M6502 CPU_Pre;
    CPU_Pre = CPU;

    Cycles cycles = CPU.exec(5);

    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(CPU.readMem(0x0085), 0x2F);

    VerfityUnmodifiedFlagsFromSTA(CPU, CPU_Pre);
}

/*INS_STA_IND_X*/
TEST_F(M6502_TEST_INS, M6502_TEST_INS_STA_IND_X)
{
    CPU.WriteRegA(0x2F);
    CPU.WriteRegX(4);
    CPU.writeMem(0x0080, 0x00);
    
    CPU.writeMem(0xFFFC, MOS6502::INS_STA_IND_X);
    CPU.writeMem(0xFFFD, 0x02);
    CPU.writeMem(0x0006, 0x80);
    CPU.writeMem(0x0007, 0x00);

    MOS6502::M6502 CPU_Pre;
    CPU_Pre = CPU;

    Cycles cycles = CPU.exec(6);

    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(CPU.readMem(0x0080), 0x2F);

    VerfityUnmodifiedFlagsFromSTA(CPU, CPU_Pre);
}

/*INS_STA_IND_Y*/
TEST_F(M6502_TEST_INS, M6502_TEST_INS_STA_IND_Y)
{
    CPU.WriteRegA(0x2F);
    CPU.WriteRegY(5);
    CPU.writeMem(0x0085, 0x00);

    CPU.writeMem(0xFFFC, MOS6502::INS_STA_IND_Y);

    CPU.writeMem(0xFFFD, 0x02);
    CPU.writeMem(0x0002, 0x80);
    CPU.writeMem(0x0003, 0x00);

    MOS6502::M6502 CPU_Pre;
    CPU_Pre = CPU;

    Cycles cycles = CPU.exec(6);

    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(CPU.readMem(0x0085), 0x2F);

    VerfityUnmodifiedFlagsFromSTA(CPU, CPU_Pre);
}


void VerfityUnmodifiedFlagsFromSTA(MOS6502::M6502 &CPU, MOS6502::M6502 &CPU_Pre)
{
    EXPECT_EQ(CPU.GetRegP_C(), CPU_Pre.GetRegP_C());
    EXPECT_EQ(CPU.GetRegP_Z(), CPU_Pre.GetRegP_Z());
    EXPECT_EQ(CPU.GetRegP_I(), CPU_Pre.GetRegP_I());
    EXPECT_EQ(CPU.GetRegP_D(), CPU_Pre.GetRegP_D());
    EXPECT_EQ(CPU.GetRegP_B(), CPU_Pre.GetRegP_B());
    EXPECT_EQ(CPU.GetRegP_V(), CPU_Pre.GetRegP_V());
    EXPECT_EQ(CPU.GetRegP_N(), CPU_Pre.GetRegP_N());
}

#endif
