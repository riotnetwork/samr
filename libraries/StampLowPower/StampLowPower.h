#ifndef _ARDUINO_LOW_POWER_H_
#define _ARDUINO_LOW_POWER_H_

#include <Arduino.h>
#include <SERCOM.h>
#include <Wire.h>
#include <SPI.h>

#ifdef ARDUINO_ARCH_AVR
#error The library is not compatible with AVR boards
#endif

#ifdef ARDUINO_ARCH_SAMR
#include "RTCR34.h"
#endif

#define RTC_ALARM_WAKEUP	0xFF

//typedef void (*voidFuncPtr)( void ) ;
typedef void (*onOffFuncPtr)( bool ) ;

enum sleepModes_e {
	SLEEP_IDLE = 2,
	SLEEP_STANDBY = 4,
	SLEEP_BACKUP = 5
};

typedef enum{
	OTHER_WAKEUP = 0,
	GPIO_WAKEUP = 1,
	NFC_WAKEUP = 2,
	ANALOG_COMPARATOR_WAKEUP = 3
} wakeup_reason;


class StampLowPower {
	public:
		void idle(void);
		void idle(uint32_t millis);
		void idle(int millis) {
			idle((uint32_t)millis);
		}
		void standby();
		void backup();

		void sleep(void);
		void sleep(uint32_t millis);
		void sleep(int millis) {
			sleep((uint32_t)millis);
		}

		void deepSleep(void);
		void deepSleep(uint32_t millis);
		void deepSleep(int millis) {
			deepSleep((uint32_t)millis);
		}
		void setSleepMode(sleepModes_e sleepMode);
		void attachInterruptWakeup(uint32_t pin, voidFuncPtr callback, uint32_t mode);
		void attachWakeupfromBackup(uint32_t pin, uint32_t mode);
		
		uint8_t getWakeupReason();
		uint8_t getExtWakeNo();
		
		void wakeOnWire(TwoWire * wire, bool intEnable);
		void wakeOnSPI(SPIClass * spi, bool intEnable);
		void wakeOnSerial(Uart * uart, bool intEnable);
		
		
#ifdef BOARD_HAS_COMPANION_CHIP
		void companionLowPowerCallback(onOffFuncPtr callback) {
			companionSleepCB = callback;
		}
		void companionSleep() {
			companionSleepCB(true);
		}
		void companionWakeup() {
			companionSleepCB(false);
		}
#endif

	private:
		void setAlarmIn(uint32_t millis);
		RTCR34 rtc;
#ifdef BOARD_HAS_COMPANION_CHIP
		void (*companionSleepCB)(bool);
#endif
	
};

extern StampLowPower LowPower;

#endif
