#include "SerialReader.h"

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>

SerialReader::SerialReader(const char* port){
    fd = open(port, O_RDWR);

    if (fd < 0) {
        throw std::runtime_error("failed to open serial reader");
    }

    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        throw std::runtime_error("failed to get termios attributes");
    }

    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag |= CREAD | CLOCAL;

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO;
    tty.c_lflag &= ~ECHONL;
    tty.c_lflag &= ~ISIG;

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);

    tty.c_oflag &= ~OPOST;
    tty.c_oflag &= ~ONLCR;

    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 10;

    cfsetispeed(&tty, B115200);
    cfsetospeed(&tty, B115200);

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        throw std::runtime_error("failed to set termios attributes");
    }
}

std::vector<uint8_t> SerialReader::read_bytes(){
    std::vector<uint8_t> buf(256);
    int n = ::read(fd, buf.data(), buf.size());

    if (n < 0){
        throw std::runtime_error("failed to read bytes from serial port");
    }
    buf.resize(n);
    return buf;
    
}

SerialReader::~SerialReader(){
    close(fd);

}