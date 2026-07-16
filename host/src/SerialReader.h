#pragma once

#include <cstdint>
#include <vector>

class SerialReader
{
private:
    int fd;
    
public:
    SerialReader(const char* port);
    SerialReader(const SerialReader&) =delete;
    SerialReader& operator=(const SerialReader&) =delete;
    ~SerialReader();
    std::vector<uint8_t> read_bytes();

};

