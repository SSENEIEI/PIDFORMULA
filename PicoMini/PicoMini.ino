#include "POP32.h"

int _sensorPins[20];
int _NumofSensor = 8;
int _min_sensor_values[20];
int _max_sensor_values[20];
double _lastPosition = 0;
int _Sensitive  = 20;
int stateOfRunPID = 0;
double  errors = 0, output = 0, integral = 0, derivative = 0, previous_error = 0;
uint8_t FrontLineColor = 0;
uint8_t BackLineColor = 0;

void setSensitive(const uint16_t  _SensorSensitive)
{
  _Sensitive = _SensorSensitive;
}
void setSensorPins(const int * pins, int _NumofSensor)
{
  NumofSensor = _NumofSensor;
  for (uint8_t i = 0; i < NumofSensor; i++)
  {
    _sensorPins[i] = _pins[i];
    _min_sensor_values[i] = 4095;
    _max_sensor_values[i] = 0;
  }

}
void setSensorMin(const int * _MinSensor)
{
  for (uint8_t i = 0; i < _NumofSensor; i++)
  {
    _min_sensor_values[i] = _MinSensor[i];
  }
}
void setSensorMax(const int * _MaxSensor)
{
  for (uint8_t i = 0; i < _NumofSensor; i++)
  {
    _max_sensor_values[i] = _MaxSensor[i];
  }
}
void setFrontLineColor(const uint16_t  _setFrontLineColor)     // if Value = 1 is BlackLine ,value = 0 is WhiteLine
{
  FrontLineColor = _setFrontLineColor;
}

void setCalibrate(int cal_round) {
  for (uint8_t i = 0; i < _NumofSensor; i++){
    _max_sensor_values[i] = 0;
    _min_sensor_values[i] = 4095;
  }
  
  for (int round_count = 0; round_count < cal_round; round_count ++ ) {

    for (uint8_t i = 0; i < _NumofSensor; i++)
    {
      if (analogRead(_sensorPins[i]) > _max_sensor_values[i] ) {
        _max_sensor_values[i]  = analogRead(_sensorPins[i]);
        if (_max_sensor_values[i] > 4095 )_max_sensor_values[i] = 4095;
      }
    }
    for (uint8_t i = 0; i < _NumofSensor; i++)
    {
      if (analogRead(_sensorPins[i]) < _min_sensor_values[i] ) {
        _min_sensor_values[i] = analogRead(_sensorPins[i]);
        if (_min_sensor_values[i] < 0) _min_sensor_values[i] = 0;
      }
    }
    
  }
  Serial.println();
  for(int i = 0 ;i < 8;i++){
    Serial.print("Min"+String(i)+"=");
    Serial.print(_min_sensor_values[i]); 
    Serial.print("   ");
  }
  Serial.println();
  for(int i = 0 ;i < 8;i++){
    Serial.print("Max"+String(i)+"=");
    Serial.print(_max_sensor_values[i]);
    Serial.print("   ");  
  }
  // }
  // Serial.print("ADC0=");
  // Serial.print(_min_sensor_values[0]);
  // Serial.print("  ADC1=");
  // Serial.print(analogRead(1));
  // Serial.print("  ADC2=");
  // Serial.print(analogRead(2));
  // Serial.print("  ADC3=");
  // Serial.print(analogRead(3));
  // Serial.print("  ADC4=");
  // Serial.print(analogRead(4));
  // Serial.print("  ADC5=");
  // Serial.print(analogRead(5));
  // Serial.print("  ADC6=");
  // Serial.print(analogRead(6));
  // Serial.print("  ADC7=");
  // Serial.println(analogRead(7));
  
}

int ReadLightSensor(int analog_CH) {
  int value = 0;

  if(FrontLineColor == 0)value= map(analogRead(_sensorPins[analog_CH]), _min_sensor_values[analog_CH], _max_sensor_values[analog_CH], 100, 0);
  else if (FrontLineColor == 1) value= map(analogRead(_sensorPins[analog_CH]), _min_sensor_values[analog_CH], _max_sensor_values[analog_CH], 0, 100);
  if(value < 0)value = 0;
  else if(value >100)value = 100;
  return value;
}

int readline()
{
  uint8_t onLine = 0;
  long avg = 0;
  long sum = 0;
  for (uint8_t i = 0; i < _NumofSensor; i++)
  {
    long value = ReadLightSensor(i);
    if (value > _Sensitive) {
      onLine = 1;
    }
    if (value > 5)
    {
      avg += (long)value * (i * 100);
      sum += value;
    }
  }
  if (onLine == 0)
  {
    if (_lastPosition < (_NumofSensor - 1) * 100 / 2)
    {
      return 0;
    }
    else
    {
      return (_NumofSensor - 1) * 100;
    }
  }
  _lastPosition = avg / sum;
  return _lastPosition;
}

void lineFollow_PID(int RUN_PID_speed , float RUN_PID_KP, float RUN_PID_KI, float RUN_PID_KD) {
  int speed_PID = RUN_PID_speed;
  int present_position = readline();
  int setpoint = ((_NumofSensor - 1) * 100) / 2;
  errors = present_position - setpoint;
  derivative = (errors - previous_error) ;
  output = (RUN_PID_KP * errors)   + (RUN_PID_KD * derivative);
  int motorL = constrain(RUN_PID_speed - output, -100, 100);
  int motorR = constrain(RUN_PID_speed + output, -100, 100);
  motor(1,motorL);
  motor(2,motorR);
  previous_error = errors;
  // delay(1);
}

void setup() {
  // Serial.begin(115200);
  setSensitive(20);
  setFrontLineColor(1);
  setSensorPins((const int[]){ 0, 1, 2, 3, 4, 5, 6, 7 }, 8);                        //คำสั่งกำหนด พิน ของเซ็นเซอร์แบบ Multiplxer
  setSensorMax((const int[]){ //ตัวเลขที่หุ่นยนต์อ่านได้จาก พื้นสีขาว ของเซ็นเซอร์ตัวนั้นๆ
                              3856, 3854, 3877, 3880, 3870, 3874, 3859, 3856 });
  setSensorMin((const int[]){ //ตัวเลขที่หุ่นยนต์อ่านได้จาก เส้นสีดำ ของเซ็นเซอร์ตัวนั้นๆ
                              799, 692, 1049, 1029, 673, 870, 680, 742 });
  // setCalibrate(3000);
  while (!sw_OK());
  delay(200);
  for(float i = 20;i<60;i+=0.1){
    lineFollow_PID(i, 0.2, 0,0);
    delay(1);
  }
} 

void loop() { //0.75 3 -> 0.65 7
  lineFollow_PID(50, 0.85, 0, 0);         //คำสั่งวิ่งตามเส้นแบบ PID (ความเร็ว , KP, KI ,KD)   KI ให้เป็น 0 เสมอ
}
