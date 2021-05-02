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

/* Function Name:       exec
 * Func Description:    Execute the instructions from PC
 * Paramters:           [Cycles] cycles: the cycles that instructions need to take
 * Return:              [void] 
 * Usage:
 * Appendix:            http://www.obelisk.me.uk/6502/instructions.html
 */

void M6502::exec(Cycles cycles)
{
    while (cycles > 0)
    {
        Byte ins = fetch(cycles);  //fetch instruction. 1 cycle
        Byte dataLow = 0;
        Byte dataHigh = 0;
        Word data16;
        switch (ins)
        {
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
    *Loads a byte of memory into the accumulator setting the zeroand negative flags as appropriate.
    * C	Carry Flag	            Not affected
    * Z	Zero Flag	            Set if A = 0
    * I	Interrupt Disable	    Not affected
    * D	Decimal Mode Flag	    Not affected
    * B	Break Command	        Not affected
    * V	Overflow Flag	        Not affected
    * N	Negative Flag	        Set if bit 7 of A is set
    *
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
        case INS_LDA_IM:                    //2cycles
            dataLow = fetch(cycles);        //fetch memory data read by PC. 1 cycle
            AssignReg(A, dataLow);          //transfer the data to A register
            AssignSRegFlagWhenDoLDAIns();   //change the P's flag N and Z
            break;
        case INS_LDA_ZP:                        //3 cycles
            dataLow = fetch(cycles);            //fetch the zero page address. 1 cycle
            dataLow = seek(cycles, dataLow);    //seek the data by the zero page address. 1 cycle
            AssignReg(A, dataLow);
            AssignSRegFlagWhenDoLDAIns();       //change the P's flag N and Z
            break;
        case INS_LDA_ZPX:                       //4 cycles
            dataLow = fetch(cycles);            //fetch the zero page address. 1 cycle
            dataLow += GetReg(X); cycles--;     //offset the address by X register. 1 cycle
            dataLow = seek(cycles, dataLow);    //seek the data by the zero page address. 1 cycle
            AssignReg(A, dataLow);
            AssignSRegFlagWhenDoLDAIns();       //change the P's flag N and Z
            break;
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
    * | [C] | [Carry Flag]            | Not affected |
    * |-------------------------------|--------------|
    * | [Z] | [Zero Flag]             | Not affected |
    * | [I] | [Interrupt Disable]     | Not affected |
    * | [D] | [Decimal Mode Flag]     | Not affected |
    * | [B] | [Break Command]         | Not affected |
    * | [V] | [Overflow Flag]         | Not affected |
    * | [N] | [Negative Flag]         | Not affected |
    * 
    * |  Addressing Mode |  Opcode  |   Bytes   |   Cycles   |
    * | -----------------| ---------| --------- | ---------- |
    * | [Absolute]       | $20      | 3         | 6          |
    */
        case INS_JSR:     //6 cycles
            //6502 is little endian low address low byte,high address high byte
            dataLow = fetch(cycles);   //fecth the low address. 1 cycle
            dataHigh = fetch(cycles);  //fetch the high address. 1 cycle
            data16 = dataLow;
            data16 |= (dataHigh << 8); //get the sub function addr

            //save the original PC value to the satck,[preserve the scene]
            write(cycles, GetReg(SP), GetReg(PC) & 0xFF);   //write PC low byte to stack. 1 cycle
            AssignReg(SP, GetReg(SP) - 1);                  //SP = SP - 1
            write(cycles, GetReg(SP), GetReg(PC) >> 8);     //write PC high byte to stack. 1 cycle
            AssignReg(SP, GetReg(SP) - 1);                  //SP = SP - 1
            AssignReg(PC, data16);                          //PC point to the Sub function address
            cycles--;                                       //1 cycle
        default:
            break;
        }
#ifdef Debug
        
#endif
    }
}

void M6502::writeMem(MemAddr addr, Byte data)
{
    mem[addr] = data;
}

Byte M6502::readMem(MemAddr addr)
{
    return mem[addr];
}

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

/* Function Name:       AssignSRegFlagWhenDoLDAIns
 * Func Description:    when do the operation LDA,the status register [P] N flag and Z flag is change
 * Paramters:           [void]
 * Return:              [void]
 * Usage:
 */
void M6502::AssignSRegFlagWhenDoLDAIns(void)
{
    AssignSRegFlag(P, Z, (GetReg(A) == 0));
    AssignSRegFlag(P, N, ((GetReg(A) & 0b10000000) > 0));
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
