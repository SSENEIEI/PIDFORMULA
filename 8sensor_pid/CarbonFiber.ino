#include <POP32.h>

int set_min[] = { 840, 812, 955, 1018, 1119, 1118, 851, 909 };
int set_max[] = { 3780, 3780, 3780, 3780, 3780, 3780, 3780, 3780 };

int ref[] = { 2200, 0, 0, 2200 };

const int numsensor = 8;
const float Ki = 0;
int leftMotorSpeed, rightMotorSpeed;
float integral;
int last_position, last_error;
unsigned long lastTime = 0;


int sensor(int pin) {
  int value = 0;
  value = map(analogRead(pin), set_min[pin], set_max[pin], 100, 0);
  if (value < 0) value = 0;
  else if (value > 100) value = 100;
  return value;
}

int readline() {
  bool onLine = false;
  long avg = 0;
  long sum = 0;
  const int threshold = 50;

  for (int i = 0; i < numsensor; i++) {
    int value = sensor(i);

    if (value >= threshold) {
      onLine = true;
    }
    avg += value * (i * 100);
    sum += value;
  }

  if (!onLine) {
    // หมุนไปด้านซ้ายหรือขวาตามปกติถ้าไม่ใช่โค้ง
    if (last_position > (numsensor - 1) * 100 / 2) {
      return ((numsensor - 1) * 100);  // หมุนไปด้านขวา
    } else {
      return 0;  // หมุนไปด้านซ้าย
    }
  }
  if (sum == 0) {
    return 0;  // หารด้วยศูนย์ไม่ได้
  }

  last_position = avg / sum;// คำนวณตำแหน่งใหม่และคืนค่าตำแหน่งที่คำนวณได้

  if (abs(last_position) < 10) {
    return 0;
  } else if (abs(last_position) < 20) {
    return last_position / 2;  // ลดความเร็วในการหมุนเมื่อใกล้ศูนย์กลาง
  }

  return last_position;
}

void Run_PID(const int sp, const float Kp, const float Kd) {
  int present_position = readline();
  const int setpoint = ((numsensor - 1) * 100) / 2;
  int error = present_position - setpoint;
  // integral = integral + error;
  float derivative = error - last_error;
  float output = Kp * error + Ki * integral + Kd * derivative;

  leftMotorSpeed = constrain(sp + output, -100, 100);
  rightMotorSpeed = constrain(sp - output, -100, 100);

  if (leftMotorSpeed <= 20) {
    leftMotorSpeed = -20;
  }
  if (rightMotorSpeed <= 20) {
    rightMotorSpeed = -20;
  }
  motor(1, leftMotorSpeed);
  motor(2, rightMotorSpeed);

  last_error = error;
}

void setup() {

  // Serial.begin(9600);
  // setCalibrate(10000);
  // moveRobot(0, 0);
}

void loop() {
  sw_OK_press();
  delay(500);
  // viewSensorOLED();
  // checkReadline();

  timer(70, 0.11, 2, 2000);
  timer(90, 0.2, 3, 800);
  timer(60, 0.1, 2, 600);
  timer(90, 0.2, 3, 500);
  timer(60, 0.1, 2, 4000);
  timerLINEOFF(60, 0.1, 2, 1000);
  timer(60, 0.1, 2, 600);
  AO();
}

bool whileLINE = false;
int readlineOFFLINE() {
  bool onLine = false;
  long avg = 0;
  long sum = 0;
  const int threshold = 50;

  for (int i = 0; i < numsensor; i++) {
    int value = sensor(i);

    if (value >= threshold) {
      onLine = true;
      whileLINE = false;
    }
    avg += value * (i * 100);
    sum += value;
  }

  if (!onLine) {
    whileLINE = true;
  }
  // คำนวณตำแหน่งใหม่และคืนค่าตำแหน่งที่คำนวณได้
  last_position = avg / sum;
  return last_position;
}
void Run_PIDLINEOFF(const int sp, const float Kp, const float Kd) {
  if (whileLINE) {
    moveRobot(sp + 5, sp);
    delay(30);
    whileLINE = false;
  } else {
    int present_position = readlineOFFLINE();
    const int setpoint = ((numsensor - 1) * 100) / 2;
    int error = present_position - setpoint;
    integral = integral + error;
    float derivative = error - last_error;
    float output = Kp * error + Ki * integral + Kd * derivative;

    leftMotorSpeed = constrain(sp + output, -100, 100);
    rightMotorSpeed = constrain(sp - output, -100, 100);

    if (leftMotorSpeed <= 20) {
      leftMotorSpeed = -20;
    }
    if (rightMotorSpeed <= 20) {
      rightMotorSpeed = -20;
    }
    motor(1, leftMotorSpeed);
    motor(2, rightMotorSpeed);

    last_error = error;
  }
}

void timerLINEOFF(int sp, float Kp, float Kd, unsigned long durationMillis) {
  unsigned long startTime = millis();
  while (millis() - startTime < durationMillis) {
    Run_PIDLINEOFF(sp, Kp, Kd);
  }
}
