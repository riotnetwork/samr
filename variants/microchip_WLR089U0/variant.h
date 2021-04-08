/*
  Copyright (c) 2014-2016 Arduino LLC.  All right reserved.

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

#ifndef _VARIANT_RIOT_R34_MCP
#define _VARIANT_RIOT_R34_MCP

// The definitions here needs a SAMD core >=1.6.10
#define ARDUINO_SAMD_VARIANT_COMPLIANCE 10610

/*----------------------------------------------------------------------------
 *        Definitions
 *----------------------------------------------------------------------------*/

/** Frequency of the board main oscillator */
#define VARIANT_MAINOSC		(32768ul)

/** Master clock frequency */
#define VARIANT_MCK			  (48000000ul)

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "WVariant.h"

#ifdef __cplusplus
#include "SERCOM.h"
#include "Uart.h"
#endif // __cplusplus

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/*----------------------------------------------------------------------------
 *        Pins
 *----------------------------------------------------------------------------*/

// Number of pins defined in PinDescription array
#ifdef __cplusplus
extern "C" unsigned int PINCOUNT_fn();
#endif
#define PINS_COUNT           (PINCOUNT_fn())
#define NUM_DIGITAL_PINS     (34u) 
#define NUM_ANALOG_INPUTS    (5u)
#define NUM_ANALOG_OUTPUTS   (0u)
// #define analogInputToDigitalPin(p)  ((p < NUM_ANALOG_INPUTS) ? (p) + NUM_DIGITAL_PINS : -1)

#define digitalPinToPort(P)        ( &(PORT->Group[g_APinDescription[P].ulPort]) )
#define digitalPinToBitMask(P)     ( 1 << g_APinDescription[P].ulPin )
//#define analogInPinToBit(P)        ( )
#define portOutputRegister(port)   ( &(port->OUT.reg) )
#define portInputRegister(port)    ( &(port->IN.reg) )
#define portModeRegister(port)     ( &(port->DIR.reg) )
#define digitalPinHasPWM(P)        ( g_APinDescription[P].ulPWMChannel != NOT_ON_PWM || g_APinDescription[P].ulTCChannel != NOT_ON_TIMER )

/*
 * digitalPinToTimer(..) is AVR-specific and is not defined for SAMD
 * architecture. If you need to check if a pin supports PWM you must
 * use digitalPinHasPWM(..).
 *
 * https://github.com/arduino/Arduino/issues/1833
 */
// #define digitalPinToTimer(P)

/*
 * Analog pins
 */
#define PIN_A0               (0ul)
#define PIN_A1               (1ul)
#define PIN_A2               (12ul)
#define PIN_A3               (17ul)


static const uint8_t A0  = PIN_A0;
static const uint8_t A1  = PIN_A1;
static const uint8_t A2  = PIN_A2;
static const uint8_t A3  = PIN_A3;

#define ADC_RESOLUTION		12


/*
 * Serial interfaces
 */
// Serial1 ( the UART ) 
#define PIN_SERIAL1_RX       (18ul)
#define PIN_SERIAL1_TX       (19ul)
#define PAD_SERIAL1_TX       (UART_TX_PAD_0)
#define PAD_SERIAL1_RX       (SERCOM_RX_PAD_1)

/*
 * SPI Interfaces
 */
#define SPI_INTERFACES_COUNT 2

// SPI connected to SX1276 lora radio
// SPI interface
#define PIN_SPI_MISO         (33u) // PC19 Sercom4 pad0
#define PIN_SPI_MOSI         (32u) // PB30 Sercom4 pad2
#define PIN_SPI_SCK          (34u) // PC18 Sercom4 pad3
#define PIN_SPI_SS           (31u) // PB31 gpio
#define PERIPH_SPI           sercom4
#define PAD_SPI_TX			  SPI_PAD_2_SCK_3
#define PAD_SPI_RX			  SERCOM_RX_PAD_0

static const uint8_t SS = PIN_SPI_SS;
static const uint8_t MOSI = PIN_SPI_MOSI;
static const uint8_t MISO = PIN_SPI_MISO;
static const uint8_t SCK = PIN_SPI_SCK;

// SPI interface ( SPI1 goes "outside")
#define PIN_SPI1_MISO         (13u)
#define PIN_SPI1_MOSI         (10u)
#define PIN_SPI1_SCK          (12u)
#define PIN_SPI1_SS           (11u)
#define PERIPH_SPI1           sercom5
#define PAD_SPI1_TX           SPI_PAD_2_SCK_3
#define PAD_SPI1_RX           SERCOM_RX_PAD_0

static const uint8_t SS1	  = PIN_SPI1_SS;
static const uint8_t MOSI1 = PIN_SPI1_MOSI;
static const uint8_t MISO1 = PIN_SPI1_MISO;
static const uint8_t SCK1  = PIN_SPI1_SCK;
/*
 * Wire Interfaces
 */
#define WIRE_INTERFACES_COUNT 1

#define PIN_WIRE_SDA         (3u)
#define PIN_WIRE_SCL         (4u)
#define PERIPH_WIRE          sercom1
#define WIRE_IT_HANDLER      SERCOM1_Handler

static const uint8_t SDA = PIN_WIRE_SDA;
static const uint8_t SCL = PIN_WIRE_SCL;
/*
// SERCOM2 can be routed to RF DIO1 and DIO2 ( PA11 , PA12) 
#define PIN_WIRE_SDA1         (23u)
#define PIN_WIRE_SCL1         (22u)
#define PERIPH_WIRE1          sercom1
#define WIRE_IT_HANDLER1      SERCOM1_Handler

static const uint8_t SDA = PIN_WIRE_SDA1;
static const uint8_t SCL = PIN_WIRE_SCL1;
*/
/*
 * USB
 */
#define PIN_USB_DM          (36u)
#define PIN_USB_DP          (35u)

 // Needed for LoRaWAN (sx1276) library
 // --------------------------------------
#define SX1276_BAND_SEL_PIN   (23u)
#define SX1276_RESET_PIN   (24u)
#define SX1276_TCXO_POWER_PIN   (7u)
#define SX1276_DIO0			(25u)
#define SX1276_DIO1			(26u)
#define SX1276_DIO2			(27u) 
#define SX1276_DIO3 		(28u)
#define SX1276_DIO4			(29u)
#define SX1276_DIO5 		(30u)
#define SX1276_SPI		SPI
#define SX1276_SPI_CS_PIN  PIN_SPI_SS

#ifdef __cplusplus
}
#endif

/*----------------------------------------------------------------------------
 *        Arduino objects - C++ only
 *----------------------------------------------------------------------------*/

#ifdef __cplusplus

/*	=========================
 *	===== SERCOM DEFINITION
 *	=========================
*/
extern SERCOM sercom0;
extern SERCOM sercom1;
extern SERCOM sercom2;
extern SERCOM sercom3;
extern SERCOM sercom4;
extern SERCOM sercom5;

//extern Uart Serial;
extern Uart Serial1;

#endif

#ifdef __cplusplus
extern "C" {
#endif
unsigned int PINCOUNT_fn();
#ifdef __cplusplus
}
#endif

// These serial port names are intended to allow libraries and architecture-neutral
// sketches to automatically default to the correct port name for a particular type
// of use.  For example, a GPS module would normally connect to SERIAL_PORT_HARDWARE_OPEN,
// the first hardware serial port whose RX/TX pins are not dedicated to another use.
//
// SERIAL_PORT_MONITOR        Port which normally prints to the Arduino Serial Monitor
//
// SERIAL_PORT_USBVIRTUAL     Port which is USB virtual serial
//
// SERIAL_PORT_LINUXBRIDGE    Port which connects to a Linux system via Bridge library
//
// SERIAL_PORT_HARDWARE       Hardware serial port, physical RX & TX pins.
//
// SERIAL_PORT_HARDWARE_OPEN  Hardware serial ports which are open for use.  Their RX & TX
//                            pins are NOT connected to anything by default.
#define SERIAL_PORT_USBVIRTUAL      SerialUSB
#define SERIAL_PORT_MONITOR         SerialUSB
// Serial has no physical pins broken out, so it's not listed as HARDWARE port
#define SERIAL_PORT_HARDWARE        Serial1
#define SERIAL_PORT_HARDWARE_OPEN   Serial1

// Legacy way to describe serial port on pins 0-1
#define Serial SerialUSB

#endif /* _VARIANT_ARDUINO_R34_MCP */

