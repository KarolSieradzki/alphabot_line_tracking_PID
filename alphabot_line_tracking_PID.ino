#include "TRSensors.h"

#define NUM_SENSORS 5
// sensors 0 through 5 are connected to analog inputs 0 through 5, respectively
TRSensors trs =   TRSensors();
unsigned int sensorValues[NUM_SENSORS];

int LMotorSpeedPin = 5;
int RMotorSpeedPin = 6;

volatile int prev_error = 0;

void setup()
{
  Serial.begin(115200);

  digitalWrite(A1, HIGH);
  digitalWrite(A0, LOW);
  digitalWrite(A2, HIGH);
  digitalWrite(A3, LOW);
  analogWrite(LMotorSpeedPin,0);
  analogWrite(RMotorSpeedPin,0);
  
  // make the calibration
  for (int i = 0; i < 400; i++)  
    trs.calibrate();
  
  Serial.println("calibrate done");

  // print the calibration minimum values measured when emitters were on
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(trs.calibratedMin[i]);
    Serial.print(' ');
  }
  Serial.println();
  
  // print the calibration maximum values measured when emitters were on
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(trs.calibratedMax[i]);
    Serial.print(' ');
  }
  Serial.println();
  delay(1000);
}

void loop()
{
  // print the sensor values as numbers from 0 to 1000, where 0 means maximum reflectance and
  // 1000 means minimum reflectance, followed by the line position
  for (unsigned char i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(sensorValues[i]);
    Serial.print('\t');
  }

  unsigned int position = trs.readLine(sensorValues);

  volatile float k = 0.06;
  volatile float d = 0.03;

  volatile int error = position - 2000;
  volatile float pop = error * k + d*(error - prev_error);
  volatile int target = 200;

  prev_error = error;

  Serial.println(position);

  if (pop > target) pop = target;
  if (pop < - target) pop = - target;
  
  if (pop < 0)
  {
    analogWrite(RMotorSpeedPin,target + pop);
    analogWrite(LMotorSpeedPin,target);
  }
  else
  {
    analogWrite(RMotorSpeedPin,target);
    analogWrite(LMotorSpeedPin,target - pop);
  }    
  
  delay(40);
}
