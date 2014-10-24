#include <EEPROM.h>
#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"

#define READING_WINDOWSIZE 200
const float defaultOffsetRoll = 0;
const float defaultOffsetPitch = 0;
const float defaultSendDelay = 200;

int acc_x_buf[READING_WINDOWSIZE];
int acc_y_buf[READING_WINDOWSIZE];
int acc_z_buf[READING_WINDOWSIZE];

int writeptr = 0;
boolean avgDirty = false;

long int acc_x_sum = 0;
long int acc_y_sum = 0;
long int acc_z_sum = 0;

float acc_x_avg = 0;
float acc_y_avg = 0;
float acc_z_avg = 0;

const int defaultAAvg = 30;
const int defaultGAvg = 30;
const int defaultTAvg = 30;
int aAvg = defaultAAvg;
int gAvg = defaultGAvg;
int tAvg = defaultTAvg;
double offsetRoll = defaultOffsetRoll;
double offsetPitch = defaultOffsetPitch;

MPU6050 mpu;

void setupGyros() 
{
  mpu.initialize();
  for(int i=0; i<READING_WINDOWSIZE; i++)
  {
    acc_x_buf[READING_WINDOWSIZE] = 0;
    acc_y_buf[READING_WINDOWSIZE] = 0;
    acc_z_buf[READING_WINDOWSIZE] = 0;
  }
}

void updateAverages()
{
  //return if old averages are still valid
  if(!avgDirty) return;
  
  //calculate averages
  acc_x_avg = ((float)acc_x_sum) / ((float)READING_WINDOWSIZE);
  acc_y_avg = ((float)acc_y_sum) / ((float)READING_WINDOWSIZE);
  acc_z_avg = ((float)acc_z_sum) / ((float)READING_WINDOWSIZE);
  
  //clear dirty flag
  avgDirty = false;
}

void ReadGyrosAverage() 
{
  //update sums (remove oldest value)
  acc_x_sum -= acc_x_buf[writeptr];
  acc_y_sum -= acc_y_buf[writeptr];
  acc_z_sum -= acc_z_buf[writeptr];
  
  //load new values to buffer
  mpu.getAcceleration(&acc_x_buf[writeptr], &acc_y_buf[writeptr], &acc_z_buf[writeptr]);
  
  //update sums (add new value)
  acc_x_sum += acc_x_buf[writeptr];
  acc_y_sum += acc_y_buf[writeptr];
  acc_z_sum += acc_z_buf[writeptr];
  
  //update writeptr
  if(++writeptr >= READING_WINDOWSIZE) writeptr = 0;
  
  //set dirty flag
  avgDirty = true;
}

float GetGyroRollRad()
{ 
  updateAverages();
  return atan2(acc_x_avg, sqrt(acc_y_avg * acc_y_avg + acc_z_avg * acc_z_avg)) - offsetRoll;
}

float GetGyroPitchRad() 
{
  updateAverages();
  return atan2(-acc_y_avg, acc_z_avg) - offsetPitch;
}
