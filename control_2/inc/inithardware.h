/*
 ===============================================================================
 Name        : inithardware.c
 Author      : $(author)
 Version     : 0.0-alpha1
 Copyright   : $(copyright)
 Description : Definitions for EDU-CIAA.
 Note        : All translations were made using the Google translator. XD.
 ===============================================================================
 */

#ifndef INITHARDWARE_H_
#define INITHARDWARE_H_

#include "chip.h"
#include "systickpause.h"

#include "edu_ciaa_servo.h"
#include "edu_ciaa_i2c.h"
#include "edu_ciaa_uart.h"
#include "edu_ciaa_estimador.h"

#include "mpu9250.h"

// =============================================================================
typedef float   float32_t;
typedef double  float64_t;

// =============================================================================
typedef enum
{
  APP_STOPPED = 0x00,
  APP_RUNNING,
} ciaa_app_state_t;

typedef enum
{
  CIAA_TEC_POLLING = 0,
  CIAA_TEC_INTERRUPT,
} ciaa_tec_mode_t;

typedef struct
{
  uint8_t SCU_Port;
  uint8_t SCU_Pin;
  uint8_t GPIO_Port;
  uint8_t GPIO_Pin;
} ciaa_gpio_t;

// =============================================================================
static const ciaa_gpio_t LED_RGB_Rojo    = {2, 0, 5, 0};
static const ciaa_gpio_t LED_RGB_Verde   = {2, 1, 5, 1};
static const ciaa_gpio_t LED_RGB_Azul    = {2, 2, 5, 2};

static const ciaa_gpio_t LED_Rojo        = {2, 10, 0, 14};
static const ciaa_gpio_t LED_Amarillo    = {2, 11, 1, 11};
static const ciaa_gpio_t LED_Verde       = {2, 12, 1, 12};

static const ciaa_gpio_t TEC01 = {1, 0, 0, 4};
static const ciaa_gpio_t TEC02 = {1, 1, 0, 8};
static const ciaa_gpio_t TEC03 = {1, 2, 0, 9};
static const ciaa_gpio_t TEC04 = {1, 6, 1, 9};

static const ciaa_gpio_t MPU_INT_PIN = {7, 4, 3, 12};

// =============================================================================
static  inline uint8_t ciaa_board_readPinValue(const ciaa_gpio_t* TEC)
{
  return LPC_GPIO_PORT->B[TEC->GPIO_Port][TEC->GPIO_Pin];
}

static inline void ciaa_board_setPinHigh(const ciaa_gpio_t* pin)
{
  LPC_GPIO_PORT->SET[pin->GPIO_Port] = (1 << pin->GPIO_Pin);
}

static inline void ciaa_board_setPinLow(const ciaa_gpio_t* pin)
{
  LPC_GPIO_PORT->CLR[pin->GPIO_Port] = (1 << pin->GPIO_Pin);
}

static inline void ciaa_board_togglePin(const ciaa_gpio_t* pin)
{
  LPC_GPIO_PORT->NOT[pin->GPIO_Port] = (1 << pin->GPIO_Pin);
}

static inline void ciaa_board_blinkPin(const ciaa_gpio_t* pin)
{
  ciaa_board_togglePin(pin);
  pauseMs(500);
  ciaa_board_togglePin(pin);
}

static inline void ciaa_board_initLED(const ciaa_gpio_t* LED)
{
  uint16_t scu_mode = (SCU_MODE_INACT | SCU_MODE_INBUFF_EN);

  (LED->GPIO_Port == 5) ? (scu_mode |= SCU_MODE_FUNC4) : (scu_mode |= SCU_MODE_FUNC0);

  LPC_SCU->SFSP[LED->SCU_Port][LED->SCU_Pin] = scu_mode;
  LPC_GPIO_PORT->DIR[LED->GPIO_Port] |= (1 << LED->GPIO_Pin);
  LPC_GPIO_PORT->CLR[LED->GPIO_Port] = (1 << LED->GPIO_Pin);
}

static inline void ciaa_board_initTec(const ciaa_gpio_t* TEC, ciaa_tec_mode_t Mode)
{
  uint16_t scu_mode = (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_FUNC0);
  IRQn_Type TEC_IRQn = 0;

  LPC_SCU->SFSP[TEC->SCU_Port][TEC->SCU_Pin] = scu_mode;
  LPC_GPIO_PORT->DIR[TEC->GPIO_Port] &= ~(1 << TEC->GPIO_Pin);

  if (Mode)
  {
    Chip_SCU_GPIOIntPinSel(TEC->GPIO_Port, TEC->GPIO_Port, TEC->GPIO_Pin);
    Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(TEC->GPIO_Port));
    Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(TEC->GPIO_Port));
    Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(TEC->GPIO_Port));

    (TEC->GPIO_Port == 0) ? (TEC_IRQn = PIN_INT0_IRQn) : (TEC_IRQn = PIN_INT1_IRQn);

    NVIC_ClearPendingIRQ(TEC_IRQn);
    NVIC_SetPriority(TEC_IRQn, (0x01 << 0));
    NVIC_EnableIRQ(TEC_IRQn);
  }
}

static inline void ciaa_board_initInterrupt(const ciaa_gpio_t* GPIO, uint16_t scu_fun)
{
  uint16_t scu_mode = (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | scu_fun);

  LPC_SCU->SFSP[GPIO->SCU_Port][GPIO->SCU_Pin] = scu_mode;
  LPC_GPIO_PORT->DIR[GPIO->GPIO_Port] &= ~(1 << GPIO->GPIO_Pin);

  Chip_SCU_GPIOIntPinSel(GPIO->GPIO_Port, GPIO->GPIO_Port, GPIO->GPIO_Pin);
  Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(GPIO->GPIO_Port));
  Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(GPIO->GPIO_Port));
  Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(GPIO->GPIO_Port));

  NVIC_ClearPendingIRQ(PIN_INT3_IRQn);
  NVIC_SetPriority(PIN_INT3_IRQn, (0x01 << 1));
  NVIC_EnableIRQ(PIN_INT3_IRQn);
}

static inline void ciaa_board_deInitInterrupt(const ciaa_gpio_t* GPIO, uint16_t scu_fun)
{
  (void)(GPIO);
  (void)(scu_fun);

  NVIC_DisableIRQ(PIN_INT3_IRQn);
}

// =============================================================================
void initHardware_Init(void);
void initHardware_testOutputs(void);

#endif /* INITHARDWARE_H_ */
// EOF =========================================================================
