#include <Arduino_LSM9DS1.h>

unsigned long lastSample = 0;
const unsigned long interval = 20;
const int WINDOW = 31;
const float pi = 3.14159f;
const float cutoff_frequency = 0.14f;
const int M = WINDOW - 1;
const int MA_WINDOW = 10;

float kernel[WINDOW];

struct FIR {
  int index;
  float buffer[WINDOW];
};

struct MovingAverage {
  float sum;
  float buffer[MA_WINDOW];
  int index;
};

void init_FIR(FIR &f);
float update_FIR(FIR &f, float sample);
void init_MovingAverage(MovingAverage &ma);
float update_MovingAverage(MovingAverage &ma, float sample);

void init_kernel() {
  float sum = 0;
  for (int i = 0; i < WINDOW; i++) {
    int offset = i - M / 2;
    if (offset == 0) {
      kernel[i] = 2 * pi * cutoff_frequency;
    } else {
      kernel[i] = sin(2 * pi * cutoff_frequency * offset) / (offset);
    }
    kernel[i] = kernel[i] * (0.42f - 0.5f * cos((2 * pi * i) / M) + 0.08f * cos((4 * pi * i) / M));
  }
  for (int i = 0; i < WINDOW; i++) sum += kernel[i];
  for (int i = 0; i < WINDOW; i++) kernel[i] = kernel[i] / sum;
}

void init_FIR(FIR &f) {
  for (int i = 0; i < WINDOW; i++) f.buffer[i] = 0;
  f.index = 0;
}

float update_FIR(FIR &f, float sample) {
  f.buffer[f.index] = sample;
  f.index = (f.index + 1) % WINDOW;
  float acc = 0.0f;
  int j = f.index;
  for (int i = WINDOW - 1; i >= 0; i--) {
    acc += (kernel[i] * f.buffer[j]);
    j = (j + 1) % WINDOW;
  }
  return acc;
}

void init_MovingAverage(MovingAverage &ma) {
  for (int i = 0; i < MA_WINDOW; i++) ma.buffer[i] = 0.0f;
  ma.sum = 0.0f;
  ma.index = 0;
}

float update_MovingAverage(MovingAverage &ma, float sample) {
  ma.sum -= ma.buffer[ma.index];
  ma.buffer[ma.index] = sample;
  ma.sum += ma.buffer[ma.index];
  ma.index = (ma.index + 1) % MA_WINDOW;
  return ma.sum / MA_WINDOW;
}

FIR fx, fy, fz;
MovingAverage maX, maY, maZ;

void setup() {
  Serial.begin(9600);
  Serial.println("Started");
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");
  init_kernel();
  init_FIR(fx); init_FIR(fy); init_FIR(fz);
  init_MovingAverage(maX); init_MovingAverage(maY); init_MovingAverage(maZ);
}

void loop() {
  unsigned long now = millis();
  if (now - lastSample >= interval) {
    lastSample = now;
    float x, y, z;
    if (IMU.accelerationAvailable()) {
      IMU.readAcceleration(x, y, z);
      float ma_x = update_MovingAverage(maX, x);
      float fir_x = update_FIR(fx, x);
      float ma_y = update_MovingAverage(maY, y);
      float fir_y = update_FIR(fy, y);
      float ma_z = update_MovingAverage(maZ, z);
      float fir_z = update_FIR(fz, z);
      Serial.print(x);      Serial.print(',');
      Serial.print(ma_x);   Serial.print(',');
      Serial.print(fir_x);  Serial.print(',');
      Serial.print(y);      Serial.print(',');
      Serial.print(ma_y);   Serial.print(',');
      Serial.print(fir_y);  Serial.print(',');
      Serial.print(z);      Serial.print(',');
      Serial.print(ma_z);   Serial.print(',');
      Serial.println(fir_z);
    }
  }
}
