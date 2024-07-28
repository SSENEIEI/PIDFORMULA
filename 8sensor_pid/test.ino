// long readline() {
//   const int weights[] = {0, 1, 2, 3};  // น้ำหนักของเซ็นเซอร์แต่ละตัว (ปรับได้ตามต้องการ)
//   const int threshold = 70;           // ค่า threshold สำหรับการตรวจจับเส้น

//   int onLineSensors = 0;              // นับจำนวนเซ็นเซอร์ที่อยู่บนเส้น
//   long weightedSum = 0;               // ผลรวมของค่าเซ็นเซอร์คูณน้ำหนัก
//   long totalWeight = 0;               // ผลรวมของน้ำหนัก

//   for (int i = 0; i < 4; i++) {
//     if (_sensor_pro_(i) > threshold) {
//       onLineSensors++;
//       weightedSum += _sensor_pro_(i) * weights[i];
//       totalWeight += weights[i];
//     }
//   }

//   if (onLineSensors > 0) {
//     Position = (weightedSum * 10) / totalWeight; // คำนวณตำแหน่งเส้นแบบ Weighted Average
//   } else {
//     if (last_position > 1250) {
//       return 2500;
//     } else {
//       return 0;
//     }
//   }

//   return last_position = Position;
// }


// long readline() {
//   const int ON_LINE_THRESHOLD = 70; // ค่า Threshold สำหรับการตรวจจับเส้น
//   const int TILT_FACTOR = 500;     // ค่าสำหรับปรับ Position เมื่อเอียง
//   const int MIN_VALID_SENSOR_VALUE = 5; // ค่าต่ำสุดของเซนเซอร์ที่ถือว่าถูกต้อง

//   bool onLine = false;
//   long avg = 0;
//   long sum = 0;
//   int validSensorCount = 0; // นับจำนวนเซ็นเซอร์ที่อ่านค่าได้ถูกต้อง

//   for (int i = 0; i < 4; i++) {
//     long value = _sensor_pro_(i); // ใช้ _sensor_pro_ เพื่ออ่านค่าเฉลี่ย

//     // กรองสัญญาณรบกวน
//     if (value > MIN_VALID_SENSOR_VALUE) {
//       validSensorCount++;
//       if (value > ON_LINE_THRESHOLD) {
//         onLine = true;
//       }
//       avg += (long)value * (i * 100);
//       sum += value;
//     }
//   }

//   if (validSensorCount == 0) { // ไม่มีเซ็นเซอร์ใดอ่านค่าได้
//     return last_position; // คืนค่า Position เดิม
//   }

//   if (!onLine) { // ถ้าไม่มีเซ็นเซอร์ใดตรวจจับเส้น
//     if (last_position > 1250) {
//       return 2500;
//     } else {
//       return 0;
//     }
//   } else { // ถ้ามีเซ็นเซอร์ตรวจจับเส้น
//     Position = avg / sum;

//     // ตรวจสอบการเอียงและปรับ Position
//     if (_sensor_pro_(0) > ON_LINE_THRESHOLD && _sensor_pro_(1) < ON_LINE_THRESHOLD) { // เซ็นเซอร์ซ้ายสุดตรวจจับเส้น
//       Position -= TILT_FACTOR;
//     } else if (_sensor_pro_(3) > ON_LINE_THRESHOLD && _sensor_pro_(2) < ON_LINE_THRESHOLD) { // เซ็นเซอร์ขวาสุดตรวจจับเส้น
//       Position += TILT_FACTOR;
//     }
//   }

//   last_position = Position;
//   return Position;
// }



//////////////readline basic ยุคแรก ////////////////////////////
// long readline() {
//   int On_line = 0;
//   int threshold = (background_color == 1) ? 50 : 50;
//   if (sensor(0) > threshold || sensor(1) > threshold || sensor(2) > threshold || sensor(3) > threshold) {
//     On_line = 1;
//   } else {
//     On_line = 0;
//   }
//   if (On_line == 1) {
//     long A = (sensor(0) * 0 + sensor(1) * 100 + sensor(2) * 200 + sensor(3) * 300);
//     long B = (sensor(0) + sensor(1) + sensor(2) + sensor(3));
//     Position = A / B * 10;
//   } else {
//     if (last_position > ValuePosition) {
//       return 5000;
//     } else {
//       return -2000;
//     }
//   }
//   return last_position = Position;
// }





//สมการ FLC

// float errorMembership(float error) {
//   // Fuzzy sets: Negative (N), Zero (Z), Positive (P)
//   if (error <= -500) return 1;
//   if (error >= 500) return 0;
//   return (500 - error) / 1000.0;  // Linear membership function
// }

// float cornerMembership(float cornerValue) {
//   // Fuzzy sets: Low (L), Medium (M), High (H)
//   if (cornerValue < 0.2) return 0;
//   if (cornerValue > 0.8) return 1;
//   return (cornerValue - 0.2) / 0.6;  // Linear membership function
// }

// float speedChangeMembership(float change) {
//   // Fuzzy sets: SlowDown (SD), Maintain (M), SpeedUp (SU)
//   if (change < -50) return 1.0;
//   if (change > 50) return 0.0;
//   return (50 - change) / 100.0;  // Linear membership function
// }

// void Run_FLC(int sp) {
//   float error = readline() - ValuePosition;
//   float cornerValue = (sensor(0) + sensor(3)) / 200.0;  // ค่า cornerValue จากเซ็นเซอร์
//   float errorMF = errorMembership(error);
//   float cornerMF = cornerMembership(cornerValue);

//   // Fuzzy rules (ตัวอย่าง)
//   float slowDownMF = min(errorMF, cornerMF);
//   float maintainMF = 1.0 - max(slowDownMF, 0.0);  // สมมติว่าไม่มี SpeedUp
  
//   // Defuzzification (ใช้ Center of Gravity Method)
//   float speedChange = (-50 * slowDownMF + 0 * maintainMF) / (slowDownMF + maintainMF);

//   leftMotorSpeed = sp + speedChange;
//   rightMotorSpeed = sp - speedChange;
//   motor(1, constrain(leftMotorSpeed, -100, 100));
//   motor(2, constrain(rightMotorSpeed, -100, 100));
// }