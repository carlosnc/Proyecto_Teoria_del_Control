
#include "edu_ciaa_uart.h"
#include "stdlib.h"
#include "string.h"

#pragma GCC diagnostic ignored "-Wpointer-sign";
#pragma GCC diagnostic ignored "-Wincompatible-pointer-types";

// =============================================================================
typedef struct
{
    uint8_t SCU_Port;
    uint8_t SCU_Pin;
} ciaa_uart_gpio_t;

// Private variables and constants =============================================
static LPC_USART_T *idUART = 0;
static IRQn_Type UART_IRQn = 0x00;
static const uint8_t BASH_SIZE = 80;

static const ciaa_uart_gpio_t UART0_TX = { 9, 5 };
static const ciaa_uart_gpio_t UART0_RX = { 9, 6 };
static const uint16_t SCU_UART0_TX_MODE = (SCU_MODE_INACT | SCU_MODE_FUNC7);
static const uint16_t SCU_UART0_RX_MODE = (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC7);

static const ciaa_uart_gpio_t UART2_TX = { 1, 15 };
static const ciaa_uart_gpio_t UART2_RX = { 1, 16 };
static const uint16_t SCU_UART2_TX_MODE = (SCU_MODE_INACT | SCU_MODE_FUNC1);
static const uint16_t SCU_UART2_RX_MODE = (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC1);

static const ciaa_uart_gpio_t UART3_TX = { 2, 3 };
static const ciaa_uart_gpio_t UART3_RX = { 2, 4 };
static const uint16_t SCU_UART3_TX_MODE = (SCU_MODE_INACT | SCU_MODE_FUNC2);
static const uint16_t SCU_UART3_RX_MODE = (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC2);

// =============================================================================
static inline void ciaa_uart_putChar(uint8_t c)
{
  while(!(Chip_UART_ReadLineStatus(idUART) & UART_LSR_TEMT))
    __NOP();

  Chip_UART_SendByte(idUART, c);
}

static inline uint8_t ciaa_uart_decimal_to_Int(float32_t x)
{
  return (uint8_t)((x - (int16_t)x)*100);
}

// Public functions ============================================================
void ciaa_uart_init(ciaa_uart_init_t *UART_initStruct)
{
  switch (UART_initStruct->ID)
  {
    case CIAA_UART0:
      idUART = LPC_USART0;
      Chip_SCU_PinMuxSet(UART0_TX.SCU_Port, UART0_TX.SCU_Pin, SCU_UART0_TX_MODE);
      Chip_SCU_PinMuxSet(UART0_RX.SCU_Port, UART0_RX.SCU_Pin, SCU_UART0_RX_MODE);

      Chip_UART_Init(idUART);
      Chip_UART_SetBaud(idUART, UART_initStruct->BaudRate);
      Chip_UART_ConfigData(idUART, UART_initStruct->DataConfig);
      Chip_UART_TXEnable(idUART);

      UART_IRQn = USART0_IRQn;

      break;

    case CIAA_UART2:
      idUART = LPC_USART2;
      Chip_SCU_PinMuxSet(UART2_TX.SCU_Port, UART2_TX.SCU_Pin, SCU_UART2_TX_MODE);
      Chip_SCU_PinMuxSet(UART2_RX.SCU_Port, UART2_RX.SCU_Pin, SCU_UART2_RX_MODE);

      Chip_UART_Init(idUART);
      Chip_UART_SetBaud(idUART, UART_initStruct->BaudRate);
      Chip_UART_ConfigData(idUART, UART_initStruct->DataConfig);
      Chip_UART_TXEnable(idUART);

      UART_IRQn = USART2_IRQn;
      break;

    default:
      idUART = LPC_USART3;
      Chip_SCU_PinMuxSet(UART3_TX.SCU_Port, UART3_TX.SCU_Pin, SCU_UART3_TX_MODE);
      Chip_SCU_PinMuxSet(UART3_RX.SCU_Port, UART3_RX.SCU_Pin, SCU_UART3_RX_MODE);

      Chip_UART_Init(idUART);
      Chip_UART_SetBaud(idUART, UART_initStruct->BaudRate);
      Chip_UART_ConfigData(idUART, UART_initStruct->DataConfig);
      Chip_UART_TXEnable(idUART);

      UART_IRQn = USART3_IRQn;
      break;
  }

  if(UART_initStruct->Mode)
  {
    /**
     * RBRINT - Enables the Receive Data Available interrupt for USART.
     * RLSINT - Enables the USART RX line status interrupts.
     */
    Chip_UART_IntEnable(idUART, (UART_IER_RBRINT | UART_IER_RLSINT));
    NVIC_ClearPendingIRQ(UART_IRQn);
    NVIC_SetPriority(UART_IRQn, (0x01 << 3) | 0x01);
    NVIC_EnableIRQ(UART_IRQn);
  }
}

void ciaa_uart_send2Bash(const bash_cmd_t *bash_cmd, const uint8_t *string)
{
  uint8_t tmp[BASH_SIZE];
  uint8_t *ptmp = &tmp[0];

  memset(tmp, '\0', 80);
  strncat(ptmp, (uint8_t *)bash_cmd, BASH_SIZE);
  strncat(ptmp, (uint8_t *)string, BASH_SIZE);
  strncat(ptmp, (uint8_t *)bash_Normal, BASH_SIZE);

  ciaa_uart_putString(ptmp, BASH_SIZE);
}

void ciaa_uart_putString(const uint8_t *string, uint8_t len)
{
  uint8_t idx = len;

  while((*string != '\0') && (idx))
  {
    ciaa_uart_putChar(*string++);
    idx--;
  }
}

void ciaa_uart_sendData(float32_t *pData, uint8_t vector_len, ciaa_uart_data_format_t mode)
{
  uint8_t sTmp[10];
  uint8_t sSeparator;
  uint8_t *pSeparator = &sSeparator;
  uint8_t decimal = 0;

  switch (mode & ~0x0001)
  {
    case UART_DATA_FORMAT_LF:
      pSeparator = (uint8_t *)"\n";
      break;
    case UART_DATA_FORMAT_SPACE:
      pSeparator = (uint8_t *)" ";
      break;
    case UART_DATA_FORMAT_TAB:
      pSeparator = (uint8_t *)"\t";
      break;
    case UART_DATA_FORMAT_BS:
      pSeparator = (uint8_t *)"/";
      break;
    default:
      break;
  }

  if( mode & UART_DATA_LOG )
  {
    for (uint8_t i = 0; i < vector_len; i++)
    {
      memset(sTmp, '\0', 10);
      itoa((int16_t) pData[i], &sTmp, 10);
      ciaa_uart_putString(sTmp, 10);
      ciaa_uart_putString((uint8_t *) ".", 1);
      decimal = ciaa_uart_decimal_to_Int(pData[i]);
      itoa(decimal, &sTmp, 10);
      ciaa_uart_putString(sTmp, 10);
      ciaa_uart_putString(pSeparator, 1);
    }
    if( pSeparator != (uint8_t *)"\n" )
      ciaa_uart_putString((uint8_t *) "\n\r", 2);
  }
  else
  {
    if(pSeparator == (uint8_t *)"\n")
      pSeparator = (uint8_t *)"\t";

    ciaa_uart_send2Bash(bash_eraseLine, (uint8_t *)"\r");

    for (uint8_t i = 0; i < vector_len; i++)
    {
      memset(sTmp, '\0', 10);
      itoa((int16_t) pData[i], &sTmp, 10);

      if(pData[i] >= 0.0f)
      {
        ciaa_uart_send2Bash(bash_Green, sTmp);
        decimal = ciaa_uart_decimal_to_Int(pData[i]);
        itoa(decimal, &sTmp, 10);
        ciaa_uart_putString((uint8_t *) ".", 1);
        ciaa_uart_send2Bash(bash_Green, sTmp);
      }
      else
      {
        ciaa_uart_send2Bash(bash_Red, sTmp);
        decimal = ciaa_uart_decimal_to_Int(pData[i]);
        itoa(decimal, &sTmp, 10);
        ciaa_uart_putString((uint8_t *) ".", 1);
        ciaa_uart_send2Bash(bash_Red, sTmp);
      }
      if(i < (vector_len - 1))
        ciaa_uart_putString(pSeparator, 1);
    }
  }
}

// EOF =========================================================================
