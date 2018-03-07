
#include "inithardware.h"
#include "arm_math.h"

// =============================================================================
float32_t aInputAngles[3] = { 0.0f };
float32_t *pInputAngles = &aInputAngles[0];

float32_t aAbsInputAngles[3] = { 0.0f };
float32_t *pAbsInputAngles = &aAbsInputAngles[0];

float32_t aPastInputAngles[3] = { 0.0f };
float32_t *pPastInputAngles = &aPastInputAngles[0];

// Cycles counter
volatile uint32_t * DWT_CTRL   = (uint32_t *)0xE0001000;
volatile uint32_t * DWT_CYCCNT = (uint32_t *)0xE0001004;

// =============================================================================
static void updateData(void);

// =============================================================================
int main(void)
{
  uint32_t timerFreq = 0;

  initHardware_Init();
  initHardware_testOutputs();

  Chip_TIMER_Init(LPC_TIMER1);
  Chip_RGU_TriggerReset(RGU_TIMER1_RST);

  while(Chip_RGU_InReset(RGU_TIMER1_RST) == 1)
    __NOP();

  timerFreq = Chip_Clock_GetRate(CLK_MX_TIMER1);

  ciaa_servo_start();
  ciaa_servo_zeroPosition(SERVO_CHANNEL_ALL);

  Chip_TIMER_Reset(LPC_TIMER1);
  Chip_TIMER_MatchEnableInt(LPC_TIMER1, 1);
  Chip_TIMER_SetMatch(LPC_TIMER1, 1, timerFreq/300);
  Chip_TIMER_ResetOnMatchEnable(LPC_TIMER1, 1);
  Chip_TIMER_Enable(LPC_TIMER1);

  NVIC_EnableIRQ(TIMER1_IRQn);
  NVIC_ClearPendingIRQ(TIMER1_IRQn);

  while(1)
    __WFI();

  return 0;
}

// =============================================================================
static void updateData(void)
{
  arm_abs_f32(pInputAngles, pAbsInputAngles, 3);

  if(ciaa_estimator_filteredAngles(pInputAngles) == FILTER_OK)
  {
    while((pAbsInputAngles[0] > 0.0f) || (pAbsInputAngles[1] > 0.0f))
    {
      ciaa_servo_updatePosition(SERVO_CHANNEL_1, pInputAngles[0]--);
      ciaa_servo_updatePosition(SERVO_CHANNEL_2, pInputAngles[0]++);
      ciaa_servo_updatePosition(SERVO_CHANNEL_3, pInputAngles[1]);
    }
  }

  ciaa_uart_sendData(pInputAngles, 3, (UART_DATA_LOG | UART_DATA_FORMAT_TAB));
}

// =============================================================================
void TIMER1_IRQHandler(void)
{
  if(Chip_TIMER_MatchPending(LPC_TIMER1, 1) == 1)
  {
    Chip_TIMER_ClearMatch(LPC_TIMER1, 1);
    ciaa_board_togglePin(&LED_Verde);
    updateData();
  }
}
