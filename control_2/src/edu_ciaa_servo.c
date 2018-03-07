/*
 * ciaa_servo.c
 *
 *  Created on: Mar 2, 2018
 *      Author: carlosnc
 */

// =============================================================================
#include "edu_ciaa_servo.h"

// =============================================================================
static const servo_output_t *servo[3]   = {&SERVO_01, &SERVO_02, &SERVO_03};

static volatile uint32_t ticksPerCycle  = 0;
static volatile uint32_t posZero        = 0;
static volatile uint32_t servoStep      = 0;
static volatile int16_t  maxAngle       = 0;
static volatile int16_t  minAngle       = 0;

// =============================================================================
static inline void servo_configChannel(const servo_output_t* servo)
{
  uint16_t scu_mode = (SCU_MODE_INACT | SCU_MODE_FUNC1);

  Chip_SCU_PinMuxSet(servo->SCU_Port, servo->SCU_Pin, scu_mode);
  Chip_SCTPWM_SetOutPin(LPC_SCT, servo->SCT_Index, servo->GPIO_Output);
}

// =============================================================================
static uint32_t angle_to_pwm(float32_t angle);

// =============================================================================
void ciaa_servo_init(servo_initStruct_t *servo_initStruct)
{
  uint32_t maxDutyCycle = 0;
  uint32_t minDutyCycle = 0;

  maxAngle = servo_initStruct->Max_Angle;
  minAngle = servo_initStruct->Min_Angle;

  switch (servo_initStruct->Servo_Channel)
  {
    case SERVO_CHANNEL_1:
      servo_configChannel(servo[servo_initStruct->Servo_Channel]);
      break;
    case SERVO_CHANNEL_2:
      servo_configChannel(servo[servo_initStruct->Servo_Channel]);
      break;
    case SERVO_CHANNEL_3:
      servo_configChannel(servo[servo_initStruct->Servo_Channel]);
      break;
    case SERVO_CHANNEL_ALL:
      for(uint8_t i = 0; i < SERVO_CHANNEL_ALL; i++)
        servo_configChannel(servo[i]);
    default:
      break;
  }

  Chip_SCTPWM_Init(LPC_SCT);
  Chip_SCTPWM_SetRate(LPC_SCT, 50);

  uint32_t tmp = Chip_SCTPWM_PercentageToTicks(LPC_SCT, 100);
  if(servo_initStruct->Servo_Channel == SERVO_CHANNEL_ALL)
  {
    for(uint8_t i = 0; i < SERVO_CHANNEL_ALL; i++)
      Chip_SCTPWM_SetDutyCycle(LPC_SCT, servo[i]->SCT_Index, tmp);
  }
  else
    Chip_SCTPWM_SetDutyCycle(LPC_SCT, servo[servo_initStruct->Servo_Channel]->SCT_Index, tmp);

  if(servo_initStruct->Servo_PWM_Polarity == SERVO_PWM_POLARITY_HIGH)
  {
    maxDutyCycle = Chip_SCTPWM_PercentageToTicks(LPC_SCT, 12);
    minDutyCycle = Chip_SCTPWM_PercentageToTicks(LPC_SCT, 3);
    ticksPerCycle = maxDutyCycle - minDutyCycle;
    servoStep = ticksPerCycle/180;
  }
  else
  {
    maxDutyCycle = Chip_SCTPWM_PercentageToTicks(LPC_SCT, 97);
    minDutyCycle = Chip_SCTPWM_PercentageToTicks(LPC_SCT, 88);
    ticksPerCycle = maxDutyCycle - minDutyCycle;
    servoStep = ticksPerCycle/180;
  }

  int16_t tmpPosZero = servo_initStruct->Pos_Zero;
  posZero = (tmpPosZero >= 0)?(minDutyCycle + servoStep*tmpPosZero):(maxDutyCycle - servoStep*tmpPosZero);
}

void ciaa_servo_check(servo_channel_t servo_channel)
{
  uint8_t tmpStep = (maxAngle - minAngle)/4;
  float32_t tmpAngle = (float32_t)minAngle;

  ciaa_servo_zeroPosition(servo_channel);

  if(servo_channel == SERVO_CHANNEL_ALL)
  {
    for (uint8_t i = 0; i < SERVO_CHANNEL_ALL; i++)
    {
      for(uint8_t u = 0; u < 5; u++)
      {
        ciaa_servo_updatePosition((servo_channel_t)i, tmpAngle);
        tmpAngle += tmpStep;
        pauseMs(500);
      }
    }
  }
  else
  {
    for (uint8_t i = 0; i < 5; i++)
    {
      ciaa_servo_updatePosition(servo_channel, tmpAngle);
      tmpAngle += tmpStep;
      pauseMs(500);
    }
  }

  ciaa_servo_zeroPosition(servo_channel);
  pauseMs(500);
}

void ciaa_servo_zeroPosition(servo_channel_t servo_channel)
{
  if(servo_channel == SERVO_CHANNEL_ALL)
  {
    for(uint8_t i = 0; i < SERVO_CHANNEL_ALL; i++)
      ciaa_servo_updatePosition((servo_channel_t)i, 0.0f);
  }
  else
  {
    ciaa_servo_updatePosition(servo_channel, 0.0f);
  }
  pauseMs(1000);
}

void ciaa_servo_updatePosition(servo_channel_t servo_channel, float32_t angle)
{
  uint32_t servo_pos = 0;

  servo_pos = angle_to_pwm(angle);

  switch (servo_channel)
  {
    case SERVO_CHANNEL_1:
      Chip_SCTPWM_SetDutyCycle(LPC_SCT, servo[SERVO_CHANNEL_1]->SCT_Index, servo_pos);
      break;
    case SERVO_CHANNEL_2:
      Chip_SCTPWM_SetDutyCycle(LPC_SCT, servo[SERVO_CHANNEL_2]->SCT_Index, servo_pos);
      break;
    case SERVO_CHANNEL_3:
      Chip_SCTPWM_SetDutyCycle(LPC_SCT, servo[SERVO_CHANNEL_3]->SCT_Index, servo_pos);
      break;
    case SERVO_CHANNEL_ALL:
      for (uint8_t i = 0; i < SERVO_CHANNEL_ALL; i++)
        Chip_SCTPWM_SetDutyCycle(LPC_SCT, servo[i]->SCT_Index, servo_pos);
      break;
    default:
      break;
  }
}

// =============================================================================
static uint32_t angle_to_pwm(float32_t angle)
{
  float32_t int_degree = 0;
  uint32_t  pwm_pos    = 0;

  if (angle < 0)
    int_degree = angle;

  if (angle > 0)
    int_degree = angle;

  int_degree = (int_degree > maxAngle) ? (maxAngle) : (int_degree);
  int_degree = (int_degree < minAngle) ? (minAngle) : (int_degree);

  pwm_pos = (uint32_t)(posZero + (servoStep * int_degree));

  return pwm_pos;
}
