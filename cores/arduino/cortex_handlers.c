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

#include <sam.h>
#include <variant.h>
#include <stdio.h>

/* RTOS Hooks */
extern void svcHook(void);
extern void pendSVHook(void);
extern int sysTickHook(void);

/* Default empty handler */
void Dummy_Handler(void)
{
#if defined DEBUG
  __BKPT(3);
#endif
  for (;;) { }
}

/* Cortex-M0+ core handlers */
void HardFault_Handler		(void) __attribute__ ((weak, alias("Dummy_Handler")));
void Reset_Handler			(void);
void NonMaskableInt_Handler (void) __attribute__ ((weak, alias("Dummy_Handler"))); // NMI_Handler
void SVCall_Handler			(void) __attribute__ ((weak, alias("Dummy_Handler"))); // SVC_Handler
void PendSV_Handler			(void) __attribute__ ((weak, alias("Dummy_Handler")));
void SysTick_Handler		(void);



/* Peripherals handlers */
void SYSTEM_Handler   (void) __attribute__ ((weak, alias("Dummy_Handler")));
void WDT_Handler      (void) __attribute__ ((weak, alias("Dummy_Handler")));
void RTC_Handler      (void) __attribute__ ((weak, alias("Dummy_Handler")));
void EIC_Handler      (void) __attribute__ ((weak, alias("Dummy_Handler")));
void NVMCTRL_Handler  (void) __attribute__ ((weak, alias("Dummy_Handler")));
void DMAC_Handler     (void) __attribute__ ((weak, alias("Dummy_Handler")));
void USB_Handler      (void) __attribute__ ((weak));
void EVSYS_Handler    (void) __attribute__ ((weak, alias("Dummy_Handler")));
void SERCOM0_Handler  (void) __attribute__ ((weak, alias("Dummy_Handler")));
void SERCOM1_Handler  (void) __attribute__ ((weak, alias("Dummy_Handler")));
void SERCOM2_Handler  (void) __attribute__ ((weak, alias("Dummy_Handler")));
void SERCOM3_Handler  (void) __attribute__ ((weak, alias("Dummy_Handler")));
void SERCOM4_Handler  (void) __attribute__ ((weak, alias("Dummy_Handler")));
void SERCOM5_Handler  (void) __attribute__ ((weak, alias("Dummy_Handler")));
void TCC0_Handler     (void) __attribute__ ((weak, alias("Dummy_Handler")));
void TCC1_Handler     (void) __attribute__ ((weak, alias("Dummy_Handler")));
void TCC2_Handler     (void) __attribute__ ((weak, alias("Dummy_Handler")));
void TC0_Handler      (void) __attribute__ ((weak, alias("Dummy_Handler")));
void TC1_Handler      (void) __attribute__ ((weak, alias("Dummy_Handler")));
void TC2_Handler      (void) __attribute__ ((weak, alias("Dummy_Handler")));
void TC3_Handler      (void) __attribute__ ((weak, alias("Dummy_Handler")));
void TC4_Handler      (void) __attribute__ ((weak, alias("Dummy_Handler")));
void ADC_Handler      (void) __attribute__ ((weak, alias("Dummy_Handler")));
void AC_Handler       (void) __attribute__ ((weak, alias("Dummy_Handler")));
void DAC_Handler      (void) __attribute__ ((weak, alias("Dummy_Handler")));
void PTC_Handler      (void) __attribute__ ((weak, alias("Dummy_Handler")));
void AES_Handler      (void) __attribute__ ((weak, alias("Dummy_Handler")));
void TRNG_Handler     (void) __attribute__ ((weak, alias("Dummy_Handler")));

/* Initialize segments */
extern uint32_t __etext;
extern uint32_t __data_start__;
extern uint32_t __data_end__;
extern uint32_t __bss_start__;
extern uint32_t __bss_end__;
extern uint32_t __StackTop;

/* Exception Table */
__attribute__ ((section(".isr_vector"))) const DeviceVectors exception_table =
{
  /* Configure Initial Stack Pointer, using linker-generated symbols */
  (void*) (&__StackTop),

  (void*) Reset_Handler,
  (void*) NonMaskableInt_Handler,
  (void*) HardFault_Handler,
  (void*) (0UL), /* Reserved */
  (void*) (0UL), /* Reserved */
  (void*) (0UL), /* Reserved */
  (void*) (0UL), /* Reserved */
  (void*) (0UL), /* Reserved */
  (void*) (0UL), /* Reserved */
  (void*) (0UL), /* Reserved */
  (void*) SVCall_Handler,
  (void*) (0UL), /* Reserved */
  (void*) (0UL), /* Reserved */
  (void*) PendSV_Handler,
  (void*) SysTick_Handler,

  /* Configurable interrupts */
  (void*) SYSTEM_Handler,         /*  0 SYSTEM handler (includes SYSTEM, MCLK, OSCCTRL, OSC32KCTRL, PAC, PM, SUPC, and TAL) */
  (void*) WDT_Handler,            /*  1 Watchdog Timer */
  (void*) RTC_Handler,            /*  2 Real-Time Counter */
  (void*) EIC_Handler,            /*  3 External Interrupt Controller */
  (void*) NVMCTRL_Handler,        /*  4 / 6 Non-Volatile Memory Controller */
  (void*) DMAC_Handler,           /*  5 / 7 Direct Memory Access Controller */
  (void*) USB_Handler,            /*  6 Universal Serial Bus */
  (void*) EVSYS_Handler,          /*  7 Event System Interface */
  (void*) SERCOM0_Handler,        /*  8 Serial Communication Interface 0 */
  (void*) SERCOM1_Handler,        /*  9 Serial Communication Interface 1 */
  (void*) SERCOM2_Handler,        /* 10 Serial Communication Interface 2 */
  (void*) SERCOM3_Handler,        /* 11 Serial Communication Interface 3 */
  (void*) SERCOM4_Handler,        /* 12 Serial Communication Interface 4 */
  (void*) SERCOM5_Handler,        /* 13 Serial Communication Interface 5 */
  (void*) TCC0_Handler,           /* 14 Timer Counter Control 0 */
  (void*) TCC1_Handler,           /* 15 Timer Counter Control 1 */
  (void*) TCC2_Handler,           /* 16 Timer Counter Control 2 */
  (void*) TC0_Handler,            /* 17 Basic Timer Counter 0 */
  (void*) TC1_Handler,            /* 18 Basic Timer Counter 1 */
  (void*) TC2_Handler,            /* 19 Basic Timer Counter 2 */
  (void*) TC3_Handler,            /* 20 Basic Timer Counter 3 */
  (void*) TC4_Handler,            /* 21 Basic Timer Counter 4 */
  (void*) ADC_Handler,            /* 22 Analog Digital Converter */
  (void*) AC_Handler,             /* 23 Analog Comparators */
  (void*) DAC_Handler,            /* 24 Digital Analog Converter */
  (void*) PTC_Handler,            /* 25 Peripheral Touch Controller */
  (void*) AES_Handler,            /* 26 AES */
  (void*) TRNG_Handler,           /* 27 TRNG */
  (void*) (0UL),                  /* Reserved */
};

extern int main(void);

/* This is called on processor reset to initialize the device and call main() */
void Reset_Handler(void)
{
  uint32_t *pSrc, *pDest;

  /* Initialize the initialized data section */
  pSrc = &__etext;
  pDest = &__data_start__;

  if ((&__data_start__ != &__data_end__) && (pSrc != pDest)) {
    for (; pDest < &__data_end__; pDest++, pSrc++)
      *pDest = *pSrc;
  }

  /* Clear the zero section */
  if ((&__data_start__ != &__data_end__) && (pSrc != pDest)) {
    for (pDest = &__bss_start__; pDest < &__bss_end__; pDest++)
      *pDest = 0;
  }

  SystemInit();

  main();

  while (1)
    ;
}

/* Default Arduino systick handler */
extern void SysTick_DefaultHandler(void);

void SysTick_Handler(void)
{
  if (sysTickHook())
    return;
  SysTick_DefaultHandler();
}

static void (*usb_isr)(void) = NULL;

void USB_Handler(void)
{
  if (usb_isr)
    usb_isr();
}

void USB_SetHandler(void (*new_usb_isr)(void))
{
  usb_isr = new_usb_isr;
}
