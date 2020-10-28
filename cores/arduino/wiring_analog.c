/*
  Copyright (c) 2014 Arduino LLC.  All right reserved.

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
#include "wiring_private.h"

#ifdef __cplusplus
extern "C" {
#endif

static int _readResolution = 10;
static int _ADCResolution = 10;
static int _writeResolution = 8;

// Wait for synchronization of registers between the clock domains
static __inline__ void syncADC(uint8_t syncbit) __attribute__((always_inline, unused));
static void syncADC(uint8_t syncbit) {
	
  while (((ADC->SYNCBUSY.reg & ADC_SYNCBUSY_MASK ) &  (_U_(0x1) << syncbit)) == 1)  // read the corresponding register's sync bit 
    ;
}

// Wait for synchronization of registers between the clock domains
static __inline__ void syncDAC() __attribute__((always_inline, unused));
static void syncDAC() {
  while (DAC->SYNCBUSY.bit.SWRST == 1)
    ;
}

// Wait for synchronization of registers between the clock domains
static __inline__ void syncTC_16(Tc* TCx) __attribute__((always_inline, unused));
static void syncTC_16(Tc* TCx) {
  while (TCx->COUNT16.SYNCBUSY.bit.STATUS);
}

// Wait for synchronization of registers between the clock domains
static __inline__ void syncTCC(Tcc* TCCx) __attribute__((always_inline, unused));
static void syncTCC(Tcc* TCCx) {
  while (TCCx->SYNCBUSY.reg & TCC_SYNCBUSY_MASK);
}

void analogReadResolution(int res)
{
  _readResolution = res;
  if (res > 10) {
    ADC->CTRLC.bit.RESSEL = ADC_CTRLC_RESSEL_12BIT_Val;
    _ADCResolution = 12;
  } else if (res > 8) {
    ADC->CTRLC.bit.RESSEL = ADC_CTRLC_RESSEL_10BIT_Val;
    _ADCResolution = 10;
  } else {
    ADC->CTRLC.bit.RESSEL = ADC_CTRLC_RESSEL_8BIT_Val;
    _ADCResolution = 8;
  }
  syncADC(ADC_SYNCBUSY_CTRLC_Pos);
}

void analogWriteResolution(int res)
{
  _writeResolution = res;
}

static inline uint32_t mapResolution(uint32_t value, uint32_t from, uint32_t to)
{
  if (from == to) {
    return value;
  }
  if (from > to) {
    return value >> (from-to);
  }
  return value << (to-from);
}

/*
 * Internal Reference is at 1.0v
 * External Reference should be between 1v and VDDANA-0.6v=2.7v
 *
 * Warning : On Arduino Zero board the input/output voltage for SAMD21G18 is 3.3 volts maximum
 */
void analogReference(eAnalogReference mode)
{
  syncADC(ADC_SYNCBUSY_ENABLE_Pos);
  switch (mode)
  {
    case AR_INTERNAL:
    case AR_INTERNAL2V00:
      ADC->REFCTRL.bit.REFSEL = ADC_REFCTRL_REFSEL_INTVCC0_Val; // 1/1.6 VDDANA = 1/1.6* 3V3 = 2.0625
      break;

    case AR_EXTERNAL:
      ADC->REFCTRL.bit.REFSEL = ADC_REFCTRL_REFSEL_AREFB_Val;
      break;

    case AR_INTERNAL1V65:
      ADC->REFCTRL.bit.REFSEL = ADC_REFCTRL_REFSEL_INTVCC1_Val; // 1/2 VDDANA = 0.5* 3V3 = 1.65V
      break;

    case AR_DEFAULT:
    default:
      ADC->REFCTRL.bit.REFSEL = ADC_REFCTRL_REFSEL_INTVCC1_Val; // 1/2 VDDANA = 0.5* 3V3 = 1.65V
      break;
  }
}

uint32_t analogRead(uint32_t pin)
{
  uint32_t valueRead = 0;

  if (pin < A0) {
    pin += A0;
  }

  pinPeripheral(pin, PIO_ANALOG);

  // Disable DAC, if analogWrite() was used previously to enable the DAC
  if ((g_APinDescription[pin].ulADCChannelNumber == ADC_Channel0) || (g_APinDescription[pin].ulADCChannelNumber == DAC_Channel0)) {
    syncDAC();
    DAC->CTRLA.bit.ENABLE = 0x00; // Disable DAC
    //DAC->CTRLB.bit.EOEN = 0x00; // The DAC output is turned off.
    syncDAC();
  }

  syncADC(ADC_SYNCBUSY_ENABLE_Pos);
  ADC->INPUTCTRL.bit.MUXPOS = g_APinDescription[pin].ulADCChannelNumber; // Selection for the positive ADC input

  // Control A
  /*
   * Bit 1 ENABLE: Enable
   *   0: The ADC is disabled.
   *   1: The ADC is enabled.
   * Due to synchronization, there is a delay from writing CTRLA.ENABLE until the peripheral is enabled/disabled. The
   * value written to CTRL.ENABLE will read back immediately and the Synchronization Busy bit in the Status register
   * (STATUS.SYNCBUSY) will be set. STATUS.SYNCBUSY will be cleared when the operation is complete.
   *
   * Before enabling the ADC, the asynchronous clock source must be selected and enabled, and the ADC reference must be
   * configured. The first conversion after the reference is changed must not be used.
   */
  ADC->CTRLA.bit.ONDEMAND = 1;
  ADC->CTRLA.bit.ENABLE = 0x01;             // Enable ADC

  // Start conversion
  syncADC(ADC_SYNCBUSY_ENABLE_Pos);
  ADC->SWTRIG.bit.START = 1;

  // Clear the Data Ready flag
  ADC->INTFLAG.reg = ADC_INTFLAG_RESRDY;

  // Start conversion again, since The first conversion after the reference is changed must not be used.
  syncADC(ADC_SYNCBUSY_SWTRIG_Pos);
  ADC->SWTRIG.bit.START = 1;

  // Store the value
  while (ADC->INTFLAG.bit.RESRDY == 0);   // Waiting for conversion to complete
  valueRead = ADC->RESULT.reg;

  syncADC(ADC_SYNCBUSY_SWTRIG_Pos);
  ADC->CTRLA.bit.ENABLE = 0x00;             // Disable ADC
  syncADC(ADC_SYNCBUSY_ENABLE_Pos);

  return mapResolution(valueRead, _ADCResolution, _readResolution);
}


// Right now, PWM output only works on the pins with
// hardware support.  These are defined in the appropriate
// pins_*.c file.  For the rest of the pins, we default
// to digital output.
void analogWrite(uint32_t pin, uint32_t value)
{
  PinDescription pinDesc = g_APinDescription[pin];
  uint32_t attr = pinDesc.ulPinAttribute;

  if ((attr & PIN_ATTR_ANALOG) == PIN_ATTR_ANALOG)
  {
    // DAC handling code

    if ((pinDesc.ulADCChannelNumber != ADC_Channel0) && (pinDesc.ulADCChannelNumber != DAC_Channel0)) { // Only 1 DAC on AIN0 / PA02
      return;
    }

    value = mapResolution(value, _writeResolution, 10);

    syncDAC();
    DAC->DATA->reg = DAC_DATA_DATA(value & 0x3FF);  // DAC on 10 bits.
    syncDAC();
    DAC->CTRLA.bit.ENABLE = 0x01;     // Enable DAC
    syncDAC();
    return;
  }

  if ((attr & PIN_ATTR_PWM) == PIN_ATTR_PWM)
  {
    value = mapResolution(value, _writeResolution, 16);

    uint32_t tcNum = GetTCNumber(pinDesc.ulPWMChannel);
    uint8_t tcChannel = GetTCChannelNumber(pinDesc.ulPWMChannel);
    static bool tcEnabled[TCC_INST_NUM+TC_INST_NUM];

    if (attr & PIN_ATTR_TIMER) {
      #if !(ARDUINO_SAMD_VARIANT_COMPLIANCE >= 10603)
      // Compatibility for cores based on SAMD core <=1.6.2
      if (pinDesc.ulPinType == PIO_TIMER_ALT) {
        pinPeripheral(pin, PIO_TIMER_ALT);
      } else
      #endif
      {
        pinPeripheral(pin, PIO_TIMER);
      }
    } else {
      // We suppose that attr has PIN_ATTR_TIMER_ALT bit set...
      pinPeripheral(pin, PIO_TIMER_ALT);
    }
	


    if (!tcEnabled[tcNum]) {
      tcEnabled[tcNum] = true;

      uint16_t GCLK_CLKCTRL_IDs[] = {
		  GCM_TCC0_TCC1,// TCC0
		  GCM_TCC0_TCC1,// TCC1
		  GCM_TCC2,		// TCC2
		//  GCM_TC0_TC1,	// TC0 is not used for pwm / timer
		  GCM_TC0_TC1,	// TC1
		  GCM_TC2_TC3,	// TC2
		  GCM_TC2_TC3,	// TC3
		  GCM_TC4,		// TC4
      };
	  
	  GCLK->PCHCTRL[GCLK_CLKCTRL_IDs[tcNum]].reg = ( GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK0 );

      while ( (GCLK->PCHCTRL[GCLK_CLKCTRL_IDs[tcNum]].reg & GCLK_PCHCTRL_CHEN) == 0 );        // wait for sync

      // Set PORT
      if (tcNum >= TCC_INST_NUM) {
        // -- Configure TC
        Tc* TCx = (Tc*) GetTC(pinDesc.ulPWMChannel);
        // Disable TCx
        TCx->COUNT16.CTRLA.bit.ENABLE = 0;
        syncTC_16(TCx);
        // Set Timer counter Mode to 16 bits, normal PWM
		TCx->COUNT16.CTRLA.bit.ONDEMAND = 1;
        TCx->COUNT16.CTRLA.reg |= TC_CTRLA_MODE_COUNT16;
		TCx->COUNT16.WAVE.reg = TC_WAVE_WAVEGEN_NPWM; // set to normal PWM mode
        syncTC_16(TCx);
        // Set the initial value
        TCx->COUNT16.CC[tcChannel].reg = (uint32_t) value;
        syncTC_16(TCx);
        // Enable TCx
        TCx->COUNT16.CTRLA.bit.ENABLE = 1;
        syncTC_16(TCx);
      } else {
        // -- Configure TCC
        Tcc* TCCx = (Tcc*) GetTC(pinDesc.ulPWMChannel);
        // Disable TCCx
        TCCx->CTRLA.bit.ENABLE = 0;
        syncTCC(TCCx);
        // Set TCCx as normal PWM
        TCCx->WAVE.reg |= TCC_WAVE_WAVEGEN_NPWM;
        syncTCC(TCCx);
        // Set the initial value
        TCCx->CC[tcChannel].reg = (uint32_t) value;
        syncTCC(TCCx);
        // Set PER to maximum counter value (resolution : 0xFFFF)
        TCCx->PER.reg = 0xFFFF;
        syncTCC(TCCx);
        // Enable TCCx
        TCCx->CTRLA.bit.ENABLE = 1;
        syncTCC(TCCx);
      }
    } else {
      if (tcNum >= TCC_INST_NUM) {
        Tc* TCx = (Tc*) GetTC(pinDesc.ulPWMChannel);
        TCx->COUNT16.CC[tcChannel].reg = (uint32_t) value;
        syncTC_16(TCx);
      } else {
        Tcc* TCCx = (Tcc*) GetTC(pinDesc.ulPWMChannel);
        TCCx->CTRLBSET.bit.LUPD = 1;
        syncTCC(TCCx);
        TCCx->CC[tcChannel].reg = TCC_CC_CC((uint32_t) value);
        syncTCC(TCCx);
        TCCx->CTRLBCLR.bit.LUPD = 1;
        syncTCC(TCCx);
      }
    }
    return;
  }

  // -- Defaults to digital write
  pinMode(pin, OUTPUT);
  value = mapResolution(value, _writeResolution, 8);
  if (value < 128) {
    digitalWrite(pin, LOW);
  } else {
    digitalWrite(pin, HIGH);
  }
}

#ifdef __cplusplus
}
#endif
