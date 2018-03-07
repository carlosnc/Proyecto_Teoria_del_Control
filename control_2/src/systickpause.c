/*
 ===============================================================================
 Name        : hd44780.c
 Author      : $(author)
 Version     : 0.0-alpha1
 Copyright   : $(copyright)
 Description : Library for systick delay.
 Note        : All translations were made using the Google translator. XD.
 ===============================================================================
 */

// Includes ====================================================================
#include "systickpause.h"

// Variables ===================================================================
static volatile uint32_t pausems_count = 0u;

// Public functions ============================================================
void pauseMs(__IO uint32_t mseconds)
{
  pausems_count = mseconds;

  while(pausems_count != 0)
    __WFI();
}

// Systick IRQ handler =========================================================
void SysTick_Handler(void)
{
  if(pausems_count > 0)
    pausems_count--;
}

// EOF =========================================================================
