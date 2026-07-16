#pragma once

#include <cstdint>
#include <optional>
#include <vector>

struct Reading
{
    float x;
    float y;
    float z;
};

class FrameParser
{
private:
    int bytes_discarded;
    int valid_frames;
    std::vector<uint8_t> buf;

public:
    FrameParser();
    void feed(const std::vector<uint8_t>& data);
    std::optional<Reading> next();
    int get_frames() const {return valid_frames;}
    int get_discarded_bytes() const { return bytes_discarded;}

};