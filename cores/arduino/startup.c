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

#include "sam.h"
#include "variant.h"

#include <stdio.h>

/**
 * \brief SystemInit() configures the needed clocks and according Flash Read Wait States.
 * At reset:
 * - Generic Clock Generator 0 (GCLKMAIN) is using XOSC32K as source.
 * We need to:
 * 1) Enable XOSC32K clock (External on-board 32.768Hz oscillator), will be used as DFLL48M reference.
 * 2) Put XOSC32K as source of Generic Clock Generator 1
 * 3) Put Generic Clock Generator 1 as source for Generic Clock Multiplexer 0 (DFLL48M reference)
 * 4) Enable DFLL48M clock
 * 5) Switch Generic Clock Generator 0 to DFLL48M. CPU will run at 48MHz.
 */
// Constants for Clock generators
#define GENERIC_CLOCK_GENERATOR_MAIN      (0u)
#define GENERIC_CLOCK_GENERATOR_XOSC32K   (1u)
#define GENERIC_CLOCK_GENERATOR_OSC32K    (1u)
#define GENERIC_CLOCK_GENERATOR_OSCULP32K (2u) /* Initialized at reset for WDT */
#define GENERIC_CLOCK_GENERATOR_OSC8M     (3u)
// Constants for Clock multiplexers
#define GENERIC_CLOCK_MULTIPLEXER_DFLL48M (0u)


static void gclk_gen_sync(int generator) {
	while (GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL(1 << generator )) {
		/* Wait for synchronization */
	};
}

void SystemInit( void )
{
/* Various bits in the INTFLAG register can be set to one at startup.
	   This will ensure that these bits are cleared */
	//OSCCTRL->INTFLAG.reg = OSCCTRL_INTFLAG_DFLLRDY;
	SUPC->INTFLAG.reg = SUPC_INTFLAG_BOD33RDY | SUPC_INTFLAG_BOD33DET;
  /*
   * Disable automatic NVM write operations (errata).
   * Disable NVM cache on D51 (errata). Will be re-enabled after reset at end of programming.
   */
	NVMCTRL->CTRLB.bit.MANW = 1;
  /* Set 1 Flash Wait State for 48MHz (2 for the L21 and C21), cf tables 20.9 and 35.27 in SAMD21 Datasheet
   */
    NVMCTRL->CTRLB.reg |= NVMCTRL_CTRLB_RWS_DUAL ; // two wait states
	
	/*  Switch to PL2 to be sure configuration of GCLK0 is safe */
	/* Clear performance level status */
	PM->INTFLAG.reg = PM_INTFLAG_PLRDY;
	/* Switch performance level */
	PM->PLCFG.reg = PM_PLCFG_PLSEL_PL2_Val;
	/* Waiting performance level ready */
	while (!PM->INTFLAG.reg) {
		;
	}
	
// init external 32kHz xtal
	OSC32KCTRL_XOSC32K_Type temp = OSC32KCTRL->XOSC32K;
	/** Wait 65536 clock cycles until the clock source is considered stable */
	 temp.bit.STARTUP = 4; 
	 // always run the clock
	 temp.bit.ONDEMAND = 0;
	 // its a crystal, not a clock input
	 temp.bit.XTALEN = 1;
	 // enable 32k output 
	 temp.bit.EN32K = 1;
    // run xtal in standby 
	 temp.bit.RUNSTDBY = 1;
	 // commit changes
	OSC32KCTRL->XOSC32K.reg = temp.reg;
	
	// enable xosc32k clock
	OSC32KCTRL->XOSC32K.reg |= OSC32KCTRL_XOSC32K_ENABLE;
	 // wait for clock to become ready
	while ( (OSC32KCTRL->STATUS.reg & OSC32KCTRL_STATUS_XOSC32KRDY) == 0 );

// init DFLL
	/* Using DFLL48M COARSE CAL value from NVM Software Calibration Area Mapping 
	   in DFLL.COARSE helps to output a frequency close to 48 MHz.*/
#define NVM_DFLL_COARSE_POS    26 /* DFLL48M Coarse calibration value bit position.*/
#define NVM_DFLL_COARSE_SIZE   6  /* DFLL48M Coarse calibration value bit size.*/

	uint32_t coarse =( *((uint32_t *)(NVMCTRL_OTP5)
	+ (NVM_DFLL_COARSE_POS / 32))
	>> (NVM_DFLL_COARSE_POS % 32))
	& ((1 << NVM_DFLL_COARSE_SIZE) - 1);
	/* In some revision chip, the Calibration value is not correct */
	if (coarse == 0x3f) {
		coarse = 0x1f;
	}
	// wait for dfll register to be ready before we write to it
	while ( ( OSCCTRL->INTFLAG.reg & OSCCTRL_INTFLAG_DFLLRDY) == 0 );
	
	OSCCTRL_DFLLVAL_Type dfllval = OSCCTRL->DFLLVAL;
	dfllval.bit.COARSE = coarse; /** Coarse calibration value (closed loop mode) */
	dfllval.bit.FINE = 0xff / 4; /* Midpoint fine calibration value (closed loop mode) */
	OSCCTRL->DFLLVAL= dfllval;
	
	OSCCTRL_DFLLMUL_Type dfllMul = OSCCTRL->DFLLMUL;
	dfllMul.bit.CSTEP = (0x1f / 4); //MAX_COARSE_STEP_SIZE
	dfllMul.bit.FSTEP = (0xff / 4); //MAX_FINE_STEP_SIZE
	dfllMul.bit.MUL	 = (48000000 / 32768); // MULTIPLY_FACTOR
	OSCCTRL->DFLLMUL.reg = dfllMul.reg;
	
	OSCCTRL_DFLLCTRL_Type dfllCtrl = OSCCTRL->DFLLCTRL;
	dfllCtrl.bit.MODE = OSCCTRL_DFLLCTRL_MODE; // closed loop mode
	dfllCtrl.bit.LLAW = 0;  /** Keep DFLL lock when the device wakes from sleep */
	dfllCtrl.bit.STABLE = 0;/** Keep tracking after the DFLL has gotten a fine lock */
	dfllCtrl.bit.QLDIS = 0;/** Enable the QuickLock feature for looser lock requirements on the DFLL */
	dfllCtrl.bit.CCDIS = 0;/** Enable a chill cycle, where the DFLL output frequency is not measured */
	dfllCtrl.bit.ONDEMAND = 0; /** Disable on demand mode */
	dfllCtrl.bit.RUNSTDBY = 0; /** Do not run in standby */
    OSCCTRL->DFLLCTRL.reg = dfllCtrl.reg;
	// clock not enabled yet
	//

// init GCLK (0,1,2,3)
/* Turn on the digital interface clock */
	MCLK->APBAMASK.reg |= MCLK_APBAMASK_GCLK;
/* Software reset the module to ensure it is re-initialized correctly */
	GCLK->CTRLA.reg = GCLK_CTRLA_SWRST;
	while (GCLK->CTRLA.reg & GCLK_CTRLA_SWRST) {
		/* Wait for reset to complete */
	}
/* Configure all GCLK generators except for the main generator, which
 * is configured later after all other clock systems are set up */

	GCLK_GENCTRL_Type gclkConfig;
/* Configure GCLK generator 1  ( hw timer )
* run in standby : 1
* source : GCLK_SOURCE_XOSC32K
* prescaler ( division factor ) : 1
* output enable : 0
*/
	gclkConfig.reg = GCLK->GENCTRL[1].reg;
	gclkConfig.bit.DIV = 1;
	gclkConfig.bit.SRC = GCLK_GENCTRL_SRC_XOSC32K_Val;
	gclkConfig.bit.OE = 0;
	gclkConfig.bit.RUNSTDBY = 0;

	GCLK->GENCTRL[1].reg = gclkConfig.reg;
	/* Enable generator */
	gclk_gen_sync(1);
	GCLK->GENCTRL[1].reg |= GCLK_GENCTRL_GENEN;

/* Configure GCLK generator 2 ( adc )
* run in standby : 0
* source : GCLK_SOURCE_OSC16M
* prescaler ( division factor ) : 5 ( use improved dutycycle mode )
* output enable : 0
*/
	gclkConfig.reg = GCLK->GENCTRL[2].reg;
	gclkConfig.bit.DIV = 5;
	gclkConfig.bit.IDC = 1;
	gclkConfig.bit.SRC = GCLK_GENCTRL_SRC_OSC16M_Val;
	gclkConfig.bit.OE = 0;
	gclkConfig.bit.RUNSTDBY = 0;

	GCLK->GENCTRL[2].reg = gclkConfig.reg;
	/* Enable generator */
	gclk_gen_sync(2);
	GCLK->GENCTRL[2].reg |= GCLK_GENCTRL_GENEN;

/* Configure GCLK generator 3
* run in standby : 0
* source : GCLK_SOURCE_DFLL48M
* prescaler : 1
* output enable : 0
*/
	gclkConfig.reg = GCLK->GENCTRL[3].reg;
	gclkConfig.bit.DIV = 1;
	gclkConfig.bit.SRC = GCLK_GENCTRL_SRC_DFLL48M_Val;
	gclkConfig.bit.OE = 0;
	gclkConfig.bit.RUNSTDBY = 0;

	GCLK->GENCTRL[3].reg = gclkConfig.reg;
	/* Enable generator */
	gclk_gen_sync(3);
	GCLK->GENCTRL[3].reg |= GCLK_GENCTRL_GENEN;
	


/* Enable DFLL reference clock in closed loop mode */
/* Disable the peripheral channel */
	GCLK->PCHCTRL[OSCCTRL_GCLK_ID_DFLL48].reg &= ~GCLK_PCHCTRL_CHEN;
/* Configure the peripheral channel */
	GCLK->PCHCTRL[OSCCTRL_GCLK_ID_DFLL48].reg = GCLK_PCHCTRL_GEN(1); // Generator 1 is the source ( xosc32k )
	/* Enable the peripheral channel */
	GCLK->PCHCTRL[OSCCTRL_GCLK_ID_DFLL48].reg |= GCLK_PCHCTRL_CHEN;
	
	while (!(GCLK->PCHCTRL[OSCCTRL_GCLK_ID_DFLL48].reg & GCLK_PCHCTRL_CHEN)) {
		/* Wait for clock synchronization */
	}
	
/* DFLL Enable (Open and Closed Loop) */
// system_clock_source_dfll_set_config_errata_9905;
/* Disable ONDEMAND mode while writing configurations */
		
	// wait for dfll register to be ready before we write to it
	while ( ( OSCCTRL->INTFLAG.reg & OSCCTRL_INTFLAG_DFLLRDY) == 0 );
	
	OSCCTRL->DFLLCTRL.bit.ONDEMAND = 0;
	dfllCtrl.bit.ENABLE = 1; // set enable bit
	OSCCTRL->DFLLCTRL.reg = OSCCTRL_DFLLCTRL_ENABLE;
	
	OSCCTRL->DFLLMUL.reg = 0;
	// wait for dfll register to be ready before we write to it
	while ( ( OSCCTRL->INTFLAG.reg & OSCCTRL_INTFLAG_DFLLRDY) == 0 );
	OSCCTRL->DFLLMUL.reg = dfllMul.reg;

		// wait for dfll register to be ready before we write to it
	while ( ( OSCCTRL->INTFLAG.reg & OSCCTRL_INTFLAG_DFLLRDY) == 0 );
		OSCCTRL->DFLLVAL.reg = dfllval.reg;
	
		// wait for dfll register to be ready before we write to it
	while ( ( OSCCTRL->INTFLAG.reg & OSCCTRL_INTFLAG_DFLLRDY) == 0 );
	/* Write full configuration to DFLL control register */
	OSCCTRL->DFLLCTRL.reg = 0;
	while (!(OSCCTRL->STATUS.reg & OSCCTRL_STATUS_DFLLRDY)) {
		/* Wait for DFLL sync */
	}
	OSCCTRL->DFLLCTRL.reg = dfllCtrl.reg;

/* Enable generator 0 as it depends on other generators*/
/* Configure GCLK generator 0 (Main Clock)
* run in standby : false
* source : GCLK_SOURCE_DFLL48M
* prescaler : 1
* output enable : false
*/
gclkConfig.reg = GCLK->GENCTRL[0].reg;
gclkConfig.bit.DIV = 1;
gclkConfig.bit.SRC = GCLK_GENCTRL_SRC_DFLL48M_Val;
gclkConfig.bit.OE = 0;
gclkConfig.bit.RUNSTDBY = 0;
GCLK->GENCTRL[0].reg = gclkConfig.reg;
gclk_gen_sync(0);
GCLK->GENCTRL[0].reg |= GCLK_GENCTRL_GENEN;
	/* CPU and BUS clocks */
	MCLK->BUPDIV.reg = MCLK_BUPDIV_BUPDIV_DIV1;/** Divide Main clock by one */
	MCLK->LPDIV.reg = MCLK_LPDIV_LPDIV_DIV1; /** Divide low power clock by 1*/
	MCLK->CPUDIV.reg = MCLK_CPUDIV_CPUDIV_DIV1; /**(MCLK_CPUDIV) Divide by 1 */
  SystemCoreClock=VARIANT_MCK ;

  /* ----------------------------------------------------------------------------------------------
   * 8) Load ADC factory calibration values
   */

  // ADC Bias Calibration
  uint32_t bias = (*((uint32_t *) ADC_FUSES_BIASCOMP_ADDR) & ADC_FUSES_BIASCOMP_Msk) >> ADC_FUSES_BIASCOMP_Pos;

  // ADC Linearity bits 4:0
  uint32_t linearity = (*((uint32_t *) ADC_FUSES_BIASREFBUF_ADDR) & ADC_FUSES_BIASREFBUF_Msk) >> ADC_FUSES_BIASREFBUF_Pos;
  
  ADC->CALIB.reg = ADC_CALIB_BIASCOMP(bias) | ADC_CALIB_BIASREFBUF(linearity);

  /*
   * 9) Disable automatic NVM write operations
   */
  NVMCTRL->CTRLB.bit.MANW = 1;
}
