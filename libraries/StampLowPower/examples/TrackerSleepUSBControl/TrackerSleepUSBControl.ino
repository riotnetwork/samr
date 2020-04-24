/*

This sketch tests sleep performance on the Riot tracker
Modes :

1 : sleep mode is IDLE , wake on button press interrupt
2 : sleep mode is STANDBY , wake on button press interrupt
3 : sleep mode is BACKUP , wake on RTC wakeup timer

** NOTE ON BACKUP MODE : lowest power consumption.
 use the RTC to wake the system, GPIO/ Pin interrupts dont work to wake the system from backup mode,
 only EXTWAKEx pins work ( A0 and A1 )
*/

#include "StampLowPower.h"
#include "SPI.h"

// Tracker has a RED LED on pin 5
#define LED_BUILTIN 5
// Pin used to trigger a wakeup
const int BUTTON_PIN = 9;


const int PERIPH_EN_PIN = 10;
const int VBATT_MONIT_EN = 13;
const int STAT_LIPO_PIN = 11;
const int VBUS_MONIT_PIN = A1;
const int VBATT_MONIT_PIN = A0;

const int RTC_SLEEP_TIME_MS = 15000; // how long "backup" sleep will last
volatile sleepModes_e sleepmode = SLEEP_IDLE;
volatile bool wokenUp = false;
uint32_t wakeupTime;

/*some basic settings for the SX1276 radio*/
SPISettings SX1276(2000000, MSBFIRST, SPI_MODE0);
#define OPMODE_REG            0x01
/* Value of Sleep mode in opmode register */
#define SLEEP_MODE           0x00
#define REG_WRITE_CMD        0x80


 // All pins are INPUT on power up by default
void setup() {
   pinMode(LED_BUILTIN, OUTPUT);
 pinMode(PERIPH_EN_PIN, OUTPUT);
  pinMode(VBATT_MONIT_EN, OUTPUT);
  pinMode(SX1276_RESET_PIN, OUTPUT);
  pinMode(SX1276_TCXO_POWER_PIN, OUTPUT);

//  pinMode(STAT_LIPO_PIN, INPUT);
  pinMode(VBUS_MONIT_PIN, INPUT);
  pinMode(VBATT_MONIT_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT);     // wakje on button press

  digitalWrite(LED_BUILTIN,HIGH);
  digitalWrite(PERIPH_EN_PIN, LOW);
  digitalWrite(VBATT_MONIT_EN, LOW);
  digitalWrite(SX1276_RESET_PIN, LOW);
  digitalWrite(SX1276_TCXO_POWER_PIN, LOW);

  SerialUSB.begin(115200);
   
    delay(1000);
   SerialUSB.println("Sleep system ready \n1: IDLE\n2: STANDBY\n3:BACKUP \ns:Go to SLEEP \n");
    wakeupTime = millis();
    delay(5000);
    SerialUSB.println(".");
    // Attach a wakeup interrupt on the Button pin
    LowPower.attachInterruptWakeup(BUTTON_PIN, buttonPress, CHANGE);
    LowPower.setSleepMode(sleepmode);
}


void loop() {
/*wake up, clear the interrupt value*/
  if (wokenUp)
  {
    wokenUp = false;  
    digitalWrite(LED_BUILTIN, HIGH);
  }

  // handle some Serial comms
  if (SerialUSB.available())
  {
    char c = SerialUSB.read();
    switch (c)
    {
      case '1':
        SerialUSB.println("Setting sleep mode to SLEEP_IDLE");
        sleepmode = SLEEP_IDLE;
        LowPower.setSleepMode(sleepmode);
      break;
      case '2':
        SerialUSB.println("Setting sleep mode to SLEEP_STANDBY");
        sleepmode = SLEEP_STANDBY;
        LowPower.setSleepMode(sleepmode);
      break;
      case '3':
        SerialUSB.println("Setting sleep mode to SLEEP_BACKUP");
        sleepmode = SLEEP_BACKUP;
        LowPower.setSleepMode(sleepmode);
      break;
      case 's':
      digitalWrite(VBATT_MONIT_EN,LOW);
      gotoSleep();
      break;
      default:
      break;
    }
  }
  delay(10);
  
}


/*Button press ISR*/
void buttonPress() {
    // This function will be called once on device wakeup, can do many things here, for now we just set some arbritrary bit
    wokenUp = true;
}

/*Prepare the syetem and go to teh required sleep mode*/
void gotoSleep ()
{
  SerialUSB.print("Going to sleep in ");
  uint32_t sleepSetting = PM->SLEEPCFG.bit.SLEEPMODE;
  switch (sleepSetting)
  {
    case PM_SLEEPCFG_SLEEPMODE_IDLE:
    SerialUSB.print("IDLE ");
    break;
    case PM_SLEEPCFG_SLEEPMODE_STANDBY:
    SerialUSB.print("STANDBY ");
    break;
    case PM_SLEEPCFG_SLEEPMODE_BACKUP:
    SerialUSB.print("BACKUP ");
    break;
    default:
    break;
  }
  SerialUSB.println("mode");
  setRadioSleep();
  SerialUSB.flush();
  delay(100); // give the serial / USB tome to finish the message
  SerialUSB.end(); // NB !! call this to stop the USB interrupts from happening and continuously waking the system


  digitalWrite(LED_BUILTIN, LOW);
    
  if (sleepmode == SLEEP_BACKUP) // lowest power mode
  {
    LowPower.sleep(RTC_SLEEP_TIME_MS); // wake up in a few seconds - will look like a POR / Reset/ reboot - Pin interrupts dont work in backup mode
  }
  else {
    LowPower.sleep(); // regular sleep, wait for ANY interrupt
  }
  turnRadioOn(); // Lora Radio is on
}


/* enable the SX1276 radio*/
void turnRadioOn() {
  pinMode(SX1276_RESET_PIN, OUTPUT);
  pinMode(SX1276_TCXO_POWER_PIN, OUTPUT);
  pinMode(SX1276_BAND_SEL_PIN, OUTPUT);
  pinMode(SX1276_SPI_CS_PIN, OUTPUT);
  SPI.begin();
  digitalWrite(SX1276_SPI_CS_PIN, HIGH);
  digitalWrite(SX1276_BAND_SEL_PIN, LOW);
  digitalWrite(SX1276_TCXO_POWER_PIN, HIGH);
  delay(5);
  digitalWrite(SX1276_RESET_PIN, HIGH); // Lora Radio is on
  delay(1);
  
}


/*set the SX1276 radio to low power mode*/
void setRadioSleep() {

  uint8_t current_mode,op_mode;
  uint8_t new_mode = SLEEP_MODE;

  op_mode = RADIO_RegisterRead(OPMODE_REG);
  current_mode = op_mode & 0x07;
  SerialUSB.print("Current radio mode is ");
  SerialUSB.println(current_mode);

  if (new_mode != current_mode)
  {
    // Do the actual mode switch.
    op_mode &= ~0x07;                // Clear old mode bits
    op_mode |= new_mode;              // Set new mode bits
    while (op_mode != RADIO_RegisterRead(OPMODE_REG))
    {
      RADIO_RegisterWrite(OPMODE_REG, op_mode);
    }
  }

  op_mode = RADIO_RegisterRead(OPMODE_REG);
  current_mode = op_mode & 0x07;
  SerialUSB.print("New radio mode is ");
  SerialUSB.println(current_mode);

  SPI.end();
}


/**
 * \brief This function is used to write a byte of data to the radio register
 * \param[in] reg Radio register to be written
 * \param[in] value Value to be written into the radio register
 */
void RADIO_RegisterWrite(uint8_t reg, uint8_t value)
{
  SPI.beginTransaction(SX1276);
  digitalWrite(SX1276_SPI_CS_PIN, LOW);
  SPI.transfer(REG_WRITE_CMD | reg);
  SPI.transfer(value);
  digitalWrite(SX1276_SPI_CS_PIN, HIGH);
  SPI.endTransaction();
}

/**
 * \brief This function is used to read a byte of data from the radio register
 * \param[in] reg Radio register to be read
 * \retval  Value read from the radio register
 */
uint8_t RADIO_RegisterRead(uint8_t reg)
{
  uint8_t readValue;
  reg &= 0x7F;    // Make sure write bit is not set

  SPI.beginTransaction(SX1276);
  digitalWrite(SX1276_SPI_CS_PIN, LOW);
  SPI.transfer(reg);
  readValue = SPI.transfer(0xFF);
  digitalWrite(SX1276_SPI_CS_PIN, HIGH);
  SPI.endTransaction();
  return readValue;
}
