/*
  RTC library for Arduino Zero.
  Copyright (c) 2015 Arduino LLC. All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include <time.h>

#include "RTCR34.h"

#define EPOCH_TIME_OFF      946684800  // This is 1st January 2000, 00:00:00 in epoch time
#define EPOCH_TIME_YEAR_OFF 100        // years since 1900

// Default date & time after reset
#define DEFAULT_YEAR    2000    // 2000..2063
#define DEFAULT_MONTH   1       // 1..12
#define DEFAULT_DAY     1       // 1..31
#define DEFAULT_HOUR    0       // 1..23
#define DEFAULT_MINUTE  0       // 0..59
#define DEFAULT_SECOND  0       // 0..59

voidFuncPtr RTC_callBack = NULL;

RTCR34::RTCR34()
{
  _configured = false;
}

void RTCR34::begin(bool resetTime)
{
// make sure OSC32KCTRL has the 1.024kHz output enabled
	config32kOSC();
// Setup clock MLCK and OSC32KCTRL for RTC
	configureClock();
// disable RTC before we change any settings
	RTCdisable();
// reset to have a known state
	RTCreset();
	// disable RTC before we change any settings
//	RTCdisable();
	//setup RTC in Mode 2 with 1.024kHz clock
	delay(1);
	RTCsetMode();

  
// If the RTC is in clock mode and the reset was
  // not due to POR or BOD, preserve the clock time
  // POR causes a reset anyway, BOD behaviour is?
  bool validTime = false;
  RTC_MODE2_CLOCK_Type oldTime;

  if ((!resetTime) && (RSTC->RCAUSE.reg & (RSTC_RCAUSE_SYST | RSTC_RCAUSE_WDT | RSTC_RCAUSE_EXT))) {
    if (RTC->MODE2.CTRLA.reg && RTC_MODE2_CTRLA_MODE_CLOCK) {
      validTime = true;
      oldTime.reg = RTC->MODE2.CLOCK.reg;
    }
  }
  RTC->MODE2.INTENSET.reg |= RTC_MODE2_INTENSET_ALARM0; // enable alarm interrupt on RTC
  
  NVIC_EnableIRQ(RTC_IRQn); // enable RTC interrupt in NVIC
  NVIC_SetPriority(RTC_IRQn, 0x00);
 

  RTC->MODE2.Mode2Alarm[0].MASK.bit.SEL = MATCH_OFF; // default alarm match is off (disabled)
	while (RTCisSyncing())
	;

  RTCenable();

	while (RTCisSyncing())
	;
  // If desired and valid, restore the time value, else use first valid time value
  if ((!resetTime) && (validTime) && (oldTime.reg != 0L)) {
    RTC->MODE2.CLOCK.reg = oldTime.reg;
  }
  else {
    RTC->MODE2.CLOCK.reg = RTC_MODE2_CLOCK_YEAR(DEFAULT_YEAR - 2000) | RTC_MODE2_CLOCK_MONTH(DEFAULT_MONTH) 
        | RTC_MODE2_CLOCK_DAY(DEFAULT_DAY) | RTC_MODE2_CLOCK_HOUR(DEFAULT_HOUR) 
        | RTC_MODE2_CLOCK_MINUTE(DEFAULT_MINUTE) | RTC_MODE2_CLOCK_SECOND(DEFAULT_SECOND);
  }
  while (RTCisSyncing())
    ;
RTC->MODE2.INTFLAG.reg = ~RTC_MODE2_INTFLAG_RESETVALUE; // clear all flags
  _configured = true;
}

void RTC_Handler(void)
{
  if (RTC_callBack != NULL) {
    RTC_callBack();
  }
  RTC->MODE2.INTFLAG.reg = RTC_MODE2_INTFLAG_ALARM0; // must clear flag at end
}

void RTCR34::enableAlarm(Alarm_Match match)
{
  if (_configured) {
    RTC->MODE2.Mode2Alarm[0].MASK.bit.SEL = match;
    while (RTCisSyncing())
      ;
  }
}

void RTCR34::disableAlarm()
{
  if (_configured) {
    RTC->MODE2.Mode2Alarm[0].MASK.bit.SEL = 0x00;
    while (RTCisSyncing())
      ;
  }
}

void RTCR34::attachInterrupt(voidFuncPtr callback)
{
  RTC_callBack = callback;
}

void RTCR34::detachInterrupt()
{
  RTC_callBack = NULL;
}

void RTCR34::standbyMode()
{
  // Entering standby mode when connected
  // via the native USB port causes issues.
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
  __DSB();
  __WFI();
}

/*
 * Get Functions
 */

uint8_t RTCR34::getSeconds()
{
  RTCreadRequest();
  return RTC->MODE2.CLOCK.bit.SECOND;
}

uint8_t RTCR34::getMinutes()
{
  RTCreadRequest();
  return RTC->MODE2.CLOCK.bit.MINUTE;
}

uint8_t RTCR34::getHours()
{
  RTCreadRequest();
  return RTC->MODE2.CLOCK.bit.HOUR;
}

uint8_t RTCR34::getDay()
{
  RTCreadRequest();
  return RTC->MODE2.CLOCK.bit.DAY;
}

uint8_t RTCR34::getMonth()
{
  RTCreadRequest();
  return RTC->MODE2.CLOCK.bit.MONTH;
}

uint8_t RTCR34::getYear()
{
  RTCreadRequest();
  return RTC->MODE2.CLOCK.bit.YEAR;
}

uint8_t RTCR34::getAlarmSeconds()
{
  return RTC->MODE2.Mode2Alarm[0].ALARM.bit.SECOND;
}

uint8_t RTCR34::getAlarmMinutes()
{
  return RTC->MODE2.Mode2Alarm[0].ALARM.bit.MINUTE;
}

uint8_t RTCR34::getAlarmHours()
{
  return RTC->MODE2.Mode2Alarm[0].ALARM.bit.HOUR;
}

uint8_t RTCR34::getAlarmDay()
{
  return RTC->MODE2.Mode2Alarm[0].ALARM.bit.DAY;
}

uint8_t RTCR34::getAlarmMonth()
{
  return RTC->MODE2.Mode2Alarm[0].ALARM.bit.MONTH;
}

uint8_t RTCR34::getAlarmYear()
{
  return RTC->MODE2.Mode2Alarm[0].ALARM.bit.YEAR;
}

/*
 * Set Functions
 */

void RTCR34::setSeconds(uint8_t seconds)
{
  if (_configured) {
    RTC->MODE2.CLOCK.bit.SECOND = seconds;
    while (RTCisSyncing())
      ;
  }
}

void RTCR34::setMinutes(uint8_t minutes)
{
  if (_configured) {
    RTC->MODE2.CLOCK.bit.MINUTE = minutes;
    while (RTCisSyncing())
      ;
  }
}

void RTCR34::setHours(uint8_t hours)
{
  if (_configured) {
    RTC->MODE2.CLOCK.bit.HOUR = hours;
    while (RTCisSyncing())
      ;
  }
}

void RTCR34::setTime(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
  if (_configured) {
    setSeconds(seconds);
    setMinutes(minutes);
    setHours(hours);
  }
}

void RTCR34::setDay(uint8_t day)
{
  if (_configured) {
    RTC->MODE2.CLOCK.bit.DAY = day;
    while (RTCisSyncing())
      ;
  }
}

void RTCR34::setMonth(uint8_t month)
{
  if (_configured) {
    RTC->MODE2.CLOCK.bit.MONTH = month;
    while (RTCisSyncing())
      ;
  }
}

void RTCR34::setYear(uint8_t year)
{
  if (_configured) {
    RTC->MODE2.CLOCK.bit.YEAR = year;
    while (RTCisSyncing())
      ;
  }
}

void RTCR34::setDate(uint8_t day, uint8_t month, uint8_t year)
{
  if (_configured) {
    setDay(day);
    setMonth(month);
    setYear(year);
  }
}

void RTCR34::setAlarmSeconds(uint8_t seconds)
{
  if (_configured) {
    RTC->MODE2.Mode2Alarm[0].ALARM.bit.SECOND = seconds;
    while (RTCisSyncing())
      ;
  }
}

void RTCR34::setAlarmMinutes(uint8_t minutes)
{
  if (_configured) {
    RTC->MODE2.Mode2Alarm[0].ALARM.bit.MINUTE = minutes;
    while (RTCisSyncing())
      ;
  }
}

void RTCR34::setAlarmHours(uint8_t hours)
{
  if (_configured) {
    RTC->MODE2.Mode2Alarm[0].ALARM.bit.HOUR = hours;
    while (RTCisSyncing())
      ;
  }
}

void RTCR34::setAlarmTime(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
  if (_configured) {
    setAlarmSeconds(seconds);
    setAlarmMinutes(minutes);
    setAlarmHours(hours);
  }
}

void RTCR34::setAlarmDay(uint8_t day)
{
  if (_configured) {
    RTC->MODE2.Mode2Alarm[0].ALARM.bit.DAY = day;
    while (RTCisSyncing())
      ;
  }
}

void RTCR34::setAlarmMonth(uint8_t month)
{
  if (_configured) {
    RTC->MODE2.Mode2Alarm[0].ALARM.bit.MONTH = month;
    while (RTCisSyncing())
      ;
  }
}

void RTCR34::setAlarmYear(uint8_t year)
{
  if (_configured) {
    RTC->MODE2.Mode2Alarm[0].ALARM.bit.YEAR = year;
    while (RTCisSyncing())
      ;
  }
}

void RTCR34::setAlarmDate(uint8_t day, uint8_t month, uint8_t year)
{
  if (_configured) {
    setAlarmDay(day);
    setAlarmMonth(month);
    setAlarmYear(year);
  }
}

uint32_t RTCR34::getEpoch()
{
  RTCreadRequest();
  RTC_MODE2_CLOCK_Type clockTime;
  clockTime.reg = RTC->MODE2.CLOCK.reg;

  struct tm tm;

  tm.tm_isdst = -1;
  tm.tm_yday = 0;
  tm.tm_wday = 0;
  tm.tm_year = clockTime.bit.YEAR + EPOCH_TIME_YEAR_OFF;
  tm.tm_mon = clockTime.bit.MONTH - 1;
  tm.tm_mday = clockTime.bit.DAY;
  tm.tm_hour = clockTime.bit.HOUR;
  tm.tm_min = clockTime.bit.MINUTE;
  tm.tm_sec = clockTime.bit.SECOND;

  return mktime(&tm);
}

uint32_t RTCR34::getY2kEpoch()
{
  return (getEpoch() - EPOCH_TIME_OFF);
}

void RTCR34::setAlarmEpoch(uint32_t ts)
{
  if (_configured) {
    if (ts < EPOCH_TIME_OFF) {
      ts = EPOCH_TIME_OFF;
    }

    time_t t = ts;
    struct tm* tmp = gmtime(&t);

    setAlarmDate(tmp->tm_mday, tmp->tm_mon + 1, tmp->tm_year - EPOCH_TIME_YEAR_OFF);
    setAlarmTime(tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
  }
}

void RTCR34::setEpoch(uint32_t ts)
{
  if (_configured) {
    if (ts < EPOCH_TIME_OFF) {
      ts = EPOCH_TIME_OFF;
    }

    time_t t = ts;
    struct tm* tmp = gmtime(&t);

    RTC_MODE2_CLOCK_Type clockTime;

    clockTime.bit.YEAR = tmp->tm_year - EPOCH_TIME_YEAR_OFF;
    clockTime.bit.MONTH = tmp->tm_mon + 1;
    clockTime.bit.DAY = tmp->tm_mday;
    clockTime.bit.HOUR = tmp->tm_hour;
    clockTime.bit.MINUTE = tmp->tm_min;
    clockTime.bit.SECOND = tmp->tm_sec;

    RTC->MODE2.CLOCK.reg = clockTime.reg;

    while (RTCisSyncing())
      ;
  }
}

void RTCR34::setY2kEpoch(uint32_t ts)
{
  if (_configured) {
    setEpoch(ts + EPOCH_TIME_OFF);
  }
}

/* Attach peripheral clock to 32k oscillator */
void RTCR34::configureClock() {
	/* Turn on the digital interface clock */
   MCLK->APBAMASK.reg |= MCLK_APBAMASK_RTC;
   OSC32KCTRL->RTCCTRL.reg = OSC32KCTRL_RTCCTRL_RTCSEL_XOSC1K;
  // wait for clock to become ready
  while ( (OSC32KCTRL->STATUS.reg & OSC32KCTRL_STATUS_XOSC32KRDY) == 0 );
  
}

/*
 * Private Utility Functions
 */

/* Configure the 32768Hz Oscillator */
void RTCR34::config32kOSC() 
{
#ifndef CRYSTALLESS
  OSC32KCTRL->XOSC32K.reg = OSC32KCTRL_XOSC32K_ONDEMAND |
                            OSC32KCTRL_XOSC32K_RUNSTDBY |
                            OSC32KCTRL_XOSC32K_EN32K |
							OSC32KCTRL_XOSC32K_EN1K |
                            OSC32KCTRL_XOSC32K_XTALEN |
                            OSC32KCTRL_XOSC32K_STARTUP(4) |
                            OSC32KCTRL_XOSC32K_ENABLE;
							// wait for clock to become ready
							while ( (OSC32KCTRL->STATUS.reg & OSC32KCTRL_STATUS_XOSC32KRDY) == 0 );
#endif
}

/* Synchronise the CLOCK register for reading*/
inline void RTCR34::RTCreadRequest() {
  if (_configured) {
	  
    while (RTCisSyncing()) 
      ;
  }
}

/* Wait for sync in write operations */
inline bool RTCR34::RTCisSyncing()
{
  return (RTC->MODE2.SYNCBUSY.reg && RTC_MODE2_SYNCBUSY_MASK_) ; //Synchronization Busy in Clock/Calendar mode ( all bits 0 means regs are synced)
}

void RTCR34::RTCdisable()
{  
	while (RTCisSyncing())
    ;
  RTC->MODE2.CTRLA.reg &= ~RTC_MODE2_CTRLA_ENABLE; // disable RTC
	while (RTCisSyncing())
    ;
}

 /*
        Store max uint32[ 4 ] values to rtc ram
        rtc.set_backup(1, val_1);
          ...
        rtc.set_backup(4, val_1, val_2, val_3, val_4);
    */
  void RTCR34::setBackup(int n_values, ...)
    {
        n_values &= 3;
        va_list list;
        va_start(list, n_values);
        for (int i = 0; i < n_values; i++)
            RTC->MODE0.GP[i].reg = va_arg(list, uint32_t);
        va_end(list);
    }

    
      /* Restore max uint32[ 4 ] values from rtc ram
        rtc.get_backup(1, &val_1);
          ...
        rtc.get_backup(4, &val_1, &val_2, &val_3, &val_4);
	*/
   
    void RTCR34::getBackup(int n_values, ...)
    {
        n_values &= 3;
        va_list list;
        va_start(list, n_values);
        for (int i = 0; i < n_values; i++)
        {
            uint32_t *p = va_arg(list, uint32_t *);
            if (p)
                *p = RTC->MODE0.GP[i].reg;
        }
        va_end(list);
    }

void RTCR34::RTCsetMode()
{
	
	volatile RTC_MODE2_CTRLA_Type tmp_reg;
	tmp_reg.reg = 0;
	
		tmp_reg.reg |= RTC_MODE2_CTRLA_MODE_CLOCK; // set clock operating mode
		tmp_reg.reg |= RTC_MODE2_CTRLA_PRESCALER_DIV1024; // set prescaler to 1024 for MODE2
		tmp_reg.reg &= ~RTC_MODE2_CTRLA_MATCHCLR; // disable clear on match
		//According to the datasheet RTC_MODE2_CTRL_CLKREP = 0 for 24h
		tmp_reg.reg &= ~RTC_MODE2_CTRLA_CLKREP; // 24h time representation
		// enable clock sync ( read register )
		tmp_reg.reg |= RTC_MODE2_CTRLA_CLOCKSYNC;
	while (RTCisSyncing())
    ;
	RTC->MODE2.CTRLA.reg = tmp_reg.reg;
	while (RTCisSyncing())
    ;

}

void RTCR34::RTCenable()
{
	while (RTC->MODE2.SYNCBUSY.bit.ENABLE)
    ;
  RTC->MODE2.CTRLA.reg |= RTC_MODE2_CTRLA_ENABLE; // enable RTC
  while (RTC->MODE2.SYNCBUSY.bit.ENABLE)
    ;
}

void RTCR34::RTCreset()
{
  RTC->MODE2.CTRLA.reg |= RTC_MODE2_CTRLA_SWRST; // software reset
  while (RTCisSyncing())
    ;
}

void RTCR34::RTCresetRemove()
{
  RTC->MODE2.CTRLA.reg &= ~RTC_MODE2_CTRLA_SWRST; // software reset remove
  while (RTCisSyncing())
    ;
}
