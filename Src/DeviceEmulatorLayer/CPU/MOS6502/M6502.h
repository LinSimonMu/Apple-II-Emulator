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

/*DeviceEmulatorLayer*/
namespace DeviceEmulatorLayer
{
namespace CPU
{

typedef unsigned char   Byte;  //8  bits
typedef unsigned short  Word;  //16 bits

/* MOS 6502 CPU
 * https://en.wikipedia.org/wiki/MOS_Technology_6502
 * https://baike.baidu.com/item/MOS%206502/7603374
 */
namespace MOS6502
{
enum StatusRegFlag
{
    N = (1 << 7),  //Negative Flag      [0:Positive, 1:Negative]
    V = (1 << 6),  //Overflow Flag      [0:False,    1:True    ]
    U = (1 << 5),  //Unused
    B = (1 << 4),  //Break Command Flag [0:No break, 1:Break   ]
    D = (1 << 3),  //Decimal Mode Falg  [0:False,    1:True    ]
    I = (1 << 2),  //IRQ Disable Flag   [0:Enable,   1:Disable ]
    Z = (1 << 1),  //Zero Flag          [0:Result Not Zero, 1:Result Zero]
    C = (1 << 0),  //Carry Flag         [0:False,    1:True    ]
};
class M6502
{
public:
    M6502();
    ~M6502();
public:
    void reset(void);
    void Exec(void);
private:
    void setFlag(StatusRegFlag flag, bool value);
private:
    /*Registers*/
    Word PC;  //Program Count 
    Word SP;  //Stack pointer
    Byte A;   //Acclumlator   
    Byte X;   //Index Register X
    Byte Y;   //Index Register Y
    Byte P;   //Program Status Register

    Memory mem;

};

class Memory
{
#define MAX_MEM_SIZE    64 * 1024       //64K Memory Map
public:
    Memory();
    ~Memory();
    void Init(void);
private:
    Byte Mem[MAX_MEM_SIZE];
};

}
}
}



#endif