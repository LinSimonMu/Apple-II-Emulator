/*
 * Name:                    M6502.c
 * Time:                    2021-7-16
 * Corporation:             MOVE
 * Website:                 Coming soon...
 * Bilibili Homepage:       https://space.bilibili.com/204219698  MOVEÄ§ÎÝ
 * TikTok(DouYin) ID:       689999795  MOVE
 * Declaration:
 */

#include "M6502.h"

namespace DeviceEmulatorLayer
{
namespace CPU
{
namespace MOS6502
{

M6502::M6502()
{
    PC = new Word;
    SP = new Word;
    A  = new Byte; 
    X  = new Byte;
    Y  = new Byte;
    P  = new StatusReg;
}

M6502::~M6502()
{
    delete PC;
    delete SP;
    delete A;
    delete X;
    delete Y;
    delete P; 
}

M6502& M6502::operator=(const M6502& m6502)
{
    if (this == &m6502)
    {
        return *this;
    }
    delete PC;
    delete SP;
    delete A;
    delete X;
    delete Y;
    delete P;

    PC = new Word;
    SP = new Word;
    A = new Byte;
    X = new Byte;
    Y = new Byte;
    P = new StatusReg;

    *PC = *(m6502.PC);
    *SP = *(m6502.SP);
    *A = *(m6502.A);
    *X = *(m6502.X);
    *Y = *(m6502.Y);
    *P = *(m6502.P);
    mem = m6502.mem;
}

/* Function Name:       reset
 * Func Description:    When boot on,some registers are initialized
 * Paramters:           [void]
 * Return:              [void]
 * Usage: 
 * Appendix:            https://www.pagetable.com/?p=410#:~:text=Cycle%200%3A%20When%20a%206502,register%20(IR)%20to%200.&text=Cycle%203%3A%20The%20first%20stack,%2B%20SP%2C%20then%20decrements%20SP.  
 *                      http://wiki.nesdev.com/w/index.php/CPU_power_up_state
 *                      https://www.c64-wiki.com/wiki/Reset_(Process)#Hard_Reset
 *                      Signal      Vector	        Push PC and P	  Set B Flag
 *                      RESET       $FFFC/$FFFD     no	              no
 *                      ; MOS 6510 System Reset routine[3]
                        ; Reset vector (Kernal address $FFFC) points here.
                        ; 
                        ; If cartridge is detected then cartridge cold start routine is activated.
                        ; If no cartridge is detected then I/O and memory are initialised and BASIC cold start routine is activated.

                        FCE2   A2 FF      LDX #$FF        ; 
                        FCE4   78         SEI             ; set interrupt disable
                        FCE5   9A         TXS             ; transfer .X to stack
                        FCE6   D8         CLD             ; clear decimal flag
                        FCE7   20 02 FD   JSR $FD02       ; check for cart
                        FCEA   D0 03      BNE $FCEF       ; .Z=0? then no cart detected
                        FCEC   6C 00 80   JMP ($8000)     ; direct to cartridge cold start via vector
                        FCEF   8E 16 D0   STX $D016       ; sets bit 5 (MCM) off, bit 3 (38 cols) off
                        FCF2   20 A3 FD   JSR $FDA3       ; initialise I/O
                        FCF5   20 50 FD   JSR $FD50       ; initialise memory
                        FCF8   20 15 FD   JSR $FD15       ; set I/O vectors ($0314..$0333) to kernal defaults
                        FCFB   20 5B FF   JSR $FF5B       ; more initialising... mostly set system IRQ to correct value and start
                        FCFE   58         CLI             ; clear interrupt flag
                        FCFF   6C 00 A0   JMP ($A000)     ; direct to BASIC cold start via vector
 */
void M6502::reset()
{
    AssignReg(PC, 0xFFFC);        //PC is point to Vector RESET
    AssignReg(SP, 0x01FF);        //Stack Point = 0x1FF at the top of the stack
    AssignReg(P, 0b00000000);     //N V U B D I Z C = 0
    AssignReg(A, 0);
    AssignReg(X, 0);
    AssignReg(Y, 0);

    mem.init();
}


 /* Function Name:       Addressing
  * Func Description:    Get Address by addressing mode
  * Paramters:           [Cycles] cycles: the cycles that get the address need to take
  * Return:              [MemAddr] the address
  * Usage:
  * Appendix:            http://www.obelisk.me.uk/6502/addressing.html
  */
MemAddr M6502::Addressing(Cycles& cycles, Addr_Mode mode)
{
    Byte dataLow = 0;
    Byte dataHigh = 0;
    Word data16;
    MemAddr addr = 0x0000;
    switch (mode)
    {
    case Addr_Mode_RegA:   //Accumulator

        break;
    case Addr_Mode_IM:     //Immediate
        addr = AddressConstraint((MemAddr)GetReg(PC)); //The immediate value followed by the instruction
        AssignReg(PC, GetReg(PC) + 1);                 //Increase the PC value.
        break;
    case Addr_Mode_ZP:     //Zero Page
        dataLow = fetch(cycles);
        addr = (MemAddr)dataLow;
        break;
    case Addr_Mode_ZP_RegX:      //Zero Page,X
        dataLow = fetch(cycles);            //fetch the zero page address. 1 cycle
        dataLow += GetReg(X); cycles--;     //offset the address by X register. 1 cycle
        addr = (MemAddr)dataLow;
        break;
    case Addr_Mode_ZP_RegY:                 //Zero Page,Y
        dataLow = fetch(cycles);            //fetch the zero page address. 1 cycle
        dataLow += GetReg(Y); cycles--;     //offset the address by X register. 1 cycle
        addr = (MemAddr)dataLow;
        break;
        break;
    case Addr_Mode_REL:          //Relative

        break;
    case Addr_Mode_ABS:                     //Absolute
        dataLow = fetch(cycles);            //fetch the low address. 1 cycle
        dataHigh = fetch(cycles);           //fetch the high address. 1 cycle
        data16 = dataLow;
        data16 |= (dataHigh << 8);          //merge the address
        addr = (MemAddr)data16;
        break;
    case Addr_Mode_ABS_RegX:                //Absolute,X
        dataLow = fetch(cycles);            //fetch the low address. 1 cycle
        dataHigh = fetch(cycles);           //fetch the high address. 1 cycle
        data16 = dataLow;
        data16 |= (dataHigh << 8);          //merge the address.
        data16 += GetRegX();                //offset the adress by X register.  
        if (GetRegX() >= 0xFF) cycles--;   //Cross page, take 1 cycle.
        addr = (MemAddr)data16;
        break;
    case Addr_Mode_ABS_RegY:                //Absolute,Y
        dataLow = fetch(cycles);            //fetch the low address. 1 cycle
        dataHigh = fetch(cycles);           //fetch the high address. 1 cycle
        data16 = dataLow;
        data16 |= (dataHigh << 8);          //merge the address.
        data16 += GetRegY();                //offset the adress by Y register.
        if (GetRegY() >= 0xFF) cycles--;   //Cross page, take 1 cycle.
        addr = (MemAddr)data16;
        break;
    case Addr_Mode_IND:          //Indirect
        break;
    case Addr_Mode_IND_RegX:                            //Indexed X Indirect
        dataLow = fetch(cycles);                        //fetch the zero page address. 1 cycle
        data16 = (MemAddr)dataLow + (MemAddr)GetRegX(); cycles--; //offset the address by resgister X,low address. 1 cycle
        dataLow = seek(cycles, (MemAddr)(data16));      //Seek the low byte as low address. 1 cycle
        data16 += 1;                                    //high address
        dataHigh = seek(cycles, (MemAddr)(data16));     //Seek the high byte as high address. 1 cycle
        data16 = dataLow;
        data16 |= (dataHigh << 8);                      //merge the address.
        addr = (MemAddr)data16;
        break;
    case Addr_Mode_IND_RegY:     //Indirect Indexed Y
        dataLow = fetch(cycles);                        //fetch the zero page address. 1 cycle
        data16 = (MemAddr)dataLow; //offset the address by resgister X,low address. 1 cycle
        dataLow = seek(cycles, (MemAddr)(data16));      //Seek the low byte as low address. 1 cycle
        data16 += 1;                                    //high address
        dataHigh = seek(cycles, (MemAddr)(data16));     //Seek the high byte as high address. 1 cycle
        data16 = dataLow;
        data16 |= (dataHigh << 8);                      //merge the address.
        data16 += GetRegY();                            //offset the address by register Y.
        if (GetRegY() >= 0xFF) cycles--;                //Cross page, take 1 cycle.
        addr = (MemAddr)data16;
        break;

    }

    return addr;

}

/* Function Name:       exec
 * Func Description:    Execute the instructions from PC
 * Paramters:           [Cycles] cycles: the cycles that instructions need to take
 * Return:              [Cycles] the actual cycles that exec used
 * Usage:
 * Appendix:            http://www.obelisk.me.uk/6502/instructions.html
 */

 /*
  * Load/Store Operations
  * These instructions transfer a single byte between memory and one of the registers.
  * Load operations set the negative (N) and zero (Z) flags depending on the value of transferred.
  * Store operations do not affect the flag settings.
  * | [LDA] | Load Accumulator  | [N],[Z] |
  * | [LDX] | Load X Register   | [N],[Z] |
  * | [LDY] | Load Y Register   | [N],[Z] |
  * | [STA] | Store Accumulator |         |
  * | [STX] | Store X Register  |         |
  * | [STY] | Store Y Register  |         |
  *
  */

/* *************************************** [LDA] * *************************************
    *| Addressing Mode      | Opcode     | Bytes   | Cycles                 |
    *| -------------------- | -----------| --------| -----------------------|
    *| [Immediate]          | $A9        | 2       | 2                      |
    *| [Zero Page]          | $A5        | 2       | 3                      |
    *| [Zero Page, X]       | $B5        | 2       | 4                      |
    *| [Absolute]           | $AD        | 3       | 4                      |
    *| [Absolute, X]        | $BD        | 3       | 4 (+1 if page crossed) |
    *| [Absolute, Y]        | $B9        | 3       | 4 (+1 if page crossed) |
    *| [(Indirect, X)]      | $A1        | 2       | 6                      |
    *| [(Indirect), Y]      | $B1        | 2       | 5 (+1 if page crossed) |
    *
    *******************************************************************************************
    *
    */

/*
* ### Jumps & Calls
* The following instructions modify the program counter causing a break to normal sequential execution.
* The [JSR] instruction pushes the old [PC]onto the stack before changing it to the new location
* allowing a subsequent [RTS] to return execution to the instruction after the call.
* | [JMP] | Jump to another location |      |
* | [JSR] | Jump to a subroutine     |      |
* | [RTS] | Return from subroutine   |      |
* ************************************** [JSR] **************************************
* The JSR instruction pushes the address (minus one) of the return point on to the stack and
* then sets the program counter to the target memory address.
* Processor Status after use:

* |  Addressing Mode |  Opcode  |   Bytes   |   Cycles   |
* | -----------------| ---------| --------- | ---------- |
* | [Absolute]       | $20      | 3         | 6          |
*/
Cycles M6502::exec(Cycles cycles)
{
    Cycles totalCycles = cycles;
    Cycles usingCycles = 0;
    while (cycles > 0)
    {
        Byte dataByte = 0;
        MemAddr addr = 0x0000;
        Ins_Set_Type insSetType = Ins_Set_Type_Unknow;
        Addr_Mode addrMode = Addr_Mode_IM;
        LdSt_Reg ldstReg = LdSt_Reg_A;
        LdStOpt_Type ldstOpt = LdStOpt_Load;

        Instructions ins = (Instructions)fetch(cycles);  //fetch instruction. 1 cycle

        switch (ins)
        {
            //LDA
            {
            case INS_LDA_IM:                                //2 cycles
                insSetType = Ins_Set_Type_LdStOpt;
                ldstOpt = LdStOpt_Load;
                ldstReg = LdSt_Reg_A;
                addrMode = Addr_Mode_IM;
                break;
            case INS_LDA_ZP:                                //3 cycles
                insSetType = Ins_Set_Type_LdStOpt;
                ldstOpt = LdStOpt_Load;
                ldstReg = LdSt_Reg_A;
                addrMode = Addr_Mode_ZP;
                break;
            case INS_LDA_ZP_X:                              //4 cycles
                insSetType = Ins_Set_Type_LdStOpt;
                ldstOpt = LdStOpt_Load;
                ldstReg = LdSt_Reg_A;
                addrMode = Addr_Mode_ZP_RegX;
                break;
            case INS_LDA_ABS:                               //4 cycles
                insSetType = Ins_Set_Type_LdStOpt;
                ldstOpt = LdStOpt_Load;
                ldstReg = LdSt_Reg_A;
                addrMode = Addr_Mode_ABS;
                break;
            case INS_LDA_ABS_X:                             //4 cycles (+1 if page crossed)
                insSetType = Ins_Set_Type_LdStOpt;
                ldstOpt = LdStOpt_Load;
                ldstReg = LdSt_Reg_A;
                addrMode = Addr_Mode_ABS_RegX;
                break;
            case INS_LDA_ABS_Y:                                 //4 cycles (+1 if page crossed)
                insSetType = Ins_Set_Type_LdStOpt;
                ldstOpt = LdStOpt_Load;
                ldstReg = LdSt_Reg_A;
                addrMode = Addr_Mode_ABS_RegY;
                break;
            case INS_LDA_IND_X:                                 //6 cycles, indrect way  means that the address is in mem
                insSetType = Ins_Set_Type_LdStOpt;
                ldstOpt = LdStOpt_Load;
                ldstReg = LdSt_Reg_A;
                addrMode = Addr_Mode_IND_RegX;
                break;
            case INS_LDA_IND_Y:                                 //5 cycles (+1 if page crossed), indrect way  means that the address is in mem
                insSetType = Ins_Set_Type_LdStOpt;
                ldstOpt = LdStOpt_Load;
                ldstReg = LdSt_Reg_A;
                addrMode = Addr_Mode_IND_RegY;
                break;
            }
            //LDX
            {
                case INS_LDX_IM:
                    insSetType = Ins_Set_Type_LdStOpt;
                    ldstOpt = LdStOpt_Load;
                    ldstReg = LdSt_Reg_X;
                    addrMode = Addr_Mode_IM;
                    break;
                case INS_LDX_ZP:
                    insSetType = Ins_Set_Type_LdStOpt;
                    ldstOpt = LdStOpt_Load;
                    ldstReg = LdSt_Reg_X;
                    addrMode = Addr_Mode_ZP;
                    break;
                case INS_LDX_ZP_Y:
                    insSetType = Ins_Set_Type_LdStOpt;
                    ldstOpt = LdStOpt_Load;
                    ldstReg = LdSt_Reg_X;
                    addrMode = Addr_Mode_ZP_RegY;
                    break;
                case INS_LDX_ABS:
                    insSetType = Ins_Set_Type_LdStOpt;
                    ldstOpt = LdStOpt_Load;
                    ldstReg = LdSt_Reg_X;
                    addrMode = Addr_Mode_ABS;
                    break;
                case INS_LDX_ABS_Y:
                    insSetType = Ins_Set_Type_LdStOpt;
                    ldstOpt = LdStOpt_Load;
                    ldstReg = LdSt_Reg_X;
                    addrMode = Addr_Mode_ABS_RegY;
                    break;
            }
            //LDY
            {
                case INS_LDY_IM:
                    insSetType = Ins_Set_Type_LdStOpt;
                    ldstOpt = LdStOpt_Load;
                    ldstReg = LdSt_Reg_Y;
                    addrMode = Addr_Mode_IM;
                    break;
                case INS_LDY_ZP:
                    insSetType = Ins_Set_Type_LdStOpt;
                    ldstOpt = LdStOpt_Load;
                    ldstReg = LdSt_Reg_Y;
                    addrMode = Addr_Mode_ZP;
                    break;
                case INS_LDY_ZP_X:
                    insSetType = Ins_Set_Type_LdStOpt;
                    ldstOpt = LdStOpt_Load;
                    ldstReg = LdSt_Reg_Y;
                    addrMode = Addr_Mode_ZP_RegX;
                    break;
                case INS_LDY_ABS:
                    insSetType = Ins_Set_Type_LdStOpt;
                    ldstOpt = LdStOpt_Load;
                    ldstReg = LdSt_Reg_Y;
                    addrMode = Addr_Mode_ABS;
                    break;
                case INS_LDY_ABS_X:
                    insSetType = Ins_Set_Type_LdStOpt;
                    ldstOpt = LdStOpt_Load;
                    ldstReg = LdSt_Reg_Y;
                    addrMode = Addr_Mode_ABS_RegX;
                    break;
            }

        case INS_JSR:     //6 cycles
            //6502 is little endian low address low byte,high address high byte
            addr = Addressing(cycles, Addr_Mode_ABS);

            //save the original PC value to the satck,[preserve the scene]
            write(cycles, GetReg(SP), GetReg(PC) & 0xFF);   //write PC low byte to stack. 1 cycle
            AssignReg(SP, GetReg(SP) - 1);                  //SP = SP - 1
            write(cycles, GetReg(SP), GetReg(PC) >> 8);     //write PC high byte to stack. 1 cycle
            AssignReg(SP, GetReg(SP) - 1);                  //SP = SP - 1
            AssignReg(PC, addr);                          //PC point to the Sub function address
            cycles--;                                       //1 cycle
            break;
        default:
            insSetType = Ins_Set_Type_Unknow;
            
            break;
        }

        switch (insSetType)
        {
        case Ins_Set_Type_LdStOpt:                      //Load/Store Operations, get address and then load or store
            addr = Addressing(cycles, addrMode);        //Get address by addressing mode
            dataByte = seek(cycles, addr);              //seek the data. 1 cycle
            switch (ldstOpt)
            {
            case LdStOpt_Load:
                Load2Reg(ldstReg, dataByte);
                break;
            case LdStOpt_Store:
                Store2Mem(ldstReg, dataByte);
                break;
            default:
                break;
            }
            
            break;
        case Ins_Set_Type_Unknow:
            printf("INS NOT HANDLE : %d\r\n", ins);
            break;
        default:
            break;
        }

        UpdateStatusReg(ins);
    }
    usingCycles = totalCycles - cycles;
    return usingCycles;
}

void M6502::writeMem(MemAddr addr, Byte data)
{
    mem[addr] = data;
}

Byte M6502::readMem(MemAddr addr)
{
    return mem[addr];
}

Byte M6502::GetRegA(void)
{
    return GetReg(A);
}

Byte M6502::GetRegX(void)
{
    return GetReg(X);
}

Byte M6502::GetRegY(void)
{
    return GetReg(Y);
}

Byte M6502::GetRegP_N(void)
{
    return GetPRegFlag(P, N);
}

Byte M6502::GetRegP_V(void)
{
    return GetPRegFlag(P, V);
}

Byte M6502::GetRegP_B(void)
{
    return GetPRegFlag(P, B);
}

Byte M6502::GetRegP_D(void)
{
    return GetPRegFlag(P, D);
}

Byte M6502::GetRegP_I(void)
{
    return GetPRegFlag(P, I);
}

Byte M6502::GetRegP_Z(void)
{
    return GetPRegFlag(P, Z);
}

Byte M6502::GetRegP_C(void)
{
    return GetPRegFlag(P, C);
}


Word M6502::GetRegPC(void)
{
    return GetReg(PC);
}

Word M6502::GetRegSP(void)
{
    return GetReg(SP);
}
#ifdef CPU_TEST
void M6502::WriteRegA(Byte regValue)
{
    AssignReg(A, regValue);
}

void M6502::WriteRegX(Byte regValue)
{
    AssignReg(X, regValue);
}

void M6502::WriteRegY(Byte regValue)
{
    AssignReg(Y, regValue);
}

void M6502::WriteRegP(Byte regValue)
{
}
 
void M6502::WriteRegPC(Word regValue)
{
    AssignReg(PC, regValue);
}

void M6502::WriteRegSP(Word regValue)
{
    AssignReg(SP, regValue);
}

#endif

/* Function Name:       fetch
 * Func Description:    fetch the instructions from mem by PC,then PC++,this operation can change the PC
 * Paramters:           [Cycles]  cycles    :   the cycles that fetch action need to take
 * Return:              [Byte  ]  memData   :   the data in mem find by PC
 * Usage:
 */
Byte M6502::fetch(Cycles& cycles)
{
    Byte memData = mem[AddressConstraint((MemAddr)GetReg(PC))];
    AssignReg(PC,GetReg(PC) + 1);
    cycles--;
    return memData;
}

/* Function Name:       seek
 * Func Description:    seek data from mem by the address given,this operation doesn't change the PC
 * Paramters:           [Cycles ]  cycles   :   the cycles that seek action need to take
 *                      [MemAddr]  addr     :   the address to seek
 * Return:              [Byte   ]  memData  :   the data in memory find by address given
 * Usage:
 */
Byte M6502::seek(Cycles& cycles, MemAddr addr)
{
    Byte memData = mem[AddressConstraint(addr)];
    cycles--;
    return memData;
}

/* Function Name:       write
 * Func Description:    write data to mem by the address given,this operation doesn't change the PC
 * Paramters:           [Cycles ]  cycles       : the cycles that seek action need to take
 *                      [MemAddr]  addr         : the memory address write to
 *                      [Byte   ]  dataWrite    : the data need to write
 * Return:              [void   ]
 * Usage:
 */
void M6502::write(Cycles& cycles, MemAddr addr, Byte dataWrite)
{
    mem[AddressConstraint(addr)] = dataWrite;
    cycles--;
}

/* Function Name:       AddressConstraint
 * Func Description:    Constraint address within 0x0000-0xFFFF
 * Paramters:           [MemAddr] addr: the memory address ready to  read
 * Return:              [MemAddr] the actual memory address
 * Usage:
 */
MemAddr M6502::AddressConstraint(MemAddr addr)
{
    if (addr > 0xFFFF)
        addr = 0xFFFF;
    return addr;
}

void M6502::Load2Reg(LdSt_Reg reg, Byte data)
{
    switch (reg)
    {
    case LdSt_Reg_A:
        AssignReg(A, data);
        break;
    case LdSt_Reg_X:
        AssignReg(X, data);
        break;
    case LdSt_Reg_Y:
        AssignReg(Y, data);
        break;
    default:
        break;
    }
}

void M6502::Store2Mem(LdSt_Reg reg, Byte data)
{

}

/*  *************************************** [LDA] * *************************************
    *Loads a byte of memory into the accumulator setting the zeroand negative flags as appropriate.
    * C	Carry Flag	            Not affected
    * Z	Zero Flag	            Set if A = 0
    * I	Interrupt Disable	    Not affected
    * D	Decimal Mode Flag	    Not affected
    * B	Break Command	        Not affected
    * V	Overflow Flag	        Not affected
    * N	Negative Flag	        Set if bit 7 of A is set
    * 
    * 
    *
    * ************************************* [JSR] **************************************
    * The JSR instruction pushes the address (minus one) of the return point on to the stack and
    * then sets the program counter to the target memory address.
    * Processor Status after use:
    * | [C] | [Carry Flag]            | Not affected |
    * |-------------------------------|--------------|
    * | [Z] | [Zero Flag]             | Not affected |
    * | [I] | [Interrupt Disable]     | Not affected |
    * | [D] | [Decimal Mode Flag]     | Not affected |
    * | [B] | [Break Command]         | Not affected |
    * | [V] | [Overflow Flag]         | Not affected |
    * | [N] | [Negative Flag]         | Not affected |
 */


 /* Function Name:       UpdateStatusReg
  * Func Description:    when do some instruction,the status register may change.
  * Paramters:           [Instructions] ins: the instruction executed.
  * Return:              [void]
  * Usage:
  */
void M6502::UpdateStatusReg(Instructions ins)
{
    switch (ins)
    {
    case INS_LDA_IM:
    case INS_LDA_ZP:
    case INS_LDA_ZP_X:
    case INS_LDA_ABS:
    case INS_LDA_ABS_X:
    case INS_LDA_ABS_Y:
    case INS_LDA_IND_X:
    case INS_LDA_IND_Y:
        AssignPRegFlag(P, Z, (GetReg(A) == 0));
        AssignPRegFlag(P, N, ((GetReg(A) & 0b10000000) > 0));
        break;

    case INS_LDX_IM:
    case INS_LDX_ZP:
    case INS_LDX_ZP_Y:
    case INS_LDX_ABS:
    case INS_LDX_ABS_Y:
        AssignPRegFlag(P, Z, (GetReg(X) == 0));
        AssignPRegFlag(P, N, ((GetReg(X) & 0b10000000) > 0));
        break;

    case INS_LDY_IM:
    case INS_LDY_ZP:
    case INS_LDY_ZP_X:
    case INS_LDY_ABS:
    case INS_LDY_ABS_X:
        AssignPRegFlag(P, Z, (GetReg(Y) == 0));
        AssignPRegFlag(P, N, ((GetReg(Y) & 0b10000000) > 0));
        break;
    case INS_JSR:
        break;
    }
}

Memory::Memory()
{

}

Memory::~Memory()
{
}

void Memory::init(void)
{
    for (int i = 0; i < MAX_MEM_SIZE; i++)
    {
        Mem[i] = 0;
    }
}

Byte Memory::operator[](MemAddr addr) const
{
    return Mem[addr];
}

Byte& Memory::operator[](MemAddr addr)
{
    return Mem[addr];
}

Memory& Memory::operator=(const Memory& memory)
{
    for (int i = 0; i < MAX_MEM_SIZE; i++)
    {
        Mem[i] = memory.Mem[i];
    }
    return *this;
}

Byte StatusReg::operator=(Byte p)
{
    N = (p >> 7)&(0x01);
    V = (p >> 6)&(0x01);
    U = (p >> 5)&(0x01);
    B = (p >> 4)&(0x01);
    D = (p >> 3)&(0x01);
    I = (p >> 2)&(0x01);
    Z = (p >> 1)&(0x01);
    C = (p >> 0)&(0x01);
    return p;
}

}
}
}
