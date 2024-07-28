
void timer(int sp, float Kp, float Kd, unsigned long durationMillis) {
  unsigned long startTime = millis();
  while (millis() - startTime < durationMillis) {
    Run_PID(sp, Kp, Kd);
  }
}

void runtocross(int sp, float Kp, float Kd) {
  while (true) {
    if (sensor(0) > 50 && sensor(7) > 50) {
      timer(sp, 0.01, 0, 100);
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
  while (analog(1) > 2000) {
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
  while (analog(4) > 2000) {
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
  for (int i = 50; i < 80; i++) {
    Run_PID(i, 0.35, 0);
    delay(20);
  }
}