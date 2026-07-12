#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <cerrno>
#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>

using namespace std;

void readSerialPort(int serial_port);

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
    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        cout << "Error " << errno << " from tcsetattr: " << strerror(errno) << endl;
        return 1;
    }
    readSerialPort(serial_port);
    return 0;
}

void readSerialPort(int serial_port){
    char buffer[256];
    string bytes_stored = "";
    ofstream csv_file("comparison.csv");
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
            bytes_stored.append(buffer, bytes_read);
            size_t end_position;
            while ((end_position = bytes_stored.find('\n')) != string::npos) {
                string temp_line = bytes_stored.substr(0, end_position);
                bytes_stored.erase(0, end_position + 1);
                if (!temp_line.empty() && temp_line.back() == '\r') {
                    temp_line.pop_back();
                }
                if (!temp_line.empty() && (isdigit(temp_line[0]) || temp_line[0] == '-')) {
                    csv_file << temp_line << '\n';
                }
            }
        }
    }
    csv_file.close();
}
