/*
===============================================================================
Name        : inithardware.c
Author      : $(author)
Version     : 0.0-alpha1
Copyright   : $(copyright)
Description : Start sequence for EDU-CIAA.
Note        : All translations were made using the Google translator. XD.
===============================================================================
*/

#include "inithardware.h"

// Private functions prototypes ================================================
static void initSystemClock(void);
static void initLEDs(void);
static void initButtons(void);
static void initPeripherals_and_Sensor(void);

// Public functions ============================================================
void initHardware_Init(void)
{
 initSystemClock();
 SystemCoreClockUpdate();                              //System clock 208 MHz

 SysTick_Config(SystemCoreClock / 1000u);              //1ms SysTick
 NVIC_SetPriority(SysTick_IRQn, ((0x01 << 0) - 0x01));

 initLEDs();
 initButtons();
 initPeripherals_and_Sensor();
}

void initHardware_testOutputs(void)
{
 uint8_t idx = 0;
 const ciaa_gpio_t *Leds[6] = { &LED_RGB_Rojo, &LED_RGB_Verde, &LED_RGB_Azul,
     &LED_Rojo, &LED_Amarillo, &LED_Verde };

 for (uint8_t i = 0; i < 12; i++)
 {
   ciaa_board_togglePin(Leds[idx]);
   idx = (i == 5) ? (0) : (idx + 1);
   pauseMs(200);
 }
}

// Private functions ===========================================================
static void initSystemClock(void)
{
 __NOP();
}

static void initLEDs(void)
{
 ciaa_board_initLED(&LED_RGB_Rojo);
 ciaa_board_initLED(&LED_RGB_Verde);
ciaa_board_initLED(&LED_RGB_Azul);

ciaa_board_setPinLow(&LED_RGB_Rojo);
ciaa_board_setPinLow(&LED_RGB_Verde);
ciaa_board_setPinLow(&LED_RGB_Azul);

ciaa_board_initLED(&LED_Rojo);
ciaa_board_initLED(&LED_Amarillo);
ciaa_board_initLED(&LED_Verde);

ciaa_board_setPinLow(&LED_Rojo);
ciaa_board_setPinLow(&LED_Amarillo);
ciaa_board_setPinLow(&LED_Verde);
}

static void initButtons(void)
{
 ciaa_board_initTec(&TEC01, CIAA_TEC_INTERRUPT);
}

static void initPeripherals_and_Sensor(void)
{
  ciaa_I2C_init(I2C0, (I2C_MODE_MASTER | I2C_MODE_POLLING), 400000);

  ciaa_uart_init_t ciaa_UART_initStruct;
  ciaa_UART_initStruct.ID = CIAA_UART3;
  ciaa_UART_initStruct.BaudRate = 115200;
  ciaa_UART_initStruct.Mode = UART_MODE_NORMAL;
  ciaa_UART_initStruct.DataConfig = UART_CONFIG_8N1;
  ciaa_uart_init(&ciaa_UART_initStruct);

  mpu9250_InitStruct_t mpu9250_InitStruct;
  mpu9250_InitStruct.Accel_Axes = MPU9250_ACCEL_XYZ_ENABLE;
  mpu9250_InitStruct.Accel_Scale = MPU9250_ACCEL_FULLSCALE_2G;
  mpu9250_InitStruct.Accel_LPF = MPU9250_ACCEL_LPF_5_05HZ;
  mpu9250_InitStruct.Gyro_Axes = MPU9250_GYRO_XYZ_ENABLE;
  mpu9250_InitStruct.Gyro_Scale = MPU9250_GYRO_FULLSCALE_250DPS;
  mpu9250_InitStruct.Gyro_LPF = MPU9250_GYRO_LPF_5HZ;
  mpu9250_InitStruct.SampleRate = 100;
  mpu9250_init(&mpu9250_InitStruct);

  filter_init_t filter_InitStruct;
  filter_InitStruct.SampleRate = 300;
  filter_InitStruct.Weight     = 0.95f;
  ciaa_estimator_init(&filter_InitStruct);

  servo_initStruct_t servo_InitStruct;
  servo_InitStruct.Max_Angle = 90;
  servo_InitStruct.Min_Angle = -90;
  servo_InitStruct.Pos_Zero = 90;
  servo_InitStruct.Servo_Channel = SERVO_CHANNEL_ALL;
  servo_InitStruct.Servo_PWM_Polarity = SERVO_PWM_POLARITY_LOW;
  ciaa_servo_init(&servo_InitStruct);
}
