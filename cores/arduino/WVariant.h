/*
  Copyright (c) 2015 Arduino LLC.  All right reserved.

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

#pragma once

#include <stdint.h>
#include <sam.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Definitions and types for pins */
typedef enum _EAnalogChannel
{
  No_ADC_Channel=-1,
  ADC_Channel0=0,
  ADC_Channel4=4,
  ADC_Channel5=5,
  ADC_Channel6=6,
  ADC_Channel7=7,
  ADC_Channel10=10,
  ADC_Channel11=11,
  ADC_Channel16=16,
  ADC_Channel17=17,
  DAC_Channel0,
} EAnalogChannel ;

// Definitions for TC channels ( select the  Capture compare channel ( CC ) for each waveform output )
// TCCx have 4 CC channels, TCx only has 2
typedef enum _ETCChannel
{
  NOT_ON_TIMER=-1,
  
  TCC0_CH0 = (0<<8)|(0),
  TCC0_CH1 = (0<<8)|(1),
  TCC0_CH2 = (0<<8)|(2),
  TCC0_CH3 = (0<<8)|(3),
  TCC0_CH4 = (0<<8)|(0), // Channel 4 is CC 0!
  TCC0_CH5 = (0<<8)|(1), // Channel 5 is CC 1!
  TCC0_CH6 = (0<<8)|(2), // Channel 6 is CC 2!
  TCC0_CH7 = (0<<8)|(3), // Channel 7 is CC 3!
  
  TCC1_CH0 = (1<<8)|(0),
  TCC1_CH1 = (1<<8)|(1),
  TCC1_CH2 = (1<<8)|(0), // Channel 2 is CC 0!
  TCC1_CH3 = (1<<8)|(1), // Channel 3 is CC 1!
  
  TCC2_CH0 = (2<<8)|(0),
  TCC2_CH1 = (2<<8)|(1),
  TCC2_CH2 = (2<<8)|(0), // Channel 2 is 0!
  TCC2_CH3 = (2<<8)|(1), // Channel 3 is 1!
  
  TC1_CH0  = (3<<8)|(0),
  TC1_CH1  = (3<<8)|(1),
  
  TC2_CH0  = (4<<8)|(0),
  TC2_CH1  = (4<<8)|(1),
  
  TC3_CH0  = (5<<8)|(0),
  TC3_CH1  = (5<<8)|(1),

} ETCChannel ;

extern const void* g_apTCInstances[TCC_INST_NUM+TC_INST_NUM] ;

#define GetTCNumber( x ) ( (x) >> 8 )
#define GetTCChannelNumber( x ) ( (x) & 0xff )
#define GetTC( x ) ( g_apTCInstances[(x) >> 8] )

// Definitions for PWM channels
// Cannot use TC4 - used by Tone
/*  
GCM_TCC0_TCC1,// TCC0
GCM_TCC0_TCC1,// TCC1
GCM_TCC2,		// TCC2
GCM_TC0_TC1,	// TC0 not used
GCM_TC0_TC1,	// TC1
GCM_TC2_TC3,	// TC2
GCM_TC2_TC3,	// TC3
GCM_TC4,		// TC4  - used by Tone lib */
typedef enum _EPWMChannel
{
  NOT_ON_PWM=-1,
  PWM0_CH0=TCC0_CH0,
  PWM0_CH1=TCC0_CH1,
  PWM0_CH2=TCC0_CH2,
  PWM0_CH3=TCC0_CH3,
  PWM0_CH4=TCC0_CH4,
  PWM0_CH5=TCC0_CH5,
  PWM0_CH6=TCC0_CH6,
  PWM0_CH7=TCC0_CH7,
  
  PWM1_CH0=TCC1_CH0,
  PWM1_CH1=TCC1_CH1,
  PWM1_CH2=TCC1_CH2,
  PWM1_CH3=TCC1_CH3,
  
  PWM2_CH0=TCC2_CH0,
  PWM2_CH1=TCC2_CH1,
  PWM2_CH2=TCC2_CH2,
  PWM2_CH3=TCC2_CH3,
  
  PWM3_CH0=TC1_CH0,
  PWM3_CH1=TC1_CH1,
  
  PWM4_CH0=TC2_CH0,
  PWM4_CH1=TC2_CH1,
  
  PWM5_CH0=TC3_CH0,
  PWM5_CH1=TC3_CH1,
} EPWMChannel ;

typedef enum _EPortType
{
  NOT_A_PORT=-1,
  PORTA=0,
  PORTB=1,
  PORTC=2,
} EPortType ;

typedef enum
{
  EXTERNAL_INT_0 = 0,
  EXTERNAL_INT_1,
  EXTERNAL_INT_2,
  EXTERNAL_INT_3,
  EXTERNAL_INT_4,
  EXTERNAL_INT_5,
  EXTERNAL_INT_6,
  EXTERNAL_INT_7,
  EXTERNAL_INT_8,
  EXTERNAL_INT_9,
  EXTERNAL_INT_10,
  EXTERNAL_INT_11,
  EXTERNAL_INT_12,
  EXTERNAL_INT_13,
  EXTERNAL_INT_14,
  EXTERNAL_INT_15,
  EXTERNAL_INT_NMI,
  EXTERNAL_NUM_INTERRUPTS,
  NOT_AN_INTERRUPT = -1,
  EXTERNAL_INT_NONE = NOT_AN_INTERRUPT,
} EExt_Interrupts ;

typedef enum
{
	EXTERNAL_WAKE_0 = 0,
	EXTERNAL_WAKE_1,
	EXTERNAL_WAKE_2,
	EXTERNAL_WAKE_3,
	EXTERNAL_WAKE_4,
	EXTERNAL_WAKE_5,
	EXTERNAL_WAKE_6,
	EXTERNAL_WAKE_7,
	EXTERNAL_WAKE_NUM,
} EExt_Wake ;

//A    B                 C       D          E      F   G   H        I
//EIC REF ADC AC PTC DAC SERCOM SERCOM_ALT TC/TCC TCC COM AC/GCLK  CCL

typedef enum _EPioType
{
  PIO_NOT_A_PIN=-1,     /* Not under control of a peripheral. */
  PIO_EXTINT=0,         /* The pin is controlled by the associated signal of peripheral A. */
  PIO_ANALOG,           /* The pin is controlled by the associated signal of peripheral B. */
  PIO_SERCOM,           /* The pin is controlled by the associated signal of peripheral C. */
  PIO_SERCOM_ALT,       /* The pin is controlled by the associated signal of peripheral D. */
  PIO_TIMER,            /* The pin is controlled by the associated signal of peripheral E. */
  PIO_TIMER_ALT,        /* The pin is controlled by the associated signal of peripheral F. */
  PIO_COM,              /* The pin is controlled by the associated signal of peripheral G. */
  PIO_AC_CLK,           /* The pin is controlled by the associated signal of peripheral H. */
  PIO_DIGITAL,          /* The pin is controlled by PORT. */
  PIO_INPUT,            /* The pin is controlled by PORT and is an input. */
  PIO_INPUT_PULLUP,     /* The pin is controlled by PORT and is an input with internal pull-up resistor enabled. */
  PIO_OUTPUT,           /* The pin is controlled by PORT and is an output. */
  PIO_CCL,				/* The pin is controlled by the associated signal of peripheral I*/
  
  PIO_PWM=PIO_TIMER,
  PIO_PWM_ALT=PIO_TIMER_ALT,
  PIO_PERIPH_A = PIO_EXTINT,
  PIO_PERIPH_B = PIO_ANALOG,
  PIO_PERIPH_C = PIO_SERCOM,
  PIO_PERIPH_D = PIO_SERCOM_ALT,
  PIO_PERIPH_E = PIO_TIMER,
  PIO_PERIPH_F = PIO_TIMER_ALT,
  PIO_PERIPH_G = PIO_COM,
  PIO_PERIPH_H = PIO_AC_CLK,
  PIO_PERIPH_I = PIO_CCL,
} EPioType ;

/**
 * Pin Attributes to be OR-ed
 */
#define PIN_ATTR_NONE          (0UL<<0)
#define PIN_ATTR_COMBO         (1UL<<0)
#define PIN_ATTR_ANALOG        (1UL<<1)
#define PIN_ATTR_DIGITAL       (1UL<<2)
#define PIN_ATTR_PWM           (1UL<<3)
#define PIN_ATTR_TIMER         (1UL<<4)
#define PIN_ATTR_TIMER_ALT     (1UL<<5)
#define PIN_ATTR_EXTINT        (1UL<<6)

/* Types used for the table below */
typedef struct _PinDescription
{
  EPortType       ulPort ;
  uint32_t        ulPin ;
  EPioType        ulPinType ;
  uint32_t        ulPinAttribute ;
  EAnalogChannel  ulADCChannelNumber ; /* ADC Channel number in the SAM device */
  EPWMChannel     ulPWMChannel ;
  ETCChannel      ulTCChannel ;
  EExt_Interrupts ulExtInt ;
} PinDescription ;

/* Pins table to be instantiated into variant.cpp */
extern const PinDescription g_APinDescription[] ;

/* Generic Clock Multiplexer IDs */
#define GCM_DFLL48M_REF           (0x00U)
#define GCM_FDPLL96M_INPUT        (0x01U)
#define GCM_FDPLL96M_32K          (0x02U)
#define GCM_EIC                   (0x03U)
#define GCM_USB                   (0x04U)
#define GCM_EVSYS_CHANNEL_0       (0x05U)
#define GCM_EVSYS_CHANNEL_1       (0x06U)
#define GCM_EVSYS_CHANNEL_2       (0x07U)
#define GCM_EVSYS_CHANNEL_3       (0x08U)
#define GCM_EVSYS_CHANNEL_4       (0x09U)
#define GCM_EVSYS_CHANNEL_5       (0x0AU)
#define GCM_EVSYS_CHANNEL_6       (0x0BU)
#define GCM_EVSYS_CHANNEL_7       (0x0CU)
#define GCM_EVSYS_CHANNEL_8       (0x0DU)
#define GCM_EVSYS_CHANNEL_9       (0x0EU)
#define GCM_EVSYS_CHANNEL_10      (0x0FU)
#define GCM_EVSYS_CHANNEL_11      (0x10U)
#define GCM_SERCOMx_SLOW          (0x11U)
#define GCM_SERCOM0_CORE          (0x12U)
#define GCM_SERCOM1_CORE          (0x13U)
#define GCM_SERCOM2_CORE          (0x14U)
#define GCM_SERCOM3_CORE          (0x15U)
#define GCM_SERCOM4_CORE          (0x16U)
#define GCM_SERCOM5_SLOW          (0x17U)
#define GCM_SERCOM5_CORE          (0x18U)
#define GCM_TCC0_TCC1             (0x19U)
#define GCM_TCC2                  (0x1AU)
#define GCM_TC0_TC1               (0x1BU)
#define GCM_TC2_TC3               (0x1CU)
#define GCM_TC4                   (0x1DU)
#define GCM_ADC                   (0x1EU)
#define GCM_AC		              (0x1FU)
#define GCM_DAC                   (0x20U)
#define GCM_PTC                   (0x21U)
#define GCM_CCL					  (0x22U)

#ifdef __cplusplus
} // extern "C"
#endif
