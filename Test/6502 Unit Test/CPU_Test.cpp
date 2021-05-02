#include "./../../Src/DeviceEmulatorLayer/CPU/MOS6502/M6502.h"
using namespace DeviceEmulatorLayer::CPU;
int main(void)
{
    MOS6502::M6502 CPU;
    CPU.reset();
    CPU.writeMem(0xFFFC, INS_JSR);
    CPU.writeMem(0xFFFD, 0x42);
    CPU.writeMem(0xFFFE, 0x42);
    CPU.writeMem(0x4242, INS_LDA_IM);
    CPU.writeMem(0x4243, 0x84);
    CPU.exec(9);
    return 0;
}