
void timer(int sp, float Kp, float Kd, unsigned long durationMillis) {
  unsigned long startTime = millis();
  while (millis() - startTime < durationMillis) {
    Run_PID(sp, Kp, Kd);
  }
}

void runtocross(int sp, float Kp, float Kd) {
  while (true) {
    if (sensor(0) > 50 && sensor(5) > 50) {
      timer(sp, 0, 1, 25);
      break;
    }
    Run_PID(sp, Kp, Kd);
  }
}
void crossL(int sp, float Kp, float Kd) {
  while (true) {
    if (analog(0) < ref[0] && analog(1) < ref[1] && analog(2) > ref[2] && analog(3) > ref[3]) {
      L();
      break;
    }
    Run_PID(sp, Kp, Kd);
  }
}
void crossR(int sp, float Kp, float Kd) {
  while (true) {
    if (analog(0) > ref[0] && analog(1) > ref[1] && analog(2) < ref[2] && analog(3) < ref[3]) {
      R();
      break;
    }
    Run_PID(sp, Kp, Kd);
  }
}
void moveRobot(int speed1, int speed2) {
  motor(1, speed1);
  motor(2, speed2);
}

void L() {
  moveRobot(-100, 85);
  delay(80);
  while (analog(4) > 2000) {
    moveRobot(-25, 15);
  }
  moveRobot(100, -100);
  delay(10);
  fd2(0, 0);
  delay(1);
}

void R() {
  moveRobot(85, -100);
  delay(85);
  while (analog(3) > 2000) {
    moveRobot(15, -25);
  }
  moveRobot(-100, 100);
  delay(10);
  fd2(0, 0);
  delay(1);
}

void U() {
  moveRobot(-100, 45);
  delay(240);
  while (analog(4) > 1975) {
    moveRobot(-25, 15);
  }
  moveRobot(100, -100);
  delay(13);
  AO();
  delay(1);
}

void speed() {
  for (int i = 50; i < 100; i += 1) {
    Run_PID(i, 0.05, 2);
  }
}
void checkReadline() {
  int present_position = readline();
  int setpoint = ((numsensor - 1) * 100) / 2;
  int error = present_position - setpoint;
  Serial.print("Readline: ");
  Serial.println(error);
}
void viewSensorSerial() {
  for (int i = 0; i < numsensor; i++) {
    Serial.print("Sensor ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(sensor(i));
    Serial.print(" Analog: ");
    Serial.println(analog(i));
  }
  Serial.println("////////////////////");
}
void setCalibrate(int cal_round) {
  for (int i = 0; i < numsensor; i++) {
    set_min[i] = 1000;
    set_max[i] = 0;
  }

  for (int round_count = 0; round_count < cal_round; round_count++) {
    for (int i = 0; i < numsensor; i++) {
      int sensorValue = analogRead(i);
      set_max[i] = myMax(set_max[i], sensorValue);
      set_min[i] = myMin(set_min[i], sensorValue);
    }
    delay(1);
  }

  tone(8, 2500, 400);  // Assuming the sound function is replaced with tone(pin, frequency, duration)

  Serial.println("Calibration Complete");
  Serial.println("set_max:");
  for (int i = 0; i < numsensor; i++) {
    Serial.print("Sensor ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.println(set_max[i]);
  }

  Serial.println("set_min:");
  for (int i = 0; i < 4; i++) {
    Serial.print("Sensor ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.println(set_min[i]);
  }
}
int myMax(int a, int b) {
  return (a > b) ? a : b;
}

int myMin(int a, int b) {
  return (a < b) ? a : b;
}
void checkState(){
  int present_position = readline();
  int setpoint = ((numsensor - 1) * 100) / 2;
  int error = present_position - setpoint;
  if(error < 10 && error > -10){
    Serial.print("FD");
  }
  if(error > 50){
    Serial.print("R");
  }
  if(error < -50){
    Serial.print("L");
  }
}
void viewSensorOLED() {
  oled.mode(1);
  for (int i = 0; i < numsensor; i++) {
    oled.text(i + 1, 0, "%d ", sensor(i));
    oled.text(i + 1, 6, "%d   ", analog(i));
  }
  oled.show();
}
void checkReadlineOLED() {
  oled.mode(2);
  int present_position = readline();
  int setpoint = ((numsensor - 1) * 100) / 2;
  int error = present_position - setpoint;
  oled.text(2,2,"   %d", error);
  oled.show();
}
