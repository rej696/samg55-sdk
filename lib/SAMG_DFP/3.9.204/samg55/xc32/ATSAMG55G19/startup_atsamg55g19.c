/**
 * \file
 *
 * \brief MPLAB(R) XC32 - Startup code for SAMG55G19
 *
 * Copyright (c) 2024 Microchip Technology Inc.
 *
 * \license_start
 *
 * \page License
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \license_stop
 *
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if __XC32_VERSION__ >= 2300
#pragma nocodecov
#endif


#ifndef   __INLINE
#if __GNUC_GNU_INLINE__ == 1
  #define __INLINE              __inline__
#else
  #define __INLINE              inline
#endif
#endif

#ifndef   __STATIC_INLINE
  #define __STATIC_INLINE       static __INLINE
#endif

#ifndef   __STATIC_FORCEINLINE
  #define __STATIC_FORCEINLINE  __attribute__((always_inline)) static __INLINE
#endif

#include <xc.h>
#include <libpic32c.h>

#ifndef __always_inline
#define __always_inline	__attribute__((__always_inline__))
#endif

/*
 *  Define the __XC32_RESET_HANDLER_NAME macro on the command line when you
 *  want to use a different name for the Reset Handler function.
 */
#ifndef __XC32_RESET_HANDLER_NAME
#define __XC32_RESET_HANDLER_NAME Reset_Handler
#endif /* __XC32_RESET_HANDLER_NAME */


/* Macros for compatibility between CMCC devices */
#if !defined(CMCC_CFG_CSIZESW) && defined(CMCC_CFG_PRGCSIZE)
#define CMCC_CFG_CSIZESW CMCC_CFG_PRGCSIZE
#endif
#if !defined(CMCC_CFG_CSIZESW_CONF_CSIZE_1KB_Val)
#define   CMCC_CFG_CSIZESW_CONF_CSIZE_1KB_Val _UINT32_(0x0)
#endif
#if !defined(CMCC_CFG_CSIZESW_CONF_CSIZE_2KB_Val)
#define   CMCC_CFG_CSIZESW_CONF_CSIZE_2KB_Val _UINT32_(0x1)
#endif
#if !defined(CMCC_CFG_CSIZESW_CONF_CSIZE_4KB_Val)
#define   CMCC_CFG_CSIZESW_CONF_CSIZE_4KB_Val _UINT32_(0x2)
#endif
#if !defined(CMCC_CFG_CSIZESW_CONF_CSIZE_8KB_Val)
#define   CMCC_CFG_CSIZESW_CONF_CSIZE_8KB_Val _UINT32_(0x3)
#endif

__STATIC_INLINE void __attribute__((optimize("-O1"))) CMCC_Configure(void)
{

#if defined(CMCC_REGS) && defined(CMCC_SR_CSTS_Msk)
    /* disable the cache in order to make changes ... */
    CMCC_REGS->CMCC_CTRL &=  ~(CMCC_CTRL_CEN_Msk);

    /* Set the appropriate bits in CSIZESW */

    if (!(CMCC_REGS->CMCC_SR & CMCC_SR_CSTS_Msk)){

#if (__XC32_TCM_LENGTH == 0x800)
        CMCC_REGS->CMCC_CFG = CMCC_CFG_CSIZESW(CMCC_CFG_CSIZESW_CONF_CSIZE_2KB_Val);     /* set cache size to 2 kb */
        CMCC_REGS->CMCC_CTRL |=  CMCC_CTRL_CEN_Msk;  /* re-enable CMCC */

#elif (__XC32_TCM_LENGTH == 0xC00)
        CMCC_REGS->CMCC_CFG = CMCC_CFG_CSIZESW(CMCC_CFG_CSIZESW_CONF_CSIZE_1KB_Val);     /* set cache size to 2 kb */
        CMCC_REGS->CMCC_CTRL |=  CMCC_CTRL_CEN_Msk;  /* re-enable CMCC */

#elif (__XC32_TCM_LENGTH == 0x1000)
                                            /* here we notably fail to re-enable the cache */

#else      /* __XC32_TCM_LENGTH is undef or 0 or an illegal value */
        CMCC_REGS->CMCC_CFG = CMCC_CFG_CSIZESW(CMCC_CFG_CSIZESW_CONF_CSIZE_4KB_Val);  /* set cache size to 4 kb  - i.e. no TCM */
        CMCC_REGS->CMCC_CTRL |=  CMCC_CTRL_CEN_Msk;                                   /* re-enable CMCC */

#endif
   }


#elif defined(CMCC) && defined(CMCC_SR_CSTS)
    /* disable the cache in order to make changes ... */
    CMCC->CTRL.bit.CEN = 0;

    /* Set the appropriate bits in CSIZESW */
    if (!(CMCC->SR.reg & CMCC_SR_CSTS)) {

#if (__XC32_TCM_LENGTH == 0x800)
        CMCC->CFG.bit.CSIZESW = 0x1;             /* set cache size to 2 kb */
        CMCC->CTRL.bit.CEN = 1;                  /* re-enable CMCC */
#elif (__XC32_TCM_LENGTH == 0xC00)
        CMCC->CFG.bit.CSIZESW = 0x0;             /* set cache size to 1 kb */
        CMCC->CTRL.bit.CEN = 1;                  /* re-enable CMCC */
#elif (__XC32_TCM_LENGTH == 0x1000)
                                            /* here we notably fail to re-enable the cache */

#else      /* __XC32_TCM_LENGTH is undef or 0 */
        CMCC->CFG.bit.CSIZESW = 0x2;     /* set cache size to 4 kb  - i.e. no TCM */
        CMCC->CTRL.bit.CEN = 1;
            ;      /* re-enable CMCC */

#endif    /* #if (__XC32_TCM_LENGTH == ...) */
    }


#else /* CMCC_REGS */
#warning "CMCC_Configure() not implemented for this device"
#endif

}

/* Initialize segments */
extern uint32_t __svectors;
extern uint32_t _stack;
extern uint32_t _dinit_size;
extern uint32_t _dinit_addr;

/** \cond DOXYGEN_SHOULD_SKIP_THIS */
/* We pass (0, NULL) for the arguments just in case, but applications
 * are expected to use main(void) as their prototype.  Thus, passing
 * NULL for argv is valid.
 */
extern int main(int, char **);
/** \endcond */

extern void __attribute__((long_call)) __libc_init_array(void);

/* Default empty handler */
void __attribute__((weak, used, optimize("-O1"), long_call, externally_visible)) Dummy_Handler(void);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wredundant-decls"

/* Reset handler and application-provided reset handler */
void __attribute__((weak, optimize("-O1"), long_call, naked, externally_visible)) Reset_Handler(void);
extern void __attribute__((weak, used, long_call)) __XC32_RESET_HANDLER_NAME(void);

#pragma GCC diagnostic pop

/* Cortex-M4 core handlers */
void NonMaskableInt_Handler ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void HardFault_Handler    ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void MemoryManagement_Handler ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void BusFault_Handler     ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void UsageFault_Handler   ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SVCall_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void DebugMonitor_Handler ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PendSV_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SysTick_Handler      ( void ) __attribute__ ((weak, alias("Dummy_Handler")));

/* Peripherals handlers */
void SUPC_Handler              ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void RSTC_Handler              ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void RTC_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void RTT_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void WDT_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PMC_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void EFC_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void Reserved7                 ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void FLEXCOM0_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void FLEXCOM1_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void Reserved10                ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PIOA_Handler              ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PIOB_Handler              ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PDMIC0_Handler            ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void FLEXCOM2_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void MEM2MEM_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void I2SC0_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void I2SC1_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PDMIC1_Handler            ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void FLEXCOM3_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void FLEXCOM4_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void FLEXCOM5_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void FLEXCOM6_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC0_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC0_CH0_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC1_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC0_CH1_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC2_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC0_CH2_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC3_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC1_CH0_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC4_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC1_CH1_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC5_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC1_CH2_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void ADC_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void Reserved30                ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void Reserved31                ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void Reserved32                ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void Reserved33                ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void Reserved34                ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void Reserved35                ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void Reserved36                ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void Reserved37                ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void Reserved38                ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void Reserved39                ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void Reserved40                ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void Reserved41                ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void Reserved42                ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void Reserved43                ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void Reserved44                ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void Reserved45                ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void Reserved46                ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void UHP_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void UDP_Handler               ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void CRCCU_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

/* Exception Table */
__attribute__ ((section(".vectors.default"), weak, externally_visible))
const DeviceVectors exception_table=
{
        /* Configure Initial Stack Pointer, using linker-generated symbols */
        .pvStack = (void*) (&_stack),

        .pfnReset_Handler              = (void*) __XC32_RESET_HANDLER_NAME,
        /* .pfnNonMaskableInt_Handler = */ (void*) NonMaskableInt_Handler,
        .pfnHardFault_Handler          = (void*) HardFault_Handler,
        /* .pfnMemoryManagement_Handler = */ (void*) MemoryManagement_Handler,
        .pfnBusFault_Handler           = (void*) BusFault_Handler,
        .pfnUsageFault_Handler         = (void*) UsageFault_Handler,
        /* .pvReservedC9            = */ (void*) (0UL), /* Reserved */
        /* .pvReservedC8            = */ (void*) (0UL), /* Reserved */
        /* .pvReservedC7            = */ (void*) (0UL), /* Reserved */
        /* .pvReservedC6            = */ (void*) (0UL), /* Reserved */
        /* .pfnSVCall_Handler       = */ (void*) SVCall_Handler,
        /* .pfnDebugMonitor_Handler = */ (void*) DebugMonitor_Handler,
        /* .pvReservedC3            = */ (void*) (0UL), /* Reserved */
        .pfnPendSV_Handler             = (void*) PendSV_Handler,
        .pfnSysTick_Handler            = (void*) SysTick_Handler,

#if (HEADER_FORMAT_VERSION_MAJOR > 1) /* include_mcc header file */
        /* Configurable interrupts with MCC names */
        /* .pfnSUPC_Handler         = */ (void*) SUPC_Handler,        /* 0  Supply Controller */
        /* .pfnRSTC_Handler         = */ (void*) RSTC_Handler,        /* 1  Reset Controller */
        /* .pfnRTC_Handler          = */ (void*) RTC_Handler,         /* 2  Real-time Clock */
        /* .pfnRTT_Handler          = */ (void*) RTT_Handler,         /* 3  Real-time Timer */
        /* .pfnWDT_Handler          = */ (void*) WDT_Handler,         /* 4  Watchdog Timer */
        /* .pfnPMC_Handler          = */ (void*) PMC_Handler,         /* 5  Power Management Controller */
        /* .pfnEFC_Handler          = */ (void*) EFC_Handler,         /* 6  Embedded Flash Controller */
        /* .pvReserved7             = */ (void*) (0UL),               /* 7  Reserved */
        /* .pfnFLEXCOM0_Handler     = */ (void*) FLEXCOM0_Handler,    /* 8  Flexible Serial Communication */
        /* .pfnFLEXCOM1_Handler     = */ (void*) FLEXCOM1_Handler,    /* 9  Flexible Serial Communication */
        /* .pvReserved10            = */ (void*) (0UL),               /* 10 Reserved */
        /* .pfnPIOA_Handler         = */ (void*) PIOA_Handler,        /* 11 Parallel Input/Output Controller */
        /* .pfnPIOB_Handler         = */ (void*) PIOB_Handler,        /* 12 Parallel Input/Output Controller */
        /* .pfnPDMIC0_Handler       = */ (void*) PDMIC0_Handler,      /* 13 Pulse Density Modulation Interface Controller */
        /* .pfnFLEXCOM2_Handler     = */ (void*) FLEXCOM2_Handler,    /* 14 Flexible Serial Communication */
        /* .pfnMEM2MEM_Handler      = */ (void*) MEM2MEM_Handler,     /* 15 Memory to Memory */
        /* .pfnI2SC0_Handler        = */ (void*) I2SC0_Handler,       /* 16 Inter-IC Sound Controller */
        /* .pfnI2SC1_Handler        = */ (void*) I2SC1_Handler,       /* 17 Inter-IC Sound Controller */
        /* .pfnPDMIC1_Handler       = */ (void*) PDMIC1_Handler,      /* 18 Pulse Density Modulation Interface Controller */
        /* .pfnFLEXCOM3_Handler     = */ (void*) FLEXCOM3_Handler,    /* 19 Flexible Serial Communication */
        /* .pfnFLEXCOM4_Handler     = */ (void*) FLEXCOM4_Handler,    /* 20 Flexible Serial Communication */
        /* .pfnFLEXCOM5_Handler     = */ (void*) FLEXCOM5_Handler,    /* 21 Flexible Serial Communication */
        /* .pfnFLEXCOM6_Handler     = */ (void*) FLEXCOM6_Handler,    /* 22 Flexible Serial Communication */
        /* .pfnTC0_CH0_Handler      = */ (void*) TC0_CH0_Handler,     /* 23 Timer/Counter 0 Channel 0 */
        /* .pfnTC0_CH1_Handler      = */ (void*) TC0_CH1_Handler,     /* 24 Timer/Counter 0 Channel 1 */
        /* .pfnTC0_CH2_Handler      = */ (void*) TC0_CH2_Handler,     /* 25 Timer/Counter 0 Channel 2 */
        /* .pfnTC1_CH0_Handler      = */ (void*) TC1_CH0_Handler,     /* 26 Timer/Counter 1 Channel 0 */
        /* .pfnTC1_CH1_Handler      = */ (void*) TC1_CH1_Handler,     /* 27 Timer/Counter 1 Channel 1 */
        /* .pfnTC1_CH2_Handler      = */ (void*) TC1_CH2_Handler,     /* 28 Timer/Counter 1 Channel 2 */
        /* .pfnADC_Handler          = */ (void*) ADC_Handler,         /* 29 Analog-to-Digital Converter */
        /* .pvReserved30            = */ (void*) (0UL),               /* 30 Reserved */
        /* .pvReserved31            = */ (void*) (0UL),               /* 31 Reserved */
        /* .pvReserved32            = */ (void*) (0UL),               /* 32 Reserved */
        /* .pvReserved33            = */ (void*) (0UL),               /* 33 Reserved */
        /* .pvReserved34            = */ (void*) (0UL),               /* 34 Reserved */
        /* .pvReserved35            = */ (void*) (0UL),               /* 35 Reserved */
        /* .pvReserved36            = */ (void*) (0UL),               /* 36 Reserved */
        /* .pvReserved37            = */ (void*) (0UL),               /* 37 Reserved */
        /* .pvReserved38            = */ (void*) (0UL),               /* 38 Reserved */
        /* .pvReserved39            = */ (void*) (0UL),               /* 39 Reserved */
        /* .pvReserved40            = */ (void*) (0UL),               /* 40 Reserved */
        /* .pvReserved41            = */ (void*) (0UL),               /* 41 Reserved */
        /* .pvReserved42            = */ (void*) (0UL),               /* 42 Reserved */
        /* .pvReserved43            = */ (void*) (0UL),               /* 43 Reserved */
        /* .pvReserved44            = */ (void*) (0UL),               /* 44 Reserved */
        /* .pvReserved45            = */ (void*) (0UL),               /* 45 Reserved */
        /* .pvReserved46            = */ (void*) (0UL),               /* 46 Reserved */
        /* .pfnUHP_Handler          = */ (void*) UHP_Handler,         /* 47 USB Host Port */
        /* .pfnUDP_Handler          = */ (void*) UDP_Handler,         /* 48 USB Device Port */
        /* .pfnCRCCU_Handler        = */ (void*) CRCCU_Handler        /* 49 Cyclic Redundancy Check Calculation Unit */

#else /* Legacy defined(_SAMG55G19_H) */
        /* Configurable interrupts with Legacy names */
        /* .pfnSUPC_Handler         = */ (void*) SUPC_Handler,        /* 0  Supply Controller */
        /* .pfnRSTC_Handler         = */ (void*) RSTC_Handler,        /* 1  Reset Controller */
        /* .pfnRTC_Handler          = */ (void*) RTC_Handler,         /* 2  Real-time Clock */
        /* .pfnRTT_Handler          = */ (void*) RTT_Handler,         /* 3  Real-time Timer */
        /* .pfnWDT_Handler          = */ (void*) WDT_Handler,         /* 4  Watchdog Timer */
        /* .pfnPMC_Handler          = */ (void*) PMC_Handler,         /* 5  Power Management Controller */
        /* .pfnEFC_Handler          = */ (void*) EFC_Handler,         /* 6  Embedded Flash Controller */
        /* .pvReserved7             = */ (void*) (0UL),               /* 7  Reserved */
        /* .pfnFLEXCOM0_Handler     = */ (void*) FLEXCOM0_Handler,    /* 8  Flexible Serial Communication */
        /* .pfnFLEXCOM1_Handler     = */ (void*) FLEXCOM1_Handler,    /* 9  Flexible Serial Communication */
        /* .pvReserved10            = */ (void*) (0UL),               /* 10 Reserved */
        /* .pfnPIOA_Handler         = */ (void*) PIOA_Handler,        /* 11 Parallel Input/Output Controller */
        /* .pfnPIOB_Handler         = */ (void*) PIOB_Handler,        /* 12 Parallel Input/Output Controller */
        /* .pfnPDMIC0_Handler       = */ (void*) PDMIC0_Handler,      /* 13 Pulse Density Modulation Interface Controller */
        /* .pfnFLEXCOM2_Handler     = */ (void*) FLEXCOM2_Handler,    /* 14 Flexible Serial Communication */
        /* .pfnMEM2MEM_Handler      = */ (void*) MEM2MEM_Handler,     /* 15 Memory to Memory */
        /* .pfnI2SC0_Handler        = */ (void*) I2SC0_Handler,       /* 16 Inter-IC Sound Controller */
        /* .pfnI2SC1_Handler        = */ (void*) I2SC1_Handler,       /* 17 Inter-IC Sound Controller */
        /* .pfnPDMIC1_Handler       = */ (void*) PDMIC1_Handler,      /* 18 Pulse Density Modulation Interface Controller */
        /* .pfnFLEXCOM3_Handler     = */ (void*) FLEXCOM3_Handler,    /* 19 Flexible Serial Communication */
        /* .pfnFLEXCOM4_Handler     = */ (void*) FLEXCOM4_Handler,    /* 20 Flexible Serial Communication */
        /* .pfnFLEXCOM5_Handler     = */ (void*) FLEXCOM5_Handler,    /* 21 Flexible Serial Communication */
        /* .pfnFLEXCOM6_Handler     = */ (void*) FLEXCOM6_Handler,    /* 22 Flexible Serial Communication */
        /* .pfnTC0_Handler          = */ (void*) TC0_Handler,         /* 23 Timer Counter */
        /* .pfnTC1_Handler          = */ (void*) TC1_Handler,         /* 24 Timer Counter */
        /* .pfnTC2_Handler          = */ (void*) TC2_Handler,         /* 25 Timer Counter */
        /* .pfnTC3_Handler          = */ (void*) TC3_Handler,         /* 26 Timer Counter */
        /* .pfnTC4_Handler          = */ (void*) TC4_Handler,         /* 27 Timer Counter */
        /* .pfnTC5_Handler          = */ (void*) TC5_Handler,         /* 28 Timer Counter */
        /* .pfnADC_Handler          = */ (void*) ADC_Handler,         /* 29 Analog-to-Digital Converter */
        /* .pvReserved30            = */ (void*) (0UL),               /* 30 Reserved */
        /* .pvReserved31            = */ (void*) (0UL),               /* 31 Reserved */
        /* .pvReserved32            = */ (void*) (0UL),               /* 32 Reserved */
        /* .pvReserved33            = */ (void*) (0UL),               /* 33 Reserved */
        /* .pvReserved34            = */ (void*) (0UL),               /* 34 Reserved */
        /* .pvReserved35            = */ (void*) (0UL),               /* 35 Reserved */
        /* .pvReserved36            = */ (void*) (0UL),               /* 36 Reserved */
        /* .pvReserved37            = */ (void*) (0UL),               /* 37 Reserved */
        /* .pvReserved38            = */ (void*) (0UL),               /* 38 Reserved */
        /* .pvReserved39            = */ (void*) (0UL),               /* 39 Reserved */
        /* .pvReserved40            = */ (void*) (0UL),               /* 40 Reserved */
        /* .pvReserved41            = */ (void*) (0UL),               /* 41 Reserved */
        /* .pvReserved42            = */ (void*) (0UL),               /* 42 Reserved */
        /* .pvReserved43            = */ (void*) (0UL),               /* 43 Reserved */
        /* .pvReserved44            = */ (void*) (0UL),               /* 44 Reserved */
        /* .pvReserved45            = */ (void*) (0UL),               /* 45 Reserved */
        /* .pvReserved46            = */ (void*) (0UL),               /* 46 Reserved */
        /* .pfnUHP_Handler          = */ (void*) UHP_Handler,         /* 47 USB Host Port */
        /* .pfnUDP_Handler          = */ (void*) UDP_Handler,         /* 48 USB Device Port */
        /* .pfnCRCCU_Handler        = */ (void*) CRCCU_Handler        /* 49 Cyclic Redundancy Check Calculation Unit */
#endif
};

#pragma GCC diagnostic pop

#if (__ARM_FP==14) || (__ARM_FP==4)
/* These functions are used only when compiling with -mfloat-abi=softfp or -mfloat-abi=hard */
/* Save and restore IRQs */
typedef uint32_t irqflags_t;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
static bool g_interrupt_enabled;
#pragma GCC diagnostic pop
#define cpu_irq_is_enabled()    (__get_PRIMASK() == 0)
#  define cpu_irq_enable()                             \
        do {                                           \
                g_interrupt_enabled = true;            \
                __DMB();                               \
                __enable_irq();                        \
        } while (0)
#  define cpu_irq_disable()                            \
        do {                                           \
                __disable_irq();                       \
                __DMB();                               \
                g_interrupt_enabled = false;           \
        } while (0)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
__always_inline __STATIC_INLINE bool __attribute__((optimize("-O1"))) cpu_irq_is_enabled_flags(irqflags_t flags)
{
        return (flags);
}
#pragma GCC diagnostic pop
__always_inline __STATIC_INLINE void __attribute__((optimize("-O1"))) cpu_irq_restore(irqflags_t flags)
{
        if (cpu_irq_is_enabled_flags(flags))
                cpu_irq_enable();
}
__always_inline __STATIC_INLINE __attribute__((optimize("-O1"))) irqflags_t cpu_irq_save(void)
{
        volatile irqflags_t flags = cpu_irq_is_enabled();
        cpu_irq_disable();
        return flags;
}

/** Address for ARM CPACR */
#define ADDR_CPACR 0xE000ED88
/** CPACR Register */
#define REG_CPACR  (*((volatile uint32_t *)ADDR_CPACR))

/**
* \brief Enable FPU
*/
__always_inline __STATIC_INLINE void __attribute__((optimize("-O1"))) fpu_enable(void)
{
    irqflags_t flags;
    flags = cpu_irq_save();
    REG_CPACR |=  (0xFu << 20);
    __DSB();
    __ISB();
    cpu_irq_restore(flags);
}
#endif /* (__ARM_FP==14) || (__ARM_FP==4) */
 
/* Optional application-provided functions */
extern void __attribute__((weak,long_call)) _on_reset(void);
extern void __attribute__((weak,long_call)) _on_bootstrap(void);
extern void __attribute__((weak,long_call)) _on_exit(void);

/* Reserved for use by the MPLAB XC32 Compiler */
extern void __attribute__((weak,long_call)) __xc32_on_reset(void);
extern void __attribute__((weak,long_call)) __xc32_on_bootstrap(void);


/**
 * \brief This is the code that gets called on processor reset.
 * To initialize the device, and call the main() routine.
 */
void __attribute__((weak, optimize("-O1"), section(".text.Reset_Handler"), long_call,
                    naked, externally_visible))
Reset_Handler(void)
{





#ifdef SCB_VTOR_TBLOFF_Msk
    uint32_t *pSrc;
#endif

#if defined (__REINIT_STACK_POINTER)
    /* Initialize SP from linker-defined _stack symbol. */
    __asm__ volatile ("ldr sp, =_stack" : : : "sp");

#ifdef SCB_VTOR_TBLOFF_Msk
    /* Buy stack for locals */
    __asm__ volatile ("sub sp, sp, #8" : : : "sp");
#endif
    __asm__ volatile ("add r7, sp, #0" : : : "r7");
#endif

    /* Call the optional application-provided _on_reset() function. */
    if (_on_reset)
      _on_reset();

    /* Reserved for use by MPLAB XC32. */
    if (__xc32_on_reset)
      __xc32_on_reset();


#if (__ARM_FP==14) || (__ARM_FP==4)
    /* Enable the FPU iff the application is built with -mfloat-abi=softfp or -mfloat-abi=hard */
    fpu_enable();
#endif



 




    /* get the right stuff into CMCC    this includes settings and enablement for TCM */
    CMCC_Configure();

 


    /* Data initialization from the XC32 .dinit template */
    __pic32c_data_initialization();
 
#ifdef SCB_VTOR_TBLOFF_Msk
    /*  Set the vector-table base address. This may be in flash or TCM.
     *  The __svectors symbol is created by the XC32 linker.
     */
    pSrc = (uint32_t *) & __svectors;
    SCB->VTOR = ((uint32_t) pSrc & SCB_VTOR_TBLOFF_Msk);
#endif
 
    /* Initialize the C library */
    __libc_init_array();

    /* Call the optional application-provided _on_bootstrap() function. */
    if (_on_bootstrap)
      _on_bootstrap();

    /* Reserved for use by MPLAB XC32. */
    if (__xc32_on_bootstrap)
      __xc32_on_bootstrap();

    /* Branch to application's main function.  See above about arguments. */
    main(0, NULL);

    /* Call the optional application-provided _on_exit() function */
    if (_on_exit)
        _on_exit();

#if (defined(__DEBUG) || defined(__DEBUG_D))
    __builtin_software_breakpoint();
#endif
    /* Infinite loop */
    while (1) {}
}

/**
* \brief Default interrupt handler for unused IRQs.
*/
void __attribute__((weak, optimize("-O1"), section(".text.Dummy_Handler"),
                    long_call, externally_visible))
Dummy_Handler(void)
{
#if (defined(__DEBUG) || defined(__DEBUG_D))
    __builtin_software_breakpoint();
#endif
    while (1) {}
}
