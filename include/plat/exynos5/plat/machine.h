/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(GD_GPL)
 */

#ifndef __PLAT_MACHINE_H
#define __PLAT_MACHINE_H

#include <config.h>

#define N_INTERRUPTS     160

enum IRQConstants {
//  INTERRUPT_RESERVED          =   0,
//  INTERRUPT_RESERVED          =   1,
//  INTERRUPT_RESERVED          =   2,
//  INTERRUPT_RESERVED          =   3,
//  INTERRUPT_RESERVED          =   4,
//  INTERRUPT_RESERVED          =   5,
//  INTERRUPT_RESERVED          =   6,
//  INTERRUPT_RESERVED          =   7,
//  INTERRUPT_RESERVED          =   8,
//  INTERRUPT_RESERVED          =   9,
//  INTERRUPT_RESERVED          =  10,
//  INTERRUPT_RESERVED          =  11,
//  INTERRUPT_RESERVED          =  12,
//  INTERRUPT_RESERVED          =  13,
//  INTERRUPT_RESERVED          =  14,
//  INTERRUPT_RESERVED          =  15,
//  INTERRUPT_RESERVED          =  16,
//  INTERRUPT_RESERVED          =  17,
//  INTERRUPT_RESERVED          =  18,
//  INTERRUPT_RESERVED          =  19,
//  INTERRUPT_RESERVED          =  20,
//  INTERRUPT_RESERVED          =  21,
//  INTERRUPT_RESERVED          =  22,
//  INTERRUPT_RESERVED          =  23,
//  INTERRUPT_RESERVED          =  24,
    INTERRUPT_VGIC_MAINTENANCE  =  25,
    INTERRUPT_CNTHPIRQ          =  26,
    INTERRUPT_CNTVIRQ           =  27,
//  INTERRUPT_RESERVED          =  28,
    INTERRUPT_CNTPSIRQ          =  29,
    INTERRUPT_CNTPNSIRQ         =  30,
//  INTERRUPT_RESERVED          =  31,
    INTERRUPT_INTG00            =  32,
    INTERRUPT_INTG01            =  33,
    INTERRUPT_INTG02            =  34,
    INTERRUPT_INTG03            =  35,
    INTERRUPT_INTG04            =  36,
    INTERRUPT_INTG05            =  37,
    INTERRUPT_INTG06            =  38,
    INTERRUPT_INTG07            =  39,
    INTERRUPT_INTG08            =  40,
    INTERRUPT_INTG09            =  41,
    INTERRUPT_INTG10            =  42,
    INTERRUPT_INTG11            =  43,
    INTERRUPT_INTG12            =  44,
    INTERRUPT_INTG13            =  45,
    INTERRUPT_INTG14            =  46,
    INTERRUPT_INTG15            =  47,
    INTERRUPT_INTG16            =  48,
    INTERRUPT_INTG17            =  49,
    INTERRUPT_INTG18            =  50,
    INTERRUPT_INTG19            =  51,
    INTERRUPT_INTG20            =  52,
    INTERRUPT_INTG21            =  53,
    INTERRUPT_INTG22            =  54,
    INTERRUPT_INTG23            =  55,
    INTERRUPT_INTG24            =  56,
    INTERRUPT_INTG25            =  57,
    INTERRUPT_INTG26            =  58,
    INTERRUPT_INTG27            =  59,
    INTERRUPT_INTG28            =  60,
    INTERRUPT_INTG29            =  61,
    INTERRUPT_INTG30            =  62,
    INTERRUPT_INTG31            =  63,
    INTERRUPT_EINT16_31         =  64,
#if defined(CONFIG_PLAT_EXYNOS5250)
    INTERRUPT_MDMA0_CORE        =  65,
#elif defined(CONFIG_PLAT_EXYNOS5410)
    INTERRUPT_TMU_CPU0          =  65,
#else
#endif
    INTERRUPT_PDMA0             =  66,
    INTERRUPT_PDMA1             =  67,
    INTERRUPT_TIMER0            =  68,
    INTERRUPT_TIMER1            =  69,
    INTERRUPT_TIMER2            =  70,
    INTERRUPT_TIMER3            =  71,
    INTERRUPT_TIMER4            =  72,
    INTERRUPT_RTIC              =  73,
    INTERRUPT_WDT               =  74,
    INTERRUPT_RTC_ALARM         =  75,
    INTERRUPT_RTC_TIC           =  76,
    INTERRUPT_GPIO_RT           =  77,
    INTERRUPT_GPIO_LB           =  78,
    INTERRUPT_GPIO              =  79,
//  INTERRUPT_RESERVED          =  80,
//  INTERRUPT_RESERVED          =  81,
    INTERRUPT_GPIO_C2C          =  82,
    INTERRUPT_UART0             =  83,
    INTERRUPT_UART1             =  84,
    INTERRUPT_UART2             =  85,
    INTERRUPT_UART3             =  86,
    INTERRUPT_MONOCNT           =  87,
    INTERRUPT_I2C0USI0          =  88,
    INTERRUPT_I2C1USI1          =  89,
    INTERRUPT_I2C2USI2          =  90,
    INTERRUPT_I2C3USI3          =  91,
    INTERRUPT_I2C4              =  92,
    INTERRUPT_I2C5              =  93,
    INTERRUPT_I2C6              =  94,
    INTERRUPT_I2C7              =  95,
    INTERRUPT_I2C               =  96,
    INTERRUPT_TMU               =  97,
    INTERRUPT_CPU_FIQ0          =  98,
    INTERRUPT_CPU_FIQ1          =  99,
    INTERRUPT_SPI0              = 100,
    INTERRUPT_SPI1              = 101,
    INTERRUPT_SPI2              = 102,
    INTERRUPT_USB_HOST20        = 103,
    INTERRUPT_USB_DRD30         = 104,
    INTERRUPT_MIPI_HSI          = 105,
    INTERRUPT_USBOTG            = 106,
    INTERRUPT_SDMMC0            = 107,
    INTERRUPT_SDMMC1            = 108,
    INTERRUPT_SDMMC2            = 109,
    INTERRUPT_SDMMC3            = 110,
    INTERRUPT_MIPI_CSI_A        = 111,
    INTERRUPT_MIPI_CSI_B        = 112,
    INTERRUPT_EFNFCON_DMA_ABORT = 113,
    INTERRUPT_MIPI_DSI_4LANE    = 114,
    INTERRUPT_WDT_IOP           = 115,
    INTERRUPT_ROTATOR           = 116,
    INTERRUPT_GSCL0             = 117,
    INTERRUPT_GSCL1             = 118,
    INTERRUPT_GSCL2             = 119,
    INTERRUPT_GSCL3             = 120,
    INTERRUPT_JPEG              = 121,
    INTERRUPT_EFNFCON_DMA       = 122,
    INTERRUPT_G2D               = 123,
    INTERRUPT_EFNFCON_0         = 124,
    INTERRUPT_EFNFCON_1         = 125,
    INTERRUPT_MIXER             = 126,
    INTERRUPT_HDMI              = 127,
    INTERRUPT_MFC               = 128,
    INTERRUPT_AUDIO_SS          = 129,
    INTERRUPT_I2S0              = 130,
    INTERRUPT_I2S1              = 131,
    INTERRUPT_I2S2              = 132,
    INTERRUPT_AC97              = 133,
    INTERRUPT_PCM0              = 134,
    INTERRUPT_PCM1              = 135,
    INTERRUPT_PCM2              = 136,
    INTERRUPT_SPDIF             = 137,
    INTERRUPT_ADC0              = 138,
//  INTERRUPT_RESERVED          = 139,
    INTERRUPT_SATAPHY           = 140,
    INTERRUPT_SATAPMEREQ        = 141,
    INTERRUPT_CAM_C             = 142,
    INTERRUPT_PMU               = 143,
    INTERRUPT_INTFEEDCTRL_SSS   = 144,
    INTERRUPT_DP1_1             = 145,
    INTERRUPT_CEC               = 146,
    INTERRUPT_SATA              = 147,
//  INTERRUPT_RESERVED          = 148,
    INTERRUPT_G3D_IRQGPU        = 149,
    INTERRUPT_G3D_IRQJOB        = 150,
    INTERRUPT_G3D_IRQMMU        = 151,
    INTERRUPT_MCT_L0            = 152,
    INTERRUPT_MCT_L1            = 153,
//  INTERRUPT_RESERVED          = 154,
//  INTERRUPT_RESERVED          = 155,
    INTERRUPT_MDMA1             = 156,
    INTERRUPT_CAM_A             = 157,
    INTERRUPT_CAM_B             = 158,
    INTERRUPT_RP_TIMER          = 159,

    INTERRUPT_TMU_CPU1          = 183,
    INTERRUPT_TMU_CPU2          = 184,
    INTERRUPT_TMU_CPU3          = 185,

    INTERRUPT_TMU_GPU           = 215,

    INTERRUPT_216               = 216,
    INTERRUPT_217               = 217,
    INTERRUPT_218               = 218,
    INTERRUPT_219               = 219,
    INTERRUPT_220               = 220,
    INTERRUPT_232               = 232,
    maxIRQ = 232
} platform_interrupt_t;

#define IRQ_CNODE_BITS 12

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
#define KERNEL_TIMER_IRQ INTERRUPT_CNTHPIRQ
#else
#define KERNEL_TIMER_IRQ INTERRUPT_CNTVIRQ
#endif

#include <arch/machine/gic_pl390.h>

#endif /* !__PLAT_MACHINE_H */
/*
 * Copyright 2016, 2017 Hesham Almatary, Data61/CSIRO <hesham.almatary@data61.csiro.au>
 * Copyright 2015-2016 Hesham Almatary <heshamelmatary@gmail.com>
 * Author: Hesham Almatary <heshamelmatary@gmail.com>
 */

