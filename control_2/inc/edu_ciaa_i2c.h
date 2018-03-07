/*
 * edu_ciaa_i2c.h
 *
 *  Created on: Dec 23, 2017
 *      Author: carlosnc
 */

#include "chip.h"

#ifndef EDU_CIAA_I2C_H_
#define EDU_CIAA_I2C_H_

// =============================================================================
typedef enum
{
  I2C_MODE_MASTER       = 0x00,
  I2C_MODE_POLLING      = 0x00,
  I2C_MODE_SLAVE        = 0x01,
  I2C_MODE_INTERRUPT    = 0x02,
} ciaa_i2c_mode_t;

// Public functions ============================================================
void    ciaa_I2C_init(I2C_ID_T id, ciaa_i2c_mode_t mode, uint32_t freq);
void    ciaa_I2C_ping(I2C_ID_T id, uint8_t* aDevAddr);
uint8_t ciaa_I2C_isReady(I2C_ID_T id, const uint8_t addr);

#endif /* EDU_CIAA_I2C_H_ */
// EOF =========================================================================
