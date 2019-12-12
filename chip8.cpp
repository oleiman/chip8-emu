#include "include/chip8.hpp"

#include <ctime>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

#define CODE_START 0x200
#define CODE_END 0xEA0

using std::array;
using std::ifstream;
using std::make_unique;
using std::stack;
using std::string;
using std::stringstream;

// TODO(oren): consider endianness as a source of weird output

namespace chip8 {

unsigned char chip8_fontset[80] =
{ 
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};
    
Chip8::Chip8():
    pc_(CODE_START)
    , delay_timer_(0xF)
    , sound_timer_(0xF)
    , kp_await_(false)
    , draw_flag_(false)
{
    for (int i = 0; i < 80; i++) {
        memory_[i] = chip8_fontset[i];
    }

    srand(time(0));
}

bool Chip8::loadApp(const string& app)
{
    ifstream infile(app, std::ios::binary);
    if (!infile) {
        return false;
    }
        
    infile.read(reinterpret_cast<char*>(memory_.data()) + CODE_START, CODE_END - CODE_START);
    if (!infile) {
        auto bytes_read = infile.gcount();
        infile.clear();
        infile.seekg(0);
        infile.read(reinterpret_cast<char*>(memory_.data()) + CODE_START, bytes_read);
    }
        
    return infile ? true : false;
}

void Chip8::cycle()
{
    if (!kp_await_) {
    
        // fetch opcode
        uint16_t opcode = fetch_opcode();
        // std::cout << "0x" << std::hex << opcode << std::endl;

        decode_and_exec(opcode);
    }

    // TODO(oren): do timers
    if (sound_timer_ > 0) std::cout << '\a';
}

void Chip8::setKey(uint8_t key, bool state)
{
    if (key <= 0xF) {
        keys_[key] = state;
        if (kp_await_ && key == kp_which_) {
            kp_await_ = false;
        }
    }
}

void Chip8::exportScreenBuf(uint8_t dest[][SCREEN_WIDTH][3])
{
    for (int y = 0; y < SCREEN_HEIGHT; y++ ) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            if (screen_buffer_[y][x] == 0) {
                dest[y][x][0] = dest[y][x][1] = dest[y][x][2] = 0;
            } else {
                dest[y][x][0] = dest[y][x][1] = dest[y][x][2] = 255;
            }
        }
    }
}

uint16_t Chip8::fetch_opcode()
{
    return ((memory_[pc_] << 8) | memory_[pc_+1]);
}

void Chip8::decode_and_exec(uint16_t opcode)
{
    // first, have a peek at the top nibble
    uint16_t instr = opcode & 0xF000;

    uint8_t VX  = (opcode & 0x0F00) >> 8;
    uint8_t VY  = (opcode & 0x00F0) >> 4;
    uint8_t NNN = opcode & 0x0FFF;
    uint8_t NN  = opcode & 0x00FF;
    uint8_t N   = opcode & 0x000F;

    switch(instr) {
    case 0x0000:
        if (opcode == 0x00E0) {
            clear_screen();
        } else if (opcode == 0x00EE) {
            subroutine_return();
        } else {
            // TODO(oren): RCA 1802 program?
        }
        break;
    case 0x2000:
        stack_.push(pc_);
    case 0x1000:
        pc_ = NNN;
        pc_ -= 2; // So we can increment PC after the switch
        break;
    case 0x3000:
        if (regs_[VX] == NN) {
            pc_ += 2;
        }
        break;
    case 0x4000:
        if (regs_[VX] != NN) {
            pc_ += 2;
        }
        break;
    case 0x5000:
        if (regs_[VX] == regs_[VY]) {
            pc_ += 2;
        }
        break;
    case 0x6000:
        regs_[VX] = NN;
        break;
    case 0x7000:
        regs_[VX] += NN;
        break;
    case 0x8000:
        apply_register_op(VX, VY, N);
        break;
    case 0x9000:
        if (regs_[VX] != regs_[VY]) {
            pc_ += 2;
        }
        break;
    case 0xA000:
        idx_ = NNN;
        break;
    case 0xB000:
        pc_ = regs_[0x0] + NNN;
        pc_ -= 2; // so we can increment PC after the switch
        break;
    case 0xC000:
        regs_[VX] = (rand() % 256) & NN;
        break;
    case 0xD000:
        draw_sprite(VX, VY, N);
        break;
    case 0xE000:
        if ( (NN == 0x9E && keys_[regs_[VX]]) ||
             (NN == 0xA1 && !keys_[regs_[VX]]) ) {
            pc_ += 2;
        }
        break;
    case 0xF000:
        apply_special_op(VX, NN);
        break;
    default:
        stringstream ss;
        ss << "Invalid Opcode: " << std::hex << opcode;
        throw std::runtime_error(ss.str());
    }
}

void Chip8::clear_screen()
{
    for (auto& row : screen_buffer_) {
        for (auto& pixel : row) {
            pixel = 0;
        }
    }
}

void Chip8::subroutine_return()
{
    if (stack_.empty()) throw std::runtime_error("Attempted return on empty stack.");
    pc_ = stack_.top();
    stack_.pop();
}

void Chip8::apply_register_op(uint8_t VX, uint8_t VY, uint8_t op)
{
    uint16_t tmp_16;
    switch(op) {
    case 0:
        regs_[VX] = regs_[VY];
        break;
    case 1:
        regs_[VX] |= regs_[VY];
        break;
    case 2:
        regs_[VX] &= regs_[VY];
        break;
    case 3:
        regs_[VX] ^= regs_[VY];
        break;
    case 4:
        // 1 for carry, 0 for no carry
        tmp_16 = regs_[VX] + regs_[VY];
        regs_[0xF] = tmp_16 > 0xFF ? 0x01 : 0x00;
        regs_[VX] = tmp_16 & 0xFF;
        break;
    case 5:
        // 0 for borrow, 1 for no borrow
        regs_[0xF] = regs_[VY] > regs_[VX] ? 0x00 : 0x01;
        regs_[VX] -= regs_[VY];
        break;
    case 6:
        regs_[0xF] = regs_[VX] & 0x01;
        regs_[VX] >>= 1;
        break;
    case 7:
        // 0 for borrow, 1 for no borrow
        regs_[0xF] = regs_[VX] > regs_[VY] ? 0x00 : 0x01;
        regs_[VX] = regs_[VY] - regs_[VX];
        break;
    case 0xE:
        regs_[0xF] = (regs_[VX] >> 7) & 0x01;
        regs_[VX] <<= 1;
        break;
    default:
        throw std::runtime_error("Invalid register ooperation");
    }
}

void Chip8::draw_sprite(uint8_t VX, uint8_t VY, uint8_t height)
{
    
}

void Chip8::apply_special_op(uint8_t VX, uint8_t op)
{
    uint16_t tmp_16;
    switch (op) {
    case 0x07:
        regs_[VX] = delay_timer_;
        break;
    case 0x0A:
        // TODO(oren): implement blocking get_key
        kp_await_ = true;
        kp_which_ = regs_[VX];
        break;
    case 0x15:
        delay_timer_ = regs_[VX];
        break;
    case 0x18:
        sound_timer_ = regs_[VX];
        break;
    case 0x1E:
        idx_ += regs_[VX];
        regs_[0xF] = idx_ > 0x0FFF ? 1 : 0;
        // TODO(oren): should we allow the overflow to stay in idx_ or guard against?
        break;
    case 0x29:
        idx_ = memory_[regs_[VX] * 5];
        break;
    case 0x33:
        // TODO(oren): implement binary coded decimal...
        draw_flag_ = true;
        break;
    case 0x55:
        // dump registers
        tmp_16 = idx_;
        for (const auto& reg : regs_) {
            memory_[tmp_16++] = reg;
        }
        break;
    case 0x65:
        // load registers
        tmp_16 = idx_;
        for (auto& reg : regs_) {
            reg = memory_[tmp_16++];
        }
        break;
    default:
        stringstream ss;
        ss << "Invalid Special Operation: 0x" << std::hex << (uint16_t) op;
        throw std::runtime_error(ss.str());
    }
}
    
} // namespace chip8
