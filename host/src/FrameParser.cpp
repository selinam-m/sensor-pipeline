#include "FrameParser.h"

#include <vector>
#include <cstring>

float return_float(const uint8_t *src){
    float value;
    std::memcpy(&value, src, 4);
    return value;
}

FrameParser::FrameParser() 
    : bytes_discarded(0), valid_frames(0) {}


void FrameParser::feed(const std::vector<uint8_t>& data){
    if (data.size() > 0){
        buf.insert(buf.end(), data.begin(), data.end());
    }
}

std::optional<Reading> FrameParser::next() {
    while (buf.size() >= 15) {
        if (buf[0] != 0x55 || buf[1] != 0xAA) {
            bytes_discarded++;
            buf.erase(buf.begin());
            continue;
            }
        uint8_t checksum = 0;
        for (int i = 2; i < 14; i++) {
            checksum ^= buf[i];
        }

        if (checksum != buf[14]){
            buf.erase(buf.begin());
            bytes_discarded++;
            continue;
        }
        Reading r;

        float x = return_float(&buf[2]);
        float y = return_float(&buf[6]);
        float z = return_float(&buf[10]);

        
        r.x = x;
        r.y = y;
        r.z = z;

        valid_frames++;

        buf.erase(buf.begin(), buf.begin() + 15);
        return r;
    }
    return std::nullopt;

}