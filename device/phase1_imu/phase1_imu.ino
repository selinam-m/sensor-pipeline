#include <Arduino_LSM9DS1.h>

unsigned long lastSample = 0;
const unsigned long interval = 20;
const int WINDOW = 5;

struct MovingAverage
{
  float sum;
  float buffer[WINDOW];
  int index;
};

void init_MovingAverage(MovingAverage &ma) {
  for (int i = 0; i < WINDOW; i++){
    ma.buffer[i] = 0;
  }
  ma.sum = 0.0f;
  ma.index = 0;
}

float update_MovingAverage (MovingAverage &ma, float sample) {
  ma.sum -= ma.buffer[ma.index];
  ma.buffer[ma.index] = sample;
  ma.sum += ma.buffer[ma.index];
  ma.index = (ma.index + 1) % WINDOW;
  return ma.sum / WINDOW;
}

MovingAverage maX;
MovingAverage maY;
MovingAverage maZ;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Started");

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");
  Serial.println();
  Serial.println("Acceleration in g's");
  Serial.println("X\tY\tZ");


  init_MovingAverage(maX);
  init_MovingAverage(maY);
  init_MovingAverage(maZ);
}

void loop() {
  unsigned long now = millis();

  if (now - lastSample >= interval) {
    lastSample = now;
    float x, y, z;

    if (IMU.accelerationAvailable()) {
      IMU.readAcceleration(x, y, z);

      float filter_x = update_MovingAverage(maX, x);
      float filter_y = update_MovingAverage(maY, y);
      float filter_z = update_MovingAverage(maZ, z);

      Serial.print(filter_x);
      Serial.print('\t');
      Serial.print(filter_y);
      Serial.print('\t');
      Serial.println(filter_z);
    }
  }
  
}