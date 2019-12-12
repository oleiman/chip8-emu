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
    bool drawFlag() { return draw_flag_; }
    void resetDrawFlag() { draw_flag_ = false; }

private:
    uint16_t fetch_opcode();
    void decode_and_exec(uint16_t opcode);
    void clear_screen();
    void subroutine_return();
    void apply_register_op(uint8_t VX, uint8_t VY, uint8_t op);
    void draw_sprite(uint8_t VX, uint8_t VY, uint8_t height);
    void apply_special_op(uint8_t VX, uint8_t op);
    
    std::array<uint8_t, MEM_B> memory_;
    std::array<uint8_t, N_REGS> regs_;
    uint16_t idx_;
    uint16_t pc_;

    std::stack<uint16_t> stack_;

    uint8_t delay_timer_;
    uint8_t sound_timer_;

    std::array<uint8_t, N_KEYS> keys_;
    bool kp_await_;
    uint8_t kp_which_;
    std::array<std::array<uint8_t, SCREEN_WIDTH>, SCREEN_HEIGHT> screen_buffer_;

    bool draw_flag_;
    
};

} // namespace chip8
