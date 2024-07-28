#include <POP32.h>
int set_min[] = { 900, 920, 950, 1100, 1200, 1200, 980, 920 };
int set_max[] = { 3600, 3800, 3600, 3600, 3600, 3800, 3600, 3800 };

int ref[] = { 2200, 2200, 2200, 2200, 2200, 2200, 2200, 2200 };

int numsensor = 8;

const float alpha = 1.5;
const float Ki = 0.001;
int filteredReadings[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
long Position, last_position;
int leftMotorSpeed, rightMotorSpeed;
float last_error = 0, integral = 0;
constexpr float ValuePosition = 4375; //default 3500 8 sensor

// ตัวแปรสำหรับเก็บค่าสีพื้นหลัง (0 = ดำ, 1 = ขาว)
int background_color = 1;


int sensor(int pin) {
  int rawValue = analogRead(pin);

  filteredReadings[pin] = alpha * rawValue + (1 - alpha) * filteredReadings[pin];
  return constrain(map(filteredReadings[pin], set_min[pin], set_max[pin], 100, 0), 0, 100);
}

long readline() {
  int onLine = false;
  long avg = 0;
  long sum = 0;
  int threshold = (background_color == 1) ? 50 : 50;  // ปรับ threshold ตามสีพื้นหลัง

  // อ่านค่าจากเซนเซอร์
  for (int i = 0; i < numsensor; i++) {
    int value = sensor(i);

    if (value > 50) {
      value = 100;
    }
    else {
      value = 0;
    }

    if ((background_color == 1 && value > threshold) ||  // พื้นหลังขาว, เส้นดำ
        (background_color == 0 && value < threshold)) {  // พื้นหลังดำ, เส้นขาว
      onLine = true;
    }

    if (i == 3 || i == 4) {
      avg += value * (i * 1000) * 2; 
    } else {
      avg += value * (i * 1000);
    }
    sum += value;
  }
  if (!onLine) {
    if (last_position > (numsensor - 1) * 1000 / 2) {
      return ((numsensor - 1) * 1000) + 1000;  // หมุนไปด้านขวา
    } else {
      return -1000; //ด้านซ้าย
    }
  }

  // คำนวณตำแหน่งใหม่
  last_position = (sum == 0) ? last_position : (avg / sum);

  return last_position;
}


void Run_PID(int sp, float Kp, float Kd) {
  float adjustableKp = Kp;
  float error = readline() - ValuePosition;

  if (error <= 100 && error >= -100) {
    leftMotorSpeed = sp;
    rightMotorSpeed = sp;
  } else {
    integral = constrain(integral + error, -1000, 1000);
    float derivative = error - last_error;
    long output = (error * adjustableKp) + (integral * Ki) + (derivative * Kd);
    leftMotorSpeed = sp + output;
    rightMotorSpeed = sp - output;
    last_error = error;
  }
  motor(1, constrain(leftMotorSpeed, -100, 100));
  motor(2, constrain(rightMotorSpeed, -100, 100));
}

void setCalibrate(int cal_round) {
  // oled.mode(2);
  for (int i = 0; i < numsensor; i++) {
    set_min[i] = 4095;
    set_max[i] = 0;
  }

  for (int round_count = 0; round_count < cal_round; round_count++) {
    for (int i = 0; i < numsensor; i++) {
      int sensorValue = analogRead(i);
      set_max[i] = max(set_max[i], sensorValue);
      set_min[i] = min(set_min[i], sensorValue);
    }
    delay(1);
  }

  sound(2500, 400);

  oled.text(0, 0, "set_max");
  for (int i = 0; i < 4; i++) {
    oled.text(i + 1, 0, "%d       ", set_max[i]);
  }

  oled.text(0, 9, "set_min");
  for (int i = 0; i < 4; i++) {
    oled.text(i + 1, 9, "%d       ", set_min[i]);
  }
  oled.show();
}

void setup() {
  // setCalibrate(10000);
  while (!sw_OK()) {
    // viewSensor();
    checkReadline();
  }
}

void loop() {
  // if (SW_A()) {
  // setCalibrate(10000);
  // }
  SW_OK_press();
  delay(1000);
  timer(70, 0.022, 0.033, 75);
  timer(100, 0.024, 0.032, 375);
  timer(50, 0.018, 0.028, 575);
  timer(100, 0.024, 0.032, 375);
  timer(40, 0.015, 0.02, 575);

  timer(50, 0.012, 0.02, 575);
  timer(70, 0.022, 0.033, 3500);
  runtocross(60, 0.012, 0.02);
  runtocross(80, 0.012, 0.02);
  runtocross(80, 0.012, 0.02);

  // runtocross(90, 0.019, 0.03);
  // runtocross(90, 0.025, 0.035);
  // runtocross(90, 0.025, 0.035);
  // runtocross(90, 0.025, 0.035);
  // runtocross(80, 0.02, 0.03);
  // runtocross(70, 0.02, 0.035);



  // timer(100, 0.05, 0.06, 450);
  // timer(65, 0.03, 0.04, 550);
  // runtocross(90, 0.04,0.05);
  // runtocross(80, 0.032, 0.04);
  // runtocross(80, 0.032, 0.04);
  // runtocross(80, 0.032, 0.04);
  // runtocross(80, 0.032, 0.04);
  // runtocross(70, 0.025, 0.035);
  ao();
}

void checkReadline() {
  oled.mode(2); 
  oled.textSize(2);
  long readlineValue = readline() - ValuePosition; 
  oled.text(2, 2, "%ld       ", readlineValue); 
  oled.show();
}

void viewSensor() {
  oled.mode(1);
  for (int i = 0; i < numsensor; i++) {
    oled.text(i + 1, 0, "%d ", sensor(i));
    oled.text(i + 1, 6, "%d   ", analog(i));
  }
  oled.show();
}
