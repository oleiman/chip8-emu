#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <stack>
#include <string>

#define MEM_B 4096
#define N_REGS 16
#define MAX_STACK 16
#define N_KEYS 16

#define SCREEN_HEIGHT 32
#define SCREEN_WIDTH 64

namespace chip8 {

class Chip8 {
public:

    explicit Chip8();
    ~Chip8() = default;

    bool loadApp(const std::string& app);
    void cycle();
    void setKey(uint8_t key, bool state);
    void exportScreenBuf(uint8_t dest[][SCREEN_WIDTH][3]);

private:
    std::array<uint8_t, MEM_B> memory_;
    std::array<uint8_t, N_REGS> reg_;
    uint16_t index_;
    uint16_t pc_;

    std::stack<uint16_t> stak_;

    uint8_t delay_timer_;
    uint8_t sound_timer_;

    std::array<uint8_t, N_KEYS> keys_;
    std::array<std::array<uint8_t, SCREEN_WIDTH>, SCREEN_HEIGHT> screen_buffer_;
    
};

} // namespace chip8
