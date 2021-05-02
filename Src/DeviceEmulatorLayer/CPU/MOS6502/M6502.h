/*
 * Name:                    M6502.h
 * Time:                    2021-4-25
 * Corporation:             MOVE
 * Website:                 Coming soon...
 * Bilibili Homepage:       https://space.bilibili.com/204219698  MOVEÄ§ÎÝ
 * TikTok(DouYin) ID:       689999795  MOVE
 * Declaration:             
 */

#ifndef _M6502_H_
#define _M6502_H_

#define Debug
#ifdef Debug
#include <stdio.h>
using namespace std;
#endif

/*DeviceEmulatorLayer*/
namespace DeviceEmulatorLayer
{
namespace CPU
{

typedef unsigned char   Byte;  //8  bits
typedef unsigned short  Word;  //16 bits
typedef Byte*           Reg8;
typedef Word*           Reg16;
typedef unsigned short  MemAddr;
typedef unsigned int    Cycles;
#define AssignReg(reg,v)            (*reg) = v
#define GetReg(reg)                 (*reg)
#define AssignSRegFlag(P,Flag,v)    (P->Flag) = v
/* MOS 6502 CPU
 * https://en.wikipedia.org/wiki/MOS_Technology_6502
 * https://baike.baidu.com/item/MOS%206502/7603374
 */
namespace MOS6502
{
#define INS_LDA_IM       0xA9
#define INS_LDA_ZP       0xA5
#define INS_LDA_ZPX      0xB5

#define INS_JSR          0x20

class Memory
{
#define MAX_MEM_SIZE    64 * 1024       //64K Memory Map
public:
    Memory();
    ~Memory();
    void init(void);
    Byte operator[](MemAddr addr) const;
    Byte& operator[](MemAddr addr);
private:
    Byte Mem[MAX_MEM_SIZE];

};

struct StatusReg
{    
    Byte C : 1;  //Carry Flag         [0:False,    1:True    ]
    Byte Z : 1;  //Zero Flag          [0:Result Not Zero, 1:Result Zero]
    Byte I : 1;  //IRQ Disable Flag   [0:Enable,   1:Disable ]
    Byte D : 1;  //Decimal Mode Falg  [0:False,    1:True    ]
    Byte B : 1;  //Break Command Flag [0:No break, 1:Break   ]
    Byte U : 1;  //Unused
    Byte V : 1;  //Overflow Flag      [0:False,    1:True    ]
    Byte N : 1;  //Negative Flag      [0:Positive, 1:Negative]
    Byte operator=(Byte p);
};
typedef StatusReg* SReg8;
class M6502
{
public:
    M6502();
    ~M6502();
public:
    void reset(void);
    void exec(Cycles cycles);
    void writeMem(MemAddr addr, Byte data);
    Byte readMem(MemAddr addr);
private:
    Byte fetch(Cycles& cycles);
    Byte seek(Cycles& cycles,MemAddr addr);
    void write(Cycles& cycles, MemAddr addr,Byte dataWrite);
    MemAddr AddressConstraint(MemAddr addr);
    void AssignSRegFlagWhenDoLDAIns(void);
private:
    /*Registers*/
    Reg16 PC;  //Program Count 
    Reg16 SP;  //Stack pointer
    Reg8  A;   //Acclumlator   
    Reg8  X;   //Index Register X
    Reg8  Y;   //Index Register Y
    SReg8 P;   //Program Status Register

    Memory mem;

};

}
}
}



#endif