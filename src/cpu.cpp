#include<cstdint>
#include<SDL3/SDL.h>
#include<glad/glad.h>
#include<fstream>

/*
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
    uint16_t ir{}; //index register
    uint16_t pc{}; //program counter 
    uint16_t stack[16]{};
    uint8_t sp{};  //stack pointer
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

    //2nnn - Call addr
    void Chip8::OP_2nnn(){
        uint16_t addr = opcode & 0x0FFU;
        stack[sp] = pc;
        ++sp;
        pc = addr;
    }

    //3xkk - SE Vx, byte
    void Chip8::OP_3xkk(){
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t kk = (opcode & 0x00FFu);

        if(registers[Vx] == kk){
            pc+=2;
        }
    }

    //4xkk - SNE Vx, byte 
    void Chip8::OP_4xkk(){
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t kk = (opcode &0x00FFu);

        if(registers[Vx] != kk){
            pc+=2;
        }
    }

    //5xy0 - SE Vx, Vy
    void Chip8::OP_5xy0(){
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t Vy = (opcode & 0x00F0u) >> 4u;

        if(registers[Vx] == registers[Vy]){
            pc+=2;
        }
    }

    //6xkk - LD Vx, byte
    void Chip8::OP_6xkk(){
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t kk = (opcode & 0x00FFu);

        registers[Vx] = kk;
    }

    //7xkk - ADD Vx, byte 
    void Chip8::OP_7xkk(){
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t kk = (opcode & 0x00FFu);

        registers[Vx] = registers[Vx] + kk;
    }

    //8xy0 - LD Vx, Vy
    void Chip8::OP_8xy0(){
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t Vy = (opcode & 0x00F0u) >> 4u;

        registers[Vx] = registers[Vy];
    }

    //8xy1 - OR Vx, Vy
    void Chip8::OP_8xy1(){
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t Vy = (opcode & 0x00F0u) >> 4u;

        registers[Vx] = registers[Vx] | registers[Vy];
    }

    //8xy2 - AND Vx, Vy
    void Chip8::OP_8xy2(){
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t Vy = (opcode & 0x00F0u) >> 4u;

        registers[Vx] = registers[Vx] & registers[Vy];
    }

    //8xy3 - XOR Vx, Vy
    void Chip8::OP_8xy3(){
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t Vy = (opcode & 0x00F0u) >> 4u;

        registers[Vx] = registers[Vx] ^ registers[Vy];
    }

    //8xy4 - ADD Vx, Vy 
    void Chip8::OP_8xy4(){
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t Vy = (opcode & 0x00F0u) >> 4u;

        uint16_t sum = registers[Vx] + registers[Vy];

        if (sum > 255u){
            registers[0xF] = 1;
        }else{
            registers[0xF] = 0;
        }

        registers[Vx] = sum;

    }

    //8xy5 - SUB Vx, Vy
    void Chip8::OP_8xy5(){
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t Vy = (opcode & 0x00F0u) >> 4u;

        if (registers[Vx] > registers[Vy]){
            registers[0xF] = 1;
        }else{
            registers[0xF] = 0;
        }

        registers[Vx] = registers[Vx] - registers[Vy];
    }

    //8xy6 - SHR Vx {,Vy}
    void Chip8::OP_8xy6(){
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        registers[0xF] = (registers[Vx] & 0x1u);
        registers[Vx] = registers[Vx] >> 1;
    }

    //8xy7 - SUB Vx, Vy
    void Chip8::OP_8xy5(){
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t Vy = (opcode & 0x00F0u) >> 4u;

        if (registers[Vy] > registers[Vx]){
            registers[0xF] = 1;
        }else{
            registers[0xF] = 0;
        }

        registers[Vx] = registers[Vy] - registers[Vx];
    }

    //8xyE - SHL Vx, {, Vy}
    void Chip8::OP_8xyE(){
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        registers[0xF] = (registers[Vx] & 0x1u);
        registers[Vx] = registers[Vx] << 1;
    }

    //9xy0 - SNE Vx, Vy
    void Chip8::OP_9xy0(){
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;
        uint8_t Vy = (opcode & 0x00F0u) >> 4u;

        if (registers[Vx] != registers[Vy]){
            pc += 2;
        }
    }

    //Annn - LD I, addr
    void Chip8::OP_Annn(){
        uint16_t addr = (opcode & 0x0FFFu);
        ir = addr;
    } 

    //Bnnn - JP V0, addr
    void Chip8::OP_Bnnn(){
       uint16_t addr = (opcode & 0x0FFFu);
       pc = addr + registers[0x0]; 
    }

    //Cxkk - RND Vx, byte
    void Chip8::OP_Cxkk(){

    }

    //Dxyn - DRW Vx, Vy, nibble
    void Chip8::OP_Dxyn(){

    }

    



};
