/*
 ===============================================================================
 Name        : edu_ciaa_i2c_.c
 Author      : $(author)
 Version     : 0.0-alpha1
 Copyright   : $(copyright)
 Description : EDU CIAA I2C Driver.
 Note        : All translations were made using the Google translator. XD.
 ===============================================================================
 */

#include "edu_ciaa_i2c.h"

// =============================================================================
typedef struct
{
    uint8_t SCU_Port;
    uint8_t SCU_Pin;
} ciaa_i2c_gpio_t;

// Internal variables ==========================================================
static const ciaa_i2c_gpio_t I2C1_SDA = { 2, 3 };
static const ciaa_i2c_gpio_t I2C1_SCL = { 2, 4 };

static const uint8_t I2C1_SCU_MODE = (SCU_MODE_ZIF_DIS | SCU_MODE_INBUFF_EN
    | SCU_MODE_FUNC1);

// Public functions ============================================================
/**=============================================================================
 * @brief   Configure I2C
 * @param   id: Can be I2C0 or I2C1.
 * @param   mode: Member of ciaa_i2c_mode enumeration.
 * @param   freq: Clock rate - fast mode up to 400kHz.
 * @note    The slave mode has not yet been implemented.
 *============================================================================*/
void ciaa_I2C_init(I2C_ID_T id, ciaa_i2c_mode_t mode, uint32_t freq)
{
  IRQn_Type I2C_IRQn = 0;

  switch (id)
  {
    case I2C0:
      LPC_SCU->SFSI2C0 = I2C0_STANDARD_FAST_MODE;
      I2C_IRQn = I2C0_IRQn;
      break;
    case I2C1:
      LPC_SCU->SFSP[I2C1_SDA.SCU_Port][I2C1_SDA.SCU_Pin] = I2C1_SCU_MODE;
      LPC_SCU->SFSP[I2C1_SCL.SCU_Port][I2C1_SCL.SCU_Pin] = I2C1_SCU_MODE;
      I2C_IRQn = I2C1_IRQn;
      break;
  }

  Chip_I2C_Init(id);
  Chip_I2C_SetClockRate(id, freq);

  if(mode & I2C_MODE_INTERRUPT)
  {
    Chip_I2C_SetMasterEventHandler(id, Chip_I2C_EventHandler);
    NVIC_ClearPendingIRQ(I2C_IRQn);
    NVIC_SetPriority(I2C_IRQn, (0x01 << 2));
    NVIC_EnableIRQ(I2C_IRQn);
  }
  else
  {
    Chip_I2C_SetMasterEventHandler(id, Chip_I2C_EventHandlerPolling);
    NVIC_DisableIRQ(I2C_IRQn);
  }
}

/**=============================================================================
 * @brief Determine unknown addresses.
 * @param id: I2C0 or I2C1
 * @param aDevAddr  Output addresses array.
 * @return None.
 *============================================================================*/
void ciaa_I2C_ping(I2C_ID_T id, uint8_t* aDevAddr)
{
  volatile uint8_t idx = 0x09;
  const uint8_t maxAddr = 0x7F;
  uint8_t i = 0;
  uint8_t txBuff = 0x00;

  while(idx <= maxAddr)
  {
    if(Chip_I2C_MasterSend(id, idx, &txBuff, 1))
      aDevAddr[i++] = idx++;
    else
      idx++;
  }
}

/**=============================================================================
 * @brief Check device.
 * @param id: I2C0 or I2C1
 * @param aDevAddr  Device address.
 * @return None.
 *============================================================================*/
uint8_t ciaa_I2C_isReady(I2C_ID_T id, const uint8_t addr)
{
  uint8_t status = 0x00;
  uint8_t txBuff = 0x00;

  if(Chip_I2C_MasterSend(id, addr, &txBuff, 1))
      status = 0x01;

  return status;
}

// Private functions ===========================================================

// IRQ handlers ================================================================
void I2C0_IRQHandler(void)
{
  if(Chip_I2C_IsMasterActive(I2C0))
    Chip_I2C_MasterStateHandler(I2C0);
}

void I2C1_IRQHandler(void)
{
  if(Chip_I2C_IsMasterActive(I2C1))
    Chip_I2C_MasterStateHandler(I2C1);
}
