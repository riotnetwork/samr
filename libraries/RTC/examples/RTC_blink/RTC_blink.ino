#include <RTCR34.h>

#define LED_BUILTIN 8
/* Create an rtc object */
RTCR34 rtc;

/* Change these values to set the current initial time */
const byte seconds = 0;
const byte minutes = 00;
const byte hours = 17;

/* Change these values to set the current initial date */
const byte day = 25;
const byte month = 03;
const byte year = 20;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(2000);
  digitalWrite(LED_BUILTIN, LOW);
  
  rtc.begin();
  rtc.setTime(hours, minutes, seconds);
  rtc.setDate(day, month, year);

  rtc.setAlarmTime(17, 00, 10);
  rtc.enableAlarm(rtc.MATCH_HHMMSS);
  rtc.attachInterrupt(alarmMatch);
    
   rtc.standbyMode();
}
 bool alarmWent = false;

void loop()
{
  if (alarmWent)
  {
    alarmWent = false;
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    uint32_t now = rtc.getEpoch();
    rtc.setAlarmEpoch(now + 2); // next alarm in 2 seconds
    rtc.enableAlarm(rtc.MATCH_MMSS);
    rtc.attachInterrupt(alarmMatch);    
  }

   rtc.standbyMode();    // Sleep until next alarm match
}

void alarmMatch()
{
  alarmWent = true;
}
