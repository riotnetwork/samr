//#if defined(ARDUINO_ARCH_SAMR)

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
//	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
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
	// Disable systick interrupt:  See https://www.avrfreaks.net/forum/samd21-samd21e16b-sporadically-locks-and-does-not-wake-standby-sleep-mode
	SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;	
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
	__DSB();
	__WFI();
	// Enable systick interrupt
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;	
	if (restoreUSBDevice) {
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

	// enable EIC clock to use ULP 32k instead of FLL
	GCLK_GENCTRL_Type gclkConfig;
	/* Configure GCLK generator 6
	* run in standby : 1
	* source : Ultra low power OSC32K
	* prescaler ( division factor ) : 1
	* output enable to pin: 0
	*/
	gclkConfig.reg = 0;
	gclkConfig.reg = GCLK->GENCTRL[6].reg;
	gclkConfig.bit.DIV = 1;
	gclkConfig.bit.SRC = GCLK_GENCTRL_SRC_OSCULP32K;
	gclkConfig.bit.OE = 0;
	gclkConfig.bit.RUNSTDBY = 1;

	GCLK->GENCTRL[6].reg = gclkConfig.reg;
	
	while (GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL(1 << 6 )) {
		/* Wait for synchronization */
	};
	
	/* Enable generator */
	GCLK->GENCTRL[6].reg |= GCLK_GENCTRL_GENEN;


	
		
	//Put Generic Clock Generator 6 (ULP32k) as source for Peripheral channel 3 (EIC)
	GCLK->PCHCTRL[GCM_EIC].reg = (GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK6);
	while ((GCLK->PCHCTRL[GCM_EIC].reg & GCLK_PCHCTRL_CHEN) == 0);        // wait for sync
	// Enable wakeup capability on pin in case being used during sleep
	// EIC->INTENSET.reg |= (1 << in);

	/* Errata: Make sure that the Flash does not power all the way down
     	* when in sleep mode. */
	NVMCTRL->CTRLB.bit.SLEEPPRM = NVMCTRL_CTRLB_SLEEPPRM_DISABLED_Val;
}

StampLowPower LowPower;

//#endif // ARDUINO_ARCH_SAMR
