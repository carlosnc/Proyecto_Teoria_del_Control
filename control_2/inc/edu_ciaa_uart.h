/*
 * edu_ciaa_uart.h
 *
 *  Created on: Jan 23, 2018
 *      Author: carlosnc
 */

#include "chip.h"

#ifndef EDU_CIAA_UART_H_
#define EDU_CIAA_UART_H_

// =============================================================================
typedef uint8_t bash_cmd_t;
typedef float float32_t;

typedef enum
{
  CIAA_UART0 = 0,
  CIAA_UART2 = 2,
  CIAA_UART3,
} ciaa_uart_id_t;

typedef enum
{
  UART_MODE_NORMAL = 0,
  UART_MODE_INTERRUPT,

} ciaa_uart_mode_t;

typedef enum
{
  UART_DATA_DISPLAY = 0,
  UART_DATA_LOG = (0x01 << 0),
  UART_DATA_FORMAT_TAB = (0x01 << 1),
  UART_DATA_FORMAT_SPACE = (0x01 << 2),
  UART_DATA_FORMAT_LF = (0x01 << 3),
  UART_DATA_FORMAT_BS = (0x01 << 4),
} ciaa_uart_data_format_t;

typedef enum
{
  UART_CONFIG_7N1 = (UART_LCR_WLEN7 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_DIS),
  UART_CONFIG_7E1 = (UART_LCR_WLEN7 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_EVEN),
  UART_CONFIG_7O1 = (UART_LCR_WLEN7 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_ODD),

  UART_CONFIG_8N1 = (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_DIS),
  UART_CONFIG_8E1 = (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_EVEN),
  UART_CONFIG_8O1 = (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_ODD),
} ciaa_uart_config_t;

typedef struct
{
    ciaa_uart_id_t ID;
    ciaa_uart_mode_t Mode;
    ciaa_uart_config_t DataConfig;
    uint32_t BaudRate;
} ciaa_uart_init_t;

// Linux bash sequences ========================================================
static const bash_cmd_t bash_ClearScreen[] = "\033[2J";
static const bash_cmd_t bash_eraseLine[]   = "\033[k";
static const bash_cmd_t bash_cursor2Home[] = "\033[0;0H";
static const bash_cmd_t bash_Normal[]      = "\033[0m";
static const bash_cmd_t bash_Black[]       = "\033[0;30m";
static const bash_cmd_t bash_darkGray[]    = "\033[1;30m";
static const bash_cmd_t bash_lightGray[]   = "\033[0;37m";
static const bash_cmd_t bash_White[]       = "\033[1;37m";
static const bash_cmd_t bash_Red[]         = "\033[0;31m";
static const bash_cmd_t bash_lightRed[]    = "\033[1;31m";
static const bash_cmd_t bash_Green[]       = "\033[0;32m";
static const bash_cmd_t bash_lightGreen[]  = "\033[1;32m";
static const bash_cmd_t bash_Blue[]        = "\033[0;34m";
static const bash_cmd_t bash_lightBlue[]   = "\033[1;34m";
static const bash_cmd_t bash_Purple[]      = "\033[0;35m";
static const bash_cmd_t bash_lightPurple[] = "\033[1;35m";
static const bash_cmd_t bash_Cyan[]        = "\033[0;36m";
static const bash_cmd_t bash_lightCyan[]   = "\033[1;36m";
static const bash_cmd_t bash_Brown[]       = "\033[0;33m";
static const bash_cmd_t bash_Yellow[]      = "\033[1;33m";

// Public functions ============================================================
void ciaa_uart_init(ciaa_uart_init_t *UART_initStruct);
void ciaa_uart_send2Bash(const bash_cmd_t *bash_cmd, const uint8_t *string);
void ciaa_uart_putString(const uint8_t *string, uint8_t len);
// ToDo: implement.
void ciaa_uart_sendData(float32_t *pData, uint8_t vector_len, ciaa_uart_data_format_t mode);

#endif /* EDU_CIAA_UART_H_ */
// EOF =========================================================================
