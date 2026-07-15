#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <cerrno>
#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdint.h>

using namespace std;

void readSerialPort(int serial_port);
float print_float (const uint8_t *src);


int main() {
    int serial_port = open("/dev/ttyACM0", O_RDWR);
    
    if (serial_port < 0) {
        cout << "Error " << errno << " from open: " << strerror(errno) << endl;
        return 1;
    }
    struct termios tty;
    

    if (tcgetattr(serial_port, &tty) != 0) {
        cout << "Error " << errno << " from tcgetattr: " << strerror(errno) << endl;
        return 1;
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

    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        cout << "Error " << errno << " from tcsetattr: " << strerror(errno) << endl;
        return 1;
    }

    readSerialPort(serial_port);
    
    return 0;
}

void readSerialPort(int serial_port){
    uint8_t buffer [256];
    std::vector<uint8_t> buf;
    ofstream csv_file("data.csv");
    

    if (!csv_file.is_open()) {
        cout << "Error " << errno << strerror(errno) << endl;
        return;
    }

    while (true){
        
        int bytes_read = read(serial_port, buffer, sizeof(buffer));

        if (bytes_read < 0) {
            cout << "Error " << errno << " from reading stream: " << strerror(errno) << endl;
            break;
        }

        if (bytes_read > 0) {
            buf.insert(buf.end(), buffer, buffer + bytes_read);

            while (buf.size() >= 15) {
                if (buf[0] != 0x55 || buf[1] != 0xAA) {
                    buf.erase(buf.begin());
                    continue;
                }
                uint8_t checksum = 0;
                for (int i = 2; i < 14; i++) {
                    checksum ^= buf[i];
                }

                if (checksum != buf[14]){
                    buf.erase(buf.begin());
                    continue;
                }

                float x = print_float(&buf[2]);
                float y = print_float(&buf[6]);
                float z = print_float(&buf[10]);

                buf.erase(buf.begin(), buf.begin() + 15);

                csv_file << x << "," << y << "," << z << endl;
            
            }
        }

       
    }

    csv_file.close();


}

float print_float (const uint8_t *src) {
    float value;
    memcpy(&value, src, 4);
    return value;
}

