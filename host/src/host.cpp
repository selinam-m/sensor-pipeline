#include <iostream>
#include <fstream>
#include <vector>
#include <stdint.h>

#include "SerialReader.h"
#include "FrameParser.h"

using namespace std;

int main() {
    try {
        SerialReader serial("/dev/ttyACM0");
        FrameParser parser;
        ofstream csv_file("data.csv");
        vector<uint8_t> buf;

        while (true) {
        buf = serial.read_bytes();
        parser.feed(buf);

        while (auto r = parser.next()) {
            csv_file << r->x << "," << r->y << "," << r->z << endl;
        }
        }
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << '\n';
        return 1;
    }
   
   return 0;
}

