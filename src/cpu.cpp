#include<cstdint>
#include<SDL3/SDL.h>
#include<glad/glad.h>
#include<fstream>

/*
REQUIREMENTS
Sixteen 8-bit registers (V0-VF)
16-bit index register 
64-byte stack with 8-bit stack pointer 
4096k of memory
8-bit sound timer 
64x32 bit frame buffer 
16-bit program counter
35 opcodes
16-key keyboard with 0-9 and A-F
*/

const int START_ADDRESS = 0x200;

class Chip8{
    public:
    uint8_t registers[8]{};
    uint8_t memory[4096]{};
    uint16_t ir{};
    uint16_t pc{};
    uint16_t stack[16]{};
    uint8_t sp{};
    uint8_t  delayTimer{};
    uint8_t soundTimer{};
    uint16_t inputKeys[16]{};
    uint32_t screen[64 * 32]{};
    uint16_t opcode{};

    //Clear display 
    void Chip8::OP_00E0(){
        std::memset(screen, 0, sizeof(screen));
    }

    //00EE - RET
    void Chip8::OP_00EE(){
        pc = stack[sp];
        sp-=1;
    }

    //1nnn - Jump addr
    void Chip8::OP_1nnn(){
        uint16_t addr = opcode & 0x0FFFu;
        pc = addr;
    }

    //3xkk - SE Vx, byte
    void Chip8::OP_3xkk(){
        
    }

};
