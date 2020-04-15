#if defined(ARDUINO_ARCH_SAMR)

#include "StampLowPower.h"
#include "WInterrupts.h"

void StampLowPower::idle() {
	SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
	PM->SLEEPCFG.reg = PM_SLEEPCFG_SLEEPMODE_IDLE;
	__DSB();
	__WFI();
}

/*Peripherals with RUNSTANDBY bit set in their CTRLA register will continue to run if their corresponong clock is also set as RUNSTANBY*/
void StampLowPower::standby() {
	SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
	PM->SLEEPCFG.reg = PM_SLEEPCFG_SLEEPMODE_STANDBY;
	__DSB();
	__WFI();
}

/*2.3 BackupImportant:  Backup mode is not supported on SAM L10, SAM L11 series devices.
In Backup (BACKUP) Sleep mode only the backup domain is powered and only registers in this domainwill hold their value
Wakeup from BACKUP will be similarto a device start-up after reset
*/
void StampLowPower::backup() {
	SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
	PM->SLEEPCFG.reg = PM_SLEEPCFG_SLEEPMODE_BACKUP;
	__DSB();
	__WFI();
}

void StampLowPower::setSleepMode( sleepModes_e sleepMode) {
	switch (sleepMode)
	{
	case SLEEP_IDLE :
		PM->SLEEPCFG.reg = PM_SLEEPCFG_SLEEPMODE_IDLE;
		break;
	case SLEEP_STANDBY :
		PM->SLEEPCFG.reg = PM_SLEEPCFG_SLEEPMODE_STANDBY;
		break;
	case SLEEP_BACKUP :
		PM->SLEEPCFG.reg = PM_SLEEPCFG_SLEEPMODE_BACKUP;
		break;
	default:
		break;
	}
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
}

void StampLowPower::idle(uint32_t millis) {
	setAlarmIn(millis);
	idle();
}

void StampLowPower::sleep() {
	bool restoreUSBDevice = false;
	if (SERIAL_PORT_USBVIRTUAL) {
		USBDevice.standby();
	} else {
		USBDevice.detach();
		restoreUSBDevice = true;
	}

	GCLK_GENCTRL_Type gclkConfig;
	gclkConfig.reg = 0;
	gclkConfig.reg = GCLK->GENCTRL[0].reg;
	gclkConfig.bit.SRC = GCLK_GENCTRL_SRC_OSC16M_Val;// GCLK_GENCTRL_SRC_OSCULP32K_Val ;//GCLK_GENCTRL_SRC_OSC16M_Val
	GCLK->GENCTRL[0].reg = gclkConfig.reg;
	
	while (GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL(0)) {
		/* Wait for synchronization */
	};
	OSCCTRL->OSC16MCTRL.reg |= OSCCTRL_OSC16MCTRL_ONDEMAND;
	GCLK->GENCTRL[1].bit.GENEN = 0;
	OSCCTRL->DFLLCTRL.bit.ENABLE = 0;
/* Clear performance level status */
	PM->INTFLAG.reg = PM_INTFLAG_PLRDY;
/* Switch performance level to PL0 - best power saving */
	PM->PLCFG.reg = PM_PLCFG_PLSEL_PL0_Val;
		while (!PM->INTFLAG.reg) {
			;
		}
		
	// Disable systick interrupt:  See https://www.avrfreaks.net/forum/samd21-samd21e16b-sporadically-locks-and-does-not-wake-standby-sleep-mode
	SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;	
	
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
	__DSB();
	__WFI();
	// sleeping here, will wake from here ( except from OFF or Backup modes, those look like POR )

	GCLK->GENCTRL[1].bit.GENEN = 1; // re-enable eosc32k
	while (GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL(1)) {
		/* Wait for synchronization */
	};
		// wait for clock to become ready
	while ( (OSC32KCTRL->STATUS.reg & OSC32KCTRL_STATUS_XOSC32KRDY) == 0 );
	
	/* Clear performance level status */
	PM->INTFLAG.reg = PM_INTFLAG_PLRDY;
	/* Switch performance level to PL2 - Highest performance */
	PM->PLCFG.reg = PM_PLCFG_PLSEL_PL2_Val;
	/* Waiting performance level ready */
	while (!PM->INTFLAG.reg) {
		;
	}
	
	OSCCTRL->DFLLCTRL.bit.ENABLE = 1;
	while (!(OSCCTRL->STATUS.reg & OSCCTRL_STATUS_DFLLRDY)) {
		/* Wait for DFLL sync */
	}
	gclkConfig.reg = 0;
	gclkConfig.reg = GCLK->GENCTRL[0].reg;
	gclkConfig.bit.SRC = GCLK_GENCTRL_SRC_DFLL48M_Val;
	GCLK->GENCTRL[0].reg = gclkConfig.reg;
	while (GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL(0)) {
		/* Wait for synchronization */
	};
//	GCLK->GENCTRL[0].reg |= GCLK_GENCTRL_GENEN;
	/*  Switch to PL2 to be sure configuration of GCLK0 is safe */

	
	// Enable systick interrupt
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	
	if (restoreUSBDevice) {
		delay(1);
		USBDevice.init();
		USBDevice.attach();	
	}
}

void StampLowPower::sleep(uint32_t millis) {
	setAlarmIn(millis);
	sleep();
}

void StampLowPower::deepSleep() {
	sleep();
}

void StampLowPower::deepSleep(uint32_t millis) {
	sleep(millis);
}

void StampLowPower::setAlarmIn(uint32_t millis) {

	if (!rtc.isConfigured()) {
		attachInterruptWakeup(RTC_ALARM_WAKEUP, NULL, 0);
	}

	uint32_t now = rtc.getEpoch();
	rtc.setAlarmEpoch(now + millis/1000);
	rtc.enableAlarm(rtc.MATCH_YYMMDDHHMMSS);
}

void StampLowPower::attachInterruptWakeup(uint32_t pin, voidFuncPtr callback, uint32_t mode) {

	if (pin > PINS_COUNT) {
		// check for external wakeup sources
		// RTC library should call this API to enable the alarm subsystem
		switch (pin) {
			case RTC_ALARM_WAKEUP:
				rtc.begin(false);
				rtc.attachInterrupt(callback);
			/*case UART_WAKEUP:*/
		}
		return;
	}

	EExt_Interrupts in = g_APinDescription[pin].ulExtInt;
	if (in == NOT_AN_INTERRUPT || in == EXTERNAL_INT_NMI)
    		return;
			
	//pinMode(pin, INPUT_PULLUP);
	attachInterrupt(pin, callback, mode);

	// Enable wakeup capability on pin in case being used during sleep
	EIC->CTRLA.bit.CKSEL = 1; // use ULP32k as source ( SAML21 is different to D21 series, EIC can be set to use ULP32k without GCLK )
  // Enable EIC
	EIC->CTRLA.bit.ENABLE = 1;
   while (EIC->SYNCBUSY.bit.ENABLE == 1) { /*wait for sync*/ }

	/* Errata: Make sure that the Flash does not power all the way down
     	* when in sleep mode. */
 	// NVMCTRL->CTRLB.bit.SLEEPPRM = NVMCTRL_CTRLB_SLEEPPRM_DISABLED_Val;
	 NVMCTRL->CTRLB.bit.SLEEPPRM =  NVMCTRL_CTRLB_SLEEPPRM_WAKEUPINSTANT_Val;
}

StampLowPower LowPower;

#endif // ARDUINO_ARCH_SAMR
