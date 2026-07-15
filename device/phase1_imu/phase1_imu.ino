#include <Arduino_LSM9DS1.h>
#include <string.h>

unsigned long lastSample = 0;
const unsigned long interval = 20;
const int WINDOW = 31;
const float pi = 3.14159f;
const float cutoff_frequency = 0.14f;
const int M = WINDOW - 1;
const uint8_t SYNC0 = 0x55;
const uint8_t SYNC1 = 0xAA;

void send_frame(float x, float y, float z);


float kernel[WINDOW];

struct FIR
{
  int index;
  float buffer[WINDOW];
};

void init_FIR(FIR &f);
float update_FIR(FIR &f, float sample);

void init_kernel () {
  float sum = 0;
  for (int i = 0; i < WINDOW; i++) {
    int offset = i - M / 2;
    if (offset == 0) {
      kernel[i] = 2*pi*cutoff_frequency;
    }
    else {
      kernel[i] = sin(2*pi*cutoff_frequency * offset) / (offset);
    }
    kernel[i] = kernel[i] * (0.42f - 0.5f * cos((2 * pi * i) / M) + 0.08f * cos((4 * pi * i) / M));

  }

  for (int i = 0; i < WINDOW; i++) {
    sum += kernel[i];
  }

  for (int i = 0; i < WINDOW; i++) {
    kernel[i] = kernel[i] / sum;
  }

}



void init_FIR (FIR &f) {
  for (int i = 0; i <WINDOW; i++){
    f.buffer[i] = 0;
  }
  f.index = 0;
}

float update_FIR (FIR &f, float sample) {
  f.buffer[f.index] = sample;
  f.index = (f.index + 1) % WINDOW;

  float acc = 0.0f;
  int j = f.index;

  for (int i  = WINDOW - 1; i >= 0; i--) {
    acc += (kernel[i] * f.buffer[j]);
    j = (j + 1) % WINDOW;
  }
  return acc;
}

FIR fx;
FIR fy;
FIR fz;

void send_frame(float x, float y, float z) {
  uint8_t frame [15];
  frame[0] = SYNC0;
  frame[1] = SYNC1;

  memcpy(&frame[2], &x, 4);
  memcpy(&frame[6], &y, 4);
  memcpy(&frame[10], &z, 4);
  

  uint8_t checksum = 0;
  for (int i = 2; i < 14; i++) {
    checksum ^= frame[i];
  }
  frame[14] = checksum;

  Serial.write(frame, 15);

}

void setup() {
  Serial.begin(115200);

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }



  init_kernel();
  
  init_FIR(fx);
  init_FIR(fy);
  init_FIR(fz);
}

void loop() {
  unsigned long now = millis();

  if (now - lastSample >= interval) {
    lastSample = now;
    float x, y, z;

    if (IMU.accelerationAvailable()) {
      IMU.readAcceleration(x, y, z);

      float filter_x = update_FIR(fx, x);
      float filter_y = update_FIR(fy, y);
      float filter_z = update_FIR(fz, z);

      send_frame(filter_x, filter_y, filter_z);
    }
  }
  
}