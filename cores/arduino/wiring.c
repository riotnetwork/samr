/*
  Copyright (c) 2015 Arduino LLC.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "Arduino.h"

#define VERY_LOW_POWER

#ifdef __cplusplus
extern "C" {
#endif

/*
 * System Core Clock is at 1MHz (8MHz/8) at Reset.
 * It is switched to 48MHz in the Reset Handler (startup.c)
 */
uint32_t SystemCoreClock=1000000ul ;

/*
void calibrateADC()
{
  volatile uint32_t valeur = 0;

  for(int i = 0; i < 5; ++i)
  {
    ADC->SWTRIG.bit.START = 1;
    while( ADC->INTFLAG.bit.RESRDY == 0 || ADC->STATUS.bit.SYNCBUSY == 1 )
    {
      // Waiting for a complete conversion and complete synchronization
    }

    valeur += ADC->RESULT.bit.RESULT;
  }

  valeur = valeur/5;
}*/

/*
 * Riot Stamp board initialization
 *
 * Good to know:
 *   - At reset, ResetHandler did the system clock configuration. Core is running at 48MHz.
 *   - Watchdog is disabled by default, unless someone plays with NVM User page
 *   - During reset, all PORT lines are configured as inputs with input buffers, output buffers and pull disabled.
 */
void init( void )
{
  // Set Systick to 1ms interval, common to all Cortex-M variants
  if ( SysTick_Config( SystemCoreClock / 1000 ) )
  {
    // Capture error
    while ( 1 ) ;
  }
  NVIC_SetPriority (SysTick_IRQn,  (1 << __NVIC_PRIO_BITS) - 2);  /* set Priority for Systick Interrupt (2nd lowest) */

  // Clock PORT for Digital I/O
//  PM->APBBMASK.reg |= PM_APBBMASK_PORT ;
//
//  // Clock EIC for I/O interrupts
//  PM->APBAMASK.reg |= PM_APBAMASK_EIC ;

  // Clock SERCOM0-4 for Serial
  MCLK->APBCMASK.reg |= MCLK_APBCMASK_SERCOM0 | MCLK_APBCMASK_SERCOM1 | MCLK_APBCMASK_SERCOM2 | MCLK_APBCMASK_SERCOM3 | MCLK_APBCMASK_SERCOM4;

  // Clock TC/TCC for Pulse and Analog
  MCLK->APBCMASK.reg |= MCLK_APBCMASK_TCC0 | MCLK_APBCMASK_TCC1 | MCLK_APBCMASK_TCC2 | MCLK_APBCMASK_TC0 | MCLK_APBCMASK_TC1 | MCLK_APBCMASK_TC2 | MCLK_APBCMASK_TC3 |MCLK_APBCMASK_DAC;

  // Clock TC4,  ADC/DAC for Analog
  MCLK->APBDMASK.reg |= MCLK_APBDMASK_TC4| MCLK_APBDMASK_ADC | MCLK_APBDMASK_SERCOM5 ; // AC, CCL and EVSYS remain off

// Defining VERY_LOW_POWER breaks Arduino APIs since all pins are considered INPUT at startup
// However, it really lowers the power consumption by a factor of 20 in low power mode (0.03mA vs 0.6mA)
#ifndef VERY_LOW_POWER
  // Setup all pins (digital and analog) in INPUT mode (default is nothing)
  for (uint32_t ul = 0 ; ul < NUM_DIGITAL_PINS ; ul++ )
  {
    pinMode( ul, INPUT ) ;
  }
#endif

  // Initialize Analog Controller
  // Setting clock Generic clock 0 as source
	GCLK->PCHCTRL[GCM_ADC].reg = ( GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK0 );
	while ( (GCLK->PCHCTRL[GCM_ADC].reg & GCLK_PCHCTRL_CHEN) == 0 ){
		// wait for sync
	}
	ADC->CTRLA.bit.ENABLE = 0;
	
  while( ADC->SYNCBUSY.bit.ENABLE == 1 );          // Wait for synchronization of registers between the clock domains

  ADC->CTRLB.reg = ADC_CTRLB_PRESCALER_DIV256;    // Divide Clock by 512.
  ADC->CTRLC.reg = ADC_CTRLC_RESSEL_10BIT;			 // 10 bits resolution as default

  ADC->SAMPCTRL.reg = 0x3f;                        // Set max Sampling Time Length

  while(  ADC->SYNCBUSY.bit.SAMPCTRL == 1 );          // Wait for synchronization of registers between the clock domains

  ADC->INPUTCTRL.reg = ADC_INPUTCTRL_MUXNEG_GND;   // No Negative input (Internal Ground)

  // Averaging (see datasheet table in AVGCTRL register description)
  ADC->AVGCTRL.reg = ADC_AVGCTRL_SAMPLENUM_1 |    // 1 sample only (no oversampling nor averaging)
                     ADC_AVGCTRL_ADJRES(0x0ul);   // Adjusting result by 0

  analogReference( AR_INTERNAL2V00 ) ; // Analog Reference is internal 2V reference (see wiring analog)

  // Initialize DAC
  // Setting clock
// Setting clock Generic clock 0 as source
	GCLK->PCHCTRL[GCM_DAC].reg = ( GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK0 );
	while ( (GCLK->PCHCTRL[GCM_DAC].reg & GCLK_PCHCTRL_CHEN) == 0 ){
		// wait for sync
	}
	DAC->CTRLA.bit.ENABLE = 0;
	
  while ( DAC->SYNCBUSY.bit.ENABLE == 1 ); // Wait for synchronization of registers between the clock domains
  DAC->CTRLB.reg = DAC_CTRLB_REFSEL_VDDANA_Val; // Using the 3.3V reference
  DAC->DACCTRL->bit.ENABLE = 1;   // External Output Enable (Vout)
}

#ifdef __cplusplus
}
#endif
