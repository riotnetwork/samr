#if defined(ARDUINO_ARCH_SAMR)

#include "StampLowPower.h"
#include "WInterrupts.h"
#include "sam.h"
#include "Reset.h"

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

	/* Clear performance level status */
	PM->INTFLAG.reg = PM_INTFLAG_PLRDY;
	/* Switch performance level to PL0 - best power saving */
	PM->PLCFG.reg = PM_PLCFG_PLSEL_PL0_Val;
	while (!PM->INTFLAG.reg) {
		;
	}


	OSCCTRL_DFLLCTRL_Type dfllCtrlSlp;
	dfllCtrlSlp.reg = OSCCTRL->DFLLCTRL.reg;
	dfllCtrlSlp.bit.ENABLE = 0;
	OSCCTRL->DFLLCTRL.reg = dfllCtrlSlp.reg;

	// disable DFLL GCLK
	/* Disable the peripheral channel 0 ( DFLL )*/
	GCLK->PCHCTRL[0].reg &= ~GCLK_PCHCTRL_CHEN;

	while (GCLK->PCHCTRL[0].reg & GCLK_PCHCTRL_CHEN) {
		/* Wait for clock synchronization */
	}
	

	// disable xosc32k clock
	OSC32KCTRL->XOSC32K.reg &= ~OSC32KCTRL_XOSC32K_ENABLE;

	// disable generator 1
	GCLK->GENCTRL[1].bit.GENEN = 0;
	
	// Disable systick interrupt:  See https://www.avrfreaks.net/forum/samd21-samd21e16b-sporadically-locks-and-does-not-wake-standby-sleep-mode
	SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
	// set core voltage regulator to "runstandby" - erratta Main Voltage Regulator Reference:15264
	//SUPC->VREG.bit.RUNSTDBY = 1;
	//SUPC->VREG.bit.STDBYPL0 = 1;
	
	/* CPU and BUS clocks slow down - slow down busses BEFORE cpu.. */
	MCLK->BUPDIV.reg = MCLK_BUPDIV_BUPDIV_DIV128;/** Divide Main clock ( 4MHz OSC ) by 64,ie run at 31.768kHz */
	MCLK->LPDIV.reg = MCLK_BUPDIV_BUPDIV_DIV128; /** Divide low power clock ( 4MHz OSC ) by 64, ie run at 31.768kHz*/
	MCLK->CPUDIV.reg = MCLK_CPUDIV_CPUDIV_DIV64; /**(MCLK_CPUDIV) Divide by 64 ,ie run at 62.5kHz */
	
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
	__DSB();
	__WFI();
	// sleeping here, will wake from here ( except from OFF or Backup modes, those look like POR )
	
	/* CPU and BUS clocks back to "regular ratio"*/
	MCLK->CPUDIV.reg = MCLK_CPUDIV_CPUDIV_DIV1; /**(MCLK_CPUDIV) Divide by 1 ,ie run at 4MHz.. until we start the DFLL again */
	MCLK->BUPDIV.reg = MCLK_BUPDIV_BUPDIV_DIV1;/** Div 1, so run these at main clock rate */
	MCLK->LPDIV.reg = MCLK_BUPDIV_BUPDIV_DIV1; /**low power domain back to CPU clock speed */
	
	// enable xosc32k clock
	OSC32KCTRL->XOSC32K.reg |= OSC32KCTRL_XOSC32K_ENABLE;
	// wait for clock to become ready
	while ((OSC32KCTRL->STATUS.reg & OSC32KCTRL_STATUS_XOSC32KRDY) == 0);

	GCLK->GENCTRL[1].bit.GENEN = 1; // re-enable generator 1 ( xosc32k )
	while (GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL(1)) {
		/* Wait for synchronization */
	};

	/* Enable DFLL peripheral channel */
	GCLK->PCHCTRL[0].reg |= GCLK_PCHCTRL_CHEN;

	while (GCLK->PCHCTRL[0].reg & GCLK_PCHCTRL_CHEN) {
		/* Wait for clock synchronization */
	}

	// re-enable DFLL
	dfllCtrlSlp.bit.ENABLE = 1;
	OSCCTRL->DFLLCTRL.reg = dfllCtrlSlp.reg;

	/* Clear performance level status */
	PM->INTFLAG.reg = PM_INTFLAG_PLRDY;
	/* Switch performance level to PL2 - Highest performance */
	PM->PLCFG.reg = PM_PLCFG_PLSEL_PL2_Val;
	/* Waiting performance level ready */
	while (!PM->INTFLAG.reg) {
		;
	}
	
	OSCCTRL->DFLLCTRL.reg = OSCCTRL_DFLLCTRL_ENABLE;

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


void StampLowPower::wakeOnWire(TwoWire * wire, bool intEnable) {
	wire->sercom->disableWIRE();
	wire->sercom->sercom->I2CS.CTRLA.bit.RUNSTDBY = intEnable ;
	wire->sercom->enableWIRE();
}

void StampLowPower::wakeOnSPI(SPIClass * spi, bool intEnable) {
	spi->_p_sercom->disableSPI();
	spi->_p_sercom->sercom->SPI.CTRLA.bit.RUNSTDBY = intEnable ;
	spi->_p_sercom->enableSPI();
}

void StampLowPower::wakeOnSerial(Uart * uart, bool intEnable) {
	uart->sercom->disableUART();
	uart->sercom->sercom->USART.CTRLA.bit.RUNSTDBY = intEnable ;
	uart->sercom->enableUART();
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

void StampLowPower::attachWakeupfromBackup(uint32_t pin, uint32_t mode) {

	if (pin > PINS_COUNT) {
		// make sure its a real pin
		return;
	}
	

	EExt_Interrupts in = g_APinDescription[pin].ulExtInt; // EXTINT_wake numbers are the same as EXTINT numbers
	if (in >= EXTERNAL_WAKE_NUM ) // not many external WAKE interrupts available (0..7 )
    		return;
			
// ensure pin is an input			
	pinMode(pin, INPUT);
	
	// set pin debounce to 32 cycles of ULP32k
	RSTC->WKDBCONF.reg = RSTC_WKDBCONF_WKDBCNT_32CK32;
	
//  set wake polarity , 1 = active HIGH, 0 = active LOW, for each of the 8 [0..7] EXTints
	if (mode == HIGH)
	{
		RSTC->WKPOL.reg |= (RSTC_WKPOL_WKPOL_Msk & (1 << ((uint8_t)in)) ) ;
	}
	else
	{
		RSTC->WKPOL.reg |= (RSTC_WKPOL_WKPOL_Msk & (0 << ((uint8_t)in)) ) ;
	}
	
// set RSTC WKUP enable for this wkup source
    RSTC->WKEN.reg |=  (RSTC_WKEN_WKEN_Msk &  (1 << ((uint8_t)in)) ) ;
}

/*returns wakeup reason 
0 : none of the external wakeups, could have been POR, check resetCause()
BBPS : Battery Backup Power Switch
RTC : Real Timer Counter Interrupt
EXTWAKE : External Wake-up
*/
uint8_t StampLowPower::getWakeupReason(){
	if (resetCause() == RSTC_RCAUSE_BACKUP)
	{
	// reset was because of a backup wake
	uint8_t wakeReason = RSTC->BKUPEXIT.reg;
		 if (wakeReason & RSTC_BKUPEXIT_BBPS)
		{
			return RSTC_BKUPEXIT_BBPS;
		}
		else if (wakeReason & RSTC_BKUPEXIT_RTC)
		{
			return RSTC_BKUPEXIT_RTC;
		}
		if (wakeReason & RSTC_BKUPEXIT_EXTWAKE)
		{
			return RSTC_BKUPEXIT_EXTWAKE;
		}

	}
	return 0;
}

/*returns wakeup ExtWake pin register ( 0..7 )
*/
uint8_t StampLowPower::getExtWakeNo(){
	if (resetCause() == RSTC_RCAUSE_BACKUP)
	{
		return (uint8_t)RSTC->WKCAUSE.reg;
	}
}

StampLowPower LowPower;

#endif // ARDUINO_ARCH_SAMR
