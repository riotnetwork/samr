/*
  Copyright (c) 2014-2015 Arduino LLC.  All right reserved.

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
/*
 *
 * + Pin number +   Board pin	   |  PIN   | Label/Name      | Comments (* is for default peripheral in use)
 * +------------+------------------+--------+-----------------+------------------------------
 * |            | Digital (Low)    |        |                 |
 * +------------+------------------+--------+-----------------+------------------------------
 * | 0          | 0 -> RX          |  PA05  |                 | EIC/EXTINT[5] ADC/AIN[5] *SERCOM0/PAD[1] TCC0/WO[1] CCL0 IN[1]
 * | 1          | 1 <- TX          |  PA04  |                 | EIC/EXTINT[4] ADC/AIN[4] *SERCOM0/PAD[0] TCC0/WO[0] CCL0 IN[0]
 * | 2          | ~2               |  PB02  |                 | EIC/EXTINT[2] ADC/AIN[10] SERCOM5/PAD[0] *TC2/WO[0] SUPC/OUT[1] CCL0/OUT[0
 * | 3          | 3                |  PA28  |                 | EIC/EXTINT[8] GCLK_IO[0] 
 * | 4          | ~4               |  PB23  |                 | EIC/EXTINT[7] SERCOM5/PAD[3] *TC3/WO[1] GCLK_IO[1] CCL0/OUT[0]
 * | 5          | ~5               |  PA23  |                 | EIC/EXTINT[7] PTC/X[11] SERCOM3/PAD[1] SERCOM5/PAD[1] TC0/WO[1] *TCC0/WO[5] USB/SOF 1kHz[6] GCLK_IO[7] CCL2/ IN[1]
 * | 6          | ~6               |  PA22  |                 | EIC/EXTINT[6] PTC/X[10] SERCOM3/PAD[0] SERCOM5/PAD[0] TC0/WO[0] *TCC0/WO[4] GCLK_IO[6] CCL2/ IN[0]
 * | 7          | 7                |  PA27  |                 | EIC/EXTINT[15] GCLK_IO[0]
 * | 8          | 8                |  PA14  |                 | EIC/EXTINT[14] SERCOM2/PAD[2] SERCOM4/PAD[2] TC4/WO[0] ```/WO[4] GCLK_IO[0]
 * | 9          | ~9               |  PB22  |                 | EIC/EXTINT[6] SERCOM5/PAD[2] *TC3/WO[0] GCLK_IO[0] CCL0/ IN[0]
 * +------------+------------------+--------+-----------------+------------------------------
 * |            | SPI             |                 |
 * +------------+------------------+--------+-----------------+------------------------------
 * | 10         | 10               |  PA19  |  SPI SCK        | EIC/EXTINT[3] PTC/X[7] SERCOM1/PAD[3] *SERCOM3/PAD[3] TC4/WO[1] TCC0/WO[3] AC/CMP[1] CCL0/OUT[0]
 * | 11         | 11               |  PA18  |  SPI MOSI       | EIC/EXTINT[2] PTC/X[6] SERCOM1/PAD[2] *SERCOM3/PAD[2] TC4/WO[0] TCC0/WO[2] AC/CMP[0] CCL0/IN[2]
 * | 12         | 12               |  PA17  |  SPI nSS        | EIC/EXTINT[1] PTC/X[5] SERCOM1/PAD[1] *SERCOM3/PAD[1] TCC2/WO[1] TCC0/WO[1] GCLK_IO[3] CCL0/IN[1]
 * | 13         | 13               |  PA16  |  SPI MISO       | EIC/EXTINT[0] PTC/X[4] SERCOM1/PAD[0] *SERCOM3/PAD[0] TCC2/WO[0] TCC0/WO[6] GCLK_IO[2] CCL0/IN[0]             
 * +------------+------------------+--------+-----------------+------------------------------
 * |            | I2C              |        |                 |
 * +------------+------------------+--------+-----------------+------------------------------
 * | 14         | 14               |  PA08  |  I2C SDA        | NMI ADC/AIN[16] PTC/X[0] PTC/Y[6] SERCOM0/PAD[0] *SERCOM2/PAD[0] TCC0/WO[0] TCC1/WO[2] CCL1/IN[3]
 * | 15         | 15               |  PA09  |  I2C SCL        | EIC/EXTINT[9] ADC/AIN[17] PTC/X[1] PTC/Y[7] SERCOM0/PAD[1] *SERCOM2/PAD[1] TCC0/WO[1] TCC1/WO[3] CCL1/ IN[1] 
 * +------------+------------------+--------+-----------------+------------------------------
 * |            | Analog pins      |        |                 |
 * +------------+------------------+--------+-----------------+------------------------------
 * | 16         | A0               |  PA07  |                 | EIC/EXTINT[7] EXTWAKE[7] *ADC/AIN[7] AC/AIN[3] SERCOM0/PAD[3] TCC1/WO[1] CCL0/OUT[0]
 * | 17         | A1               |  PA06  |                 | EIC/EXTINT[6] EXTWAKE[6] *ADC/AIN[6] AC/AIN[2] PTC/Y[4] SERCOM0/PAD[2] TCC1/WO[0] CCL0/ IN[2]
 * +------------+------------------+--------+-----------------+------------------------------
 * |            | SX1276 Control   |        |                 |
 * +------------+------------------+--------+-----------------+------------------------------
 * | 18         | 18               |  PA13  | RF_BAND_SEL     | EIC/EXTINT[13] SERCOM2/PAD[1] SERCOM4/PAD[1] TCC2/WO[1] TCC0/WO[7] AC/CMP[1]
 * | 19         | 19               |  PB15  | RF_RESET        | EIC/EXTINT[15] PTC/X[15] SERCOM4/PAD[3] TCC0/WO[1] GCLK_IO[1] CCL3/IN[10]
 * | 20         | 20               |  PA15  | RF_TCXO_POW     | EIC/EXTINT[15] SERCOM2/PAD[3] SERCOM4/PAD[3] TC4/WO[1] TCC0/WO[5] GCLK_IO[1]
 * | 21         | 21               |  PB16  | RF_D0           | EIC/EXTINT[0]
 * | 22         | 22               |  PA11  | RF_D1  SCK      | EIC/EXTINT[11] (SERCOM_ALT)SERCOM2/PAD[3] ( Sercom2 used by default as I2C on PA08 )
 * | 23         | 23               |  PA12  | RF_D2  SDA      | EIC/EXTINT[12] SERCOM2/PAD[0] ( Sercom2 used by default as I2C on PA09 )
 * | 24         | 24               |  PB17  | RF_D3           | EIC/EXTINT[1]
 * | 25         | 25               |  PA10  | RF_D4           | EIC/EXTINT[10]
 * | 26         | 26               |  PB00  | RF_D5           | EIC/EXTINT[0]
 * | 27         | 27               |  PB31  | RF SPI CS       | EIC/EXTINT[15]
 * | 28         | 28               |  PB30  | RF SPI MOSI     | SERCOM4/PAD[2]
 * | 29         | 29               |  PC19  | RF SPI MISO     | SERCOM4/PAD[0]
 * | 30         | 30               |  PC18  | RF SPI SCK      | SERCOM4/PAD[3]
 * +------------+------------------+--------+-----------------+------------------------------
 * |            | USB              |        |                 |
 * +------------+------------------+--------+-----------------+------------------------------
 * | 31         |                  |  PA24  | USB_NEGATIVE    | USB/DM
 * | 32         |                  |  PA25  | USB_POSITIVE    | USB/DP
 * +------------+------------------+--------+-----------------+------------------------------
 * |            | VBAT             |        |                 |
 * +------------+------------------+--------+-----------------+------------------------------
 * | 33         |                  |  PB03  |                 | EIC/EXTINT[3] ADC/AIN[11] SERCOM5/PAD[1] TC2/WO[1] *SUPC/VBAT
 * +------------+------------------+--------+-----------------+------------------------------
 * |            |32.768KHz Crystal |        |                 |
 * +------------+------------------+--------+-----------------+------------------------------
 * |            |                  |  PA00  | XIN32           | EXTINT[0] SERCOM1/PAD[0] TCC2/WO[0]
 * |            |                  |  PA01  | XOUT32          | EXTINT[1] SERCOM1/PAD[1] TCC2/WO[1]
 * +------------+------------------+--------+-----------------+------------------------------
 */


#include "variant.h"

/*
 * Pins descriptions ( default and possible configs )
 */
const PinDescription g_APinDescription[]=
{
  // 0 .. 17 - externally available Digital pins
  // ----------------------
  // 0/1 - SERCOM/UART (Serial1)
  { PORTA, 5, PIO_SERCOM_ALT, PIN_ATTR_DIGITAL, No_ADC_Channel,NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_5 }, // RX: SERCOM0/PAD[3]
  { PORTA, 4, PIO_SERCOM_ALT, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_4 }, // TX: SERCOM0/PAD[2]
  // 2..9 GPio
  { PORTB,  2, PIO_TIMER, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER), ADC_Channel10, PWM4_CH0, TC2_CH0, EXTERNAL_INT_2}, // EXTERNAL_INT_ 2 TC2/WO[0]
  { PORTA, 28, PIO_DIGITAL, PIN_ATTR_DIGITAL,No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_8 }, // DIO only, interrupt 8
  { PORTB, 23, PIO_TIMER, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER), No_ADC_Channel, PWM5_CH1, TC3_CH1, EXTERNAL_INT_7 }, // TC3/WO[1]
  { PORTA, 23, PIO_TIMER_ALT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER_ALT), No_ADC_Channel, PWM0_CH5, TCC0_CH5, EXTERNAL_INT_7 }, // TCC0/WO[5]
  { PORTA, 22, PIO_TIMER_ALT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER_ALT), No_ADC_Channel, PWM0_CH4, TCC0_CH4, EXTERNAL_INT_6 }, // TCC0/WO[4]
  { PORTA, 27, PIO_DIGITAL, PIN_ATTR_DIGITAL,No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_15 }, // DIO only, interrupt 15  
  { PORTA, 14, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel,NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_14 }, // DIO only, interrupt 14
  { PORTB, 22, PIO_TIMER, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER), No_ADC_Channel, PWM5_CH0, TC3_CH0, EXTERNAL_INT_6 }, // TC3/WO[0]
   // 10..13 SPI  
  { PORTA, 19, PIO_SERCOM_ALT, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_3 }, // SCK : SERCOM3/PAD[3]
  { PORTA, 18, PIO_SERCOM_ALT, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_2 }, // MOSI: SERCOM3/PAD[2]
  { PORTA, 17, PIO_SERCOM_ALT, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_1 }, // nSS : SERCOM3/PAD[1]
  { PORTA, 16, PIO_SERCOM_ALT, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_0 }, // MISO: SERCOM3/PAD[0]
   // 14/15 I2C
  { PORTA,  8, PIO_SERCOM_ALT, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NMI }, // SDA: SERCOM2/PAD[0]
  { PORTA,  9, PIO_SERCOM_ALT, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_9 }, // SCL: SERCOM2/PAD[1]
  // 16/17 Analog pins
  { PORTA,  7, PIO_ANALOG, PIN_ATTR_ANALOG, ADC_Channel7, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_7 }, // ADC/AIN[7]
  { PORTA,  6, PIO_ANALOG, PIN_ATTR_ANALOG, ADC_Channel6, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_6 }, // ADC/AIN[6]	  
 // 18..20 SX1276 RF radio control  
  { PORTA, 13, PIO_OUTPUT, PIN_ATTR_DIGITAL, No_ADC_Channel,  NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // RF Band Select, output only
  { PORTB, 15, PIO_OUTPUT, PIN_ATTR_DIGITAL, No_ADC_Channel,  NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // RF reset, output only
  { PORTA, 15, PIO_OUTPUT, PIN_ATTR_DIGITAL, No_ADC_Channel,  NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // RF TCXO power control, output only
 // 21..26 SX1276 RF radio DIO pins	  
  { PORTB, 16, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel,NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_0 }, // SX DIO 0, interrupt 0
  { PORTA, 11, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel,NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_11 }, // SX DIO 1, interrupt 11 
  { PORTA, 12, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel,NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_12 }, // SX DIO 2, interrupt 12 
  { PORTB, 17, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel,NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_1 }, // SX DIO 3, interrupt 1
  { PORTA, 10, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel,NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_10 }, // SX DIO 4, interrupt 10 
  { PORTB,  0, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel,NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_0 }, // SX DIO 5, interrupt 0 
 // 27..30 SX1276 RF radio SPI pins
  { PORTB, 31, PIO_OUTPUT, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // CS
  { PORTB, 30, PIO_PERIPH_F, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // MOSI : SERCOM4/PAD[2]
  { PORTC, 19, PIO_PERIPH_F, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // MISO : SERCOM4/PAD[0]
  { PORTC, 18, PIO_PERIPH_F, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // SCK : SERCOM4/PAD[1]
  // 31/32  USB
  { PORTA, 24, PIO_COM, PIN_ATTR_NONE, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // USB/DM
  { PORTA, 25, PIO_COM, PIN_ATTR_NONE, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // USB/DP
  // 33 VBAT
  { PORTB, 3, PIO_PERIPH_H, PIN_ATTR_ANALOG, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // VBAT in
 // ----------------------
} ;

extern "C" {
    unsigned int PINCOUNT_fn() {
        return (sizeof(g_APinDescription) / sizeof(g_APinDescription[0]));
    }
}

const void* g_apTCInstances[TCC_INST_NUM+TC_INST_NUM]={ TCC0, TCC1, TCC2, TC1, TC2, TC3, TC4 } ;

// Multi-serial objects instantiation
SERCOM sercom0( SERCOM0 ) ;
SERCOM sercom1( SERCOM1 ) ;
SERCOM sercom2( SERCOM2 ) ;
SERCOM sercom3( SERCOM3 ) ;
SERCOM sercom4( SERCOM4 ) ;
SERCOM sercom5( SERCOM5 ) ;

Uart Serial1( &sercom0, PIN_SERIAL1_RX, PIN_SERIAL1_TX, PAD_SERIAL1_RX, PAD_SERIAL1_TX ) ;

void SERCOM0_Handler()
{
  Serial1.IrqHandler();
}
