#include "include/chip8.hpp"

#include <fstream>
#include <iostream>

#define CODE_START 0x200
#define CODE_END 0xEA0

using std::array;
using std::ifstream;
using std::make_unique;
using std::stack;
using std::string;

namespace chip8 {
    Chip8::Chip8():
        delay_timer_(0xF)
        , sound_timer_(0xF) {}

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
    }

    void Chip8::setKey(uint8_t key, bool state)
    {
        if (key <= 0xF) {
            keys_[key] = state;
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
    
    
} // namespace chip8
