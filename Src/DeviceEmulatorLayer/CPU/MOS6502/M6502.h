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

#define CPU_TEST

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
#define AssignPRegFlag(P,Flag,v)    (P->Flag) = v
#define GetPRegFlag(P,Flag)         (P->Flag)
/* MOS 6502 CPU
 * https://en.wikipedia.org/wiki/MOS_Technology_6502
 * https://baike.baidu.com/item/MOS%206502/7603374
 */
namespace MOS6502
{
enum Instructions
{
    INS_LDA_IM      = 0xA9,
    INS_LDA_ZP      = 0xA5,
    INS_LDA_ZP_X    = 0xB5,
    INS_LDA_ABS     = 0xAD,
    INS_LDA_ABS_X   = 0xBD,
    INS_LDA_ABS_Y   = 0xB9,
    INS_LDA_IND_X   = 0xA1,
    INS_LDA_IND_Y   = 0xB1,

    INS_LDX_IM      = 0xA2,
    INS_LDX_ZP      = 0xA6,
    INS_LDX_ZP_Y    = 0xB6,
    INS_LDX_ABS     = 0xAE,
    INS_LDX_ABS_Y   = 0xBE,

    INS_LDY_IM      = 0xA0,
    INS_LDY_ZP      = 0xA4,
    INS_LDY_ZP_X    = 0xB4,
    INS_LDY_ABS     = 0xAC,
    INS_LDY_ABS_X   = 0xBC,

    INS_JSR         = 0x20,
};


enum LdSt_Reg
{
    LdSt_Reg_A = 0,
    LdSt_Reg_X,
    LdSt_Reg_Y,
};

enum Addr_Mode
{
    Addr_Mode_RegA = 0,     //Accumulator
    Addr_Mode_IM,           //Immediate
    Addr_Mode_ZP,           //Zero Page
    Addr_Mode_ZP_RegX,      //Zero Page,X
    Addr_Mode_ZP_RegY,      //Zero Page,Y
    Addr_Mode_REL,          //Relative
    Addr_Mode_ABS,          //Absolute
    Addr_Mode_ABS_RegX,     //Absolute,X
    Addr_Mode_ABS_RegY,     //Absolute,Y
    Addr_Mode_IND,          //Indirect
    Addr_Mode_IND_RegX,     //Indexed X Indirect
    Addr_Mode_IND_RegY,     //Indirect Indexed Y
};

enum Ins_Set_Type
{
    Ins_Set_Type_Unknow = 0,    //Unknow
    Ins_Set_Type_LdStOpt,       //Load/Store Operations
    Ins_Set_Type_RegTrans,      //Register Transfers
    Ins_Set_Type_StackOpt,      //Stack Operations
    Ins_Set_Type_Logical,       //Logical
    Ins_Set_Type_Arithmetic,    //Arithmetic
    Ins_Set_Type_IncDec,        //Increments& Decrements
    Ins_Set_Type_Shifts,        //Shifts
    Ins_Set_Type_JmpCall,       //Jumps& Calls
    Ins_Set_Type_Branches,      //Branches
    Ins_Set_Type_SFChange,      //Status Flag Changes
    Ins_Set_Type_SysFunc,       //System Functions
};

enum LdStOpt_Type
{
    LdStOpt_Load = 0,
    LdStOpt_Store,
};

class Memory
{
#define MAX_MEM_SIZE    64 * 1024       //64K Memory Map
public:
    Memory();
    ~Memory();
    void init(void);
    Byte operator[](MemAddr addr) const;
    Byte& operator[](MemAddr addr);
    Memory& operator=(const Memory& memory);
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
    M6502& operator=(const M6502& m6502);
public:
    void reset(void);
    Cycles exec(Cycles cycles);
    void writeMem(MemAddr addr, Byte data);
    Byte readMem(MemAddr addr);
    Byte GetRegA(void);
    Byte GetRegX(void);
    Byte GetRegY(void);
    Byte GetRegP_N(void);
    Byte GetRegP_V(void);
    Byte GetRegP_B(void);
    Byte GetRegP_D(void);
    Byte GetRegP_I(void);
    Byte GetRegP_Z(void);
    Byte GetRegP_C(void);
    Word GetRegPC(void);
    Word GetRegSP(void);
#ifdef CPU_TEST
    void WriteRegA(Byte regValue);
    void WriteRegX(Byte regValue);
    void WriteRegY(Byte regValue);
    void WriteRegP(Byte regValue);
    void WriteRegPC(Word regValue);
    void WriteRegSP(Word regValue);
#endif
private:
    MemAddr Addressing(Cycles& cycles, Addr_Mode mode);
    Byte fetch(Cycles& cycles);
    Byte seek(Cycles& cycles,MemAddr addr);
    void write(Cycles& cycles, MemAddr addr,Byte dataWrite);
    MemAddr AddressConstraint(MemAddr addr);
    void Load2Reg(LdSt_Reg reg, Byte data);
    void Store2Mem(LdSt_Reg reg, Byte data);
    void UpdateStatusReg(Instructions ins);
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