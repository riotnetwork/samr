/*

This sketch tests sleep performance 
Modes :
0 : sleep, wake on interrupt, turn LED on for 5 Seconds ,sleep mode afterwards is default ( idle )
1 : sleep, wake on interrupt, turn LED on for 5 Seconds ,sleep mode afterwards is idle
2 : sleep, wake on interrupt, turn LED on for 5 Seconds ,sleep mode afterwards is Standby
3 : sleep, wake on interrupt, turn LED on for 5 Seconds ,sleep mode afterwards is backup

** NOTE ON BACKUP MODE : lowest power consumption.
 use the RTC to wake the system, GPIO/ Pin interrupts dont work to wake the system from backup mode,
 only EXTWAKEx pins work ( A0 and A1 )
*/

#include "StampLowPower.h"

// Tracker has a RED LED on pin 5
#define LED_BUILTIN 5
// Pin used to trigger a wakeup
const int BUTTON_PIN = 9;


const int PERIPH_EN_PIN = 10;
const int VBATT_MONIT_EN = 13;
const int STAT_LIPO_PIN = 11;
const int VBUS_MONIT_PIN = A1;
const int VBATT_MONIT_PIN = A0;

const int STAY_AWAKE_MS = 5000;
volatile sleepModes_e sleepmode = SLEEP_IDLE;
volatile bool wokenUp = false;
uint32_t wakeupTime;

 // All pins are INPUT on power up by default
void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
 //   pinMode(PERIPH_EN_PIN, OUTPUT);
    pinMode(VBATT_MONIT_EN, OUTPUT);
    pinMode(SX1276_RESET_PIN, OUTPUT);
    pinMode(SX1276_TCXO_POWER_PIN, OUTPUT);

    pinMode(BUTTON_PIN, INPUT); // has external pulldown, and will pull high on press

    digitalWrite(LED_BUILTIN,HIGH); // LED on
    digitalWrite(VBATT_MONIT_EN, LOW);
    digitalWrite(SX1276_RESET_PIN, LOW);
    digitalWrite(SX1276_TCXO_POWER_PIN, LOW);

    SerialUSB.begin(115200);
   
    delay(5000);
    SerialUSB.println("Sleep system ready \n0: default (IDLE)\n1: IDLE\n2: STANDBY\n3: BACKUP\n");
    wakeupTime = millis();
    delay(5000);
    SerialUSB.println(".");
    // Attach a wakeup interrupt on pin 8, calling repetitionsIncrease when the device is woken up
    LowPower.attachInterruptWakeup(BUTTON_PIN, buttonPress, CHANGE);
    LowPower.setSleepMode(sleepmode);
}


void loop() {


    if (SerialUSB.available())
    {
        char c = SerialUSB.read();
        switch (c)
        {
        case '0':
            SerialUSB.println("Setting sleep mode to IDLE");
            sleepmode = SLEEP_IDLE;
            break;

        case '1':
            SerialUSB.println("Setting sleep mode to IDLE");
            sleepmode = SLEEP_IDLE;
            break;

        case '2':
            SerialUSB.println("Setting sleep mode to STANBY");
            sleepmode = SLEEP_STANDBY;
            break;

        case '3':
            SerialUSB.println("Setting sleep mode to BACKUP");
            sleepmode = SLEEP_BACKUP;
            break;

        default:
            break;
        }

        LowPower.setSleepMode(sleepmode);
    }

    /*wake up, clear the interrupt value*/
    if (wokenUp)
    {
        wokenUp = false;
        wakeupTime = millis();
        digitalWrite(LED_BUILTIN, HIGH);
        SerialUSB.println("Awake !");
    }

    /*if its time to sleep again, go back to sleep*/
    if (((millis() - wakeupTime) >= STAY_AWAKE_MS))
    {
        SerialUSB.print("Going to sleep in ");
        switch (sleepmode)
        {
        case SLEEP_IDLE:
            SerialUSB.print("IDLE ");
            break;
        case SLEEP_STANDBY:
            SerialUSB.print("STANDBY ");
            break;
        case SLEEP_BACKUP:
            SerialUSB.print("BACKUP ");
            break;
        default:    
            break;
        }
        SerialUSB.println("mode");
        SerialUSB.flush();
        delay(100); // give the serial / USB tome to finish the message 
        SerialUSB.end(); // NB !! call this to stop the USB interrupts from happening and continuously waking the system


        digitalWrite(LED_BUILTIN, LOW);
        
        if (sleepmode == SLEEP_BACKUP) // lowest power mode
        {
            LowPower.sleep(5000); // wake up in 5 seconds - will look like a POR / Reset/ reboot - Pin interrupts dont work in backup mode
        }
        else {
            LowPower.sleep(); // regular sleep, wait for ANY interrupt
        }
    }
}

void buttonPress() {
    // This function will be called once on device wakeup, can do many things here, for now we just set some arbritrary bit
    wokenUp = true;
}