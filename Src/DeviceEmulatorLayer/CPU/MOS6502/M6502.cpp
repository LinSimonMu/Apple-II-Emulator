#include "M6502.h"
namespace DeviceEmulatorLayer
{
namespace CPU
{
namespace MOS6502
{

M6502::M6502()
{

}

M6502::~M6502()
{

}

/* Function Name:       Reset
 * Func Description:    When boot on,some registers are initialized
 * Paramters:           void
 * Return:              void
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
    PC = 0xFFFC;        //PC is point to Vector RESET
    SP = 0x01FF;        //Stack Point = 0x1FF at the top of the stack
    P = 0x00;           //N V U B D I Z C = 0
    A = 0;
    X = 0;
    Y = 0;

    mem.init();
}

void M6502::setFlag(StatusRegFlag flag, bool value)
{
    if (value)
    {
        P |= flag;
    }
    else
    {
        P &= ~flag;
    }
}

Memory::Memory()
{

}

Memory::~Memory()
{
}

void Memory::Init(void)
{
    for (int i = 0; i < MAX_MEM_SIZE; i++)
    {
        Mem[i] = 0;
    }
}

}
}
}
