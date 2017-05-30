/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(GD_GPL)
 */

#ifndef __PLAT_MACHINE_DEVICES_H
#define __PLAT_MACHINE_DEVICES_H

/* These devices are used by the seL4 kernel. */
#define UART_PPTR                   0xfff01000
#define MCT_PPTR                    0xfff02000
#define L2CC_PPTR                   0xfff03000
#define GIC_DISTRIBUTOR_PPTR        0xfff04000
#define GIC_CONTROLLER_PPTR         0xfff05000
#define GIC_VCPUCTRL_PPTR           0xfff06000
#define ARM_DEBUG_MMAPPING_PPTR     0xfff07000

/* HYP mode kernel devices */
#define GIC_PL400_VCPUCTRL_PPTR     GIC_VCPUCTRL_PPTR

#define L2CC_L2C310_PPTR            L2CC_PPTR
#define GIC_PL390_CONTROLLER_PPTR   GIC_CONTROLLER_PPTR
#define GIC_PL390_DISTRIBUTOR_PPTR  GIC_DISTRIBUTOR_PPTR

#define GIC_DISTRIBUTOR_PADDR       (GIC_PADDR + 0x1000)
#define GIC_CONTROLLER0_PADDR       (GIC_PADDR + 0x2000)
#define GIC_VCPUCTRL_PADDR          (GIC_PADDR + 0x4000)
#define GIC_VCPU_PADDR              (GIC_PADDR + 0x6000)

#define EXYNOS_MCT_MHZ              24
#define EXYNOS_MCT_PPTR             MCT_PPTR

/* Generic devices */
#define SECURE_FIRMWARE       0x02070000
#define CHIP_ID_PADDR         0x10000000
#define CMU_CPU_PADDR         0x10010000
#define CMU_CORE_PADDR        0x10014000
#define CMU_ACP_PADDR         0x10018000
#define CMU_ISP_PADDR         0x1001C000
#define CMU_TOP_PADDR         0x10020000
#define CMU_LEX_PADDR         0x10024000
#define CMU_R0X_PADDR         0x10028000
#define CMU_R1X_PADDR         0x1002C000
#define CMU_CDREX_PADDR       0x10030000
#define CMU_MEM_PADDR         0x10038000
#define ALIVE_PADDR           0x10040000
#define SYSREG_PADDR          0x10050000
#define TMU0_PADDR            0x10060000
#define TMU1_PADDR            0x10064000
#define TMU2_PADDR            0x10068000
#define TMU3_PADDR            0x1006C000
#define TMU_GPU_PADDR         0x100A0000
#define MONOTONIC_CNT_PADDR   0x100C0000
#define HDMI_CEC_PADDR        0x101B0000
#define MCT_PADDR             0x101C0000
#define WDT_PADDR             0x101D0000
#define RTC_PADDR             0x101E0000
#define INT_COMB_CPU_PADDR    0x10440000
#define INT_COMB_IOP_PADDR    0x10450000
#define GIC_PADDR             0x10480000
#define GIC_IOPC_PADDR        0x104A0000
#define GIC_IOPD_PADDR        0x104B0000
#define MPCORE_PRIV_REG_PADDR 0x10500000
#define NS_MDMA0_PADDR        0x10800000
#define SSS_PADDR             0x10830000
#define SSS_KEY_PADDR         0x10840000
#define ENGINE_2D_PADDR       0x10850000
#define CSSYS_PADDR           0x10880000
#define A15_EAGLE_PADDR       0x10890000
#define A5_IOP_PADDR          0x108A0000
#define A5_ISP_PADDR          0x108B0000
#define SYSMMU_MDMA_PADDR     0x10A40000
#define SYSMMU_SSS_PADDR      0x10A50000
#define SYSMMU_2D_PADDR       0x10A60000
#define DREXII_PHY0_PADDR     0x10C00000
#define DREXII_PHY1_PADDR     0x10C10000
#define AS_A_3D_PADDR         0x10CC0000
#define AS_A_C2C_PADDR        0x10CD0000
#define AS_A_LEFT_BUS_PADDR   0x10CE0000
#define AS_A_RIGHT0_BUS_PADDR 0x10CF0000
#define AS_A_DISP1_BUS_PADDR  0x10D00000
#define DREXII_PADDR          0x10DD0000
#define AS_A_EFCON_PADDR      0x10DE0000
#define AP_C2C_PADDR          0x10E00000
#define CP_C2C_PADDR          0x10E40000
#define AS_A_ACP_BLK_PADDR    0x10E80000
#define AS_A_CPU_P_BLK_PADDR  0x10E90000
#define AS_A_LBX_BUS_PADDR    0x10F00000
#define AS_A_R1BX_BUS_PADDR   0x10F10000
#define AS_A_R0BX_BUS_PADDR   0x10F20000
#define AS_A_CPU_PADDR        0x10F30000
#define MFC_PADDR             0x11000000
#define SYSMMU_MFC0_PADDR     0x11200000
#define SYSMMU_MFC1_PADDR     0x11210000
#define AS_A_MFC_PADDR        0x11680000
#define AS_A_GENX_PADDR       0x116A0000
#define ENGINE_3D_PADDR       0x11800000
#define ROTATOR_PADDR         0x11C00000
#define NS_MDMA1_PADDR        0x11C10000
#define SYSMMU_ROTATOR_PADDR  0x11D40000
#define SYSMMU_MDMA1_PADDR    0x11D50000
#define AS_A_FILE_PADDR       0x11DA0000
#define AS_A_GPS_PADDR        0x11DB0000
#define AS_A_JPEG_PADDR       0x11DC0000
#define JPEG_PADDR            0x11E00000
#define SYSMMU_JPEG_PADDR     0x11F20000
#define USB3_DEV_LINK_PADDR   0x12000000
#define USB3_DEV_CTRL_PADDR   0x12100000
#define USB2_HOST_EHCI_PADDR  0x12110000
#define USB2_HOST_OHCI_PADDR  0x12120000
#define USB2_HOST_CTRL_PADDR  0x12130000
#define USB2_DEV_LINK_PADDR   0x12140000
#define MIPI_HSI_PADDR        0x12160000
#define SATA_PHY_CTRL_PADDR   0x12170000
#define MCUCTL_IOP_PADDR      0x12180000
#define WDT_IOP_PADDR         0x12190000
#define PDMA0_PADDR           0x121A0000
#define PDMA1_PADDR           0x121B0000
#define RTIC_PADDR            0x121C0000
#define SATA_I2CPHYCTRL_PADDR 0x121D0000
#define MSH0_PADDR            0x12200000
#define MSH1_PADDR            0x12210000
#define MSH2_PADDR            0x12220000
#define MSH3_PADDR            0x12230000
#define SROMC_PADDR           0x12250000
#define SATA_PADDR            0x122F0000
#define AXI_FILE_D64_PADDR    0x12300000
// #define AXI_FILE_D64_PADDR    0x12310000
#define AXI_USBSATA_D64_PADDR 0x12320000
// #define AXI_USBSATA_D64_PADDR 0x12330000
#define SYSMMU_IOPROC_PADDR   0x12360000
#define SYSMMU_RTIC_PADDR     0x12370000
#define AS_A_IOP_FD64X_PADDR  0x12380000
#define AS_A_AUDIO_PADDR      0x12390000
#define USB_PADDR             0x12400000
#define USB3_PHY1_PADDR       0x12500000
#define AXI_GPS_PADDR         0x12600000
// #define AXI_GPS_PADDR         0x12610000
#define AS_A_GPSCPU_PADDR     0x12620000
#define SYSMMU_GPS_PADDR      0x12630000
#define UART0_PADDR           0x12C00000
#define UART1_PADDR           0x12C10000
#define UART2_PADDR           0x12C20000
#define UART3_PADDR           0x12C30000
#define USI0_PADDR            0x12C50000
#define I2C0_PADDR            0x12C60000
#define I2C1_PADDR            0x12C70000
#define I2C2_PADDR            0x12C80000
#define I2C3_PADDR            0x12C90000
#define I2C4_PADDR            0x12CA0000
#define I2C5_PADDR            0x12CB0000
#define I2C6_PADDR            0x12CC0000
#define I2C7_PADDR            0x12CD0000
#define I2C_HDMI_PADDR        0x12CE0000
#define USI1_PADDR            0x12D00000
#define TSADC_PADDR           0x12D10000
#define SPI0_PADDR            0x12D20000
#define SPI1_PADDR            0x12D30000
#define SPI2_PADDR            0x12D40000
#define USI2_PADDR            0x12D50000
#define I2S1_PADDR            0x12D60000
#define I2S2_PADDR            0x12D70000
#define PCM1_PADDR            0x12D80000
#define PCM2_PADDR            0x12D90000
#define AC97_PADDR            0x12DA0000
#define SPDIF_PADDR           0x12DB0000
#define PWM_PADDR             0x12DD0000
#define USI3_PADDR            0x12DE0000
#define FIMC_ISP_PADDR        0x13000000
#define FIMC_DRC_TOP_PADDR    0x13010000
#define FIMC_SCALERC_PADDR    0x13020000
#define FIMC_SCALERP_PADDR    0x13030000
#define FIMC_FD_TOP_PADDR     0x13040000
#define FIMC_ODC_PADDR        0x13050000
#define FIMC_DIS_PADDR        0x13060000
#define FIMC_3DNR_PADDR       0x13070000
#define ASYNC_AXI_M_PADDR     0x130F0000
#define MPWM_ISP_PADDR        0x13110000
#define I2C2_ISP_PADDR        0x13120000
#define I2C0_ISP_PADDR        0x13130000
#define I2C1_ISP_PADDR        0x13140000
#define MTCADC_ISP_PADDR      0x13150000
#define PWM_ISP_PADDR         0x13160000
#define WDT_ISP_PADDR         0x13170000
#define MCUCTL_ISP_PADDR      0x13180000
#define UART_ISP_PADDR        0x13190000
#define SPI0_ISP_PADDR        0x131A0000
#define SPI1_ISP_PADDR        0x131B0000
#define GIC_C_ISP_PADDR       0x131E0000
#define GIC_D_ISP_PADDR       0x131F0000
#define SYSMMU_FIMCISP_PADDR  0x13260000
#define SYSMMU_FIMCDRC_PADDR  0x13270000
#define SYSMMU_FIMCSCLC_PADDR 0x13280000
#define SYSMMU_FIMCSCLP_PADDR 0x13290000
#define SYSMMU_FIMCFD_PADDR   0x132A0000
#define SYSMMU_ISPCPU_PADDR   0x132B0000
#define SYSMMU_FIMCODC_PADDR  0x132C0000
#define SYSMMU_FIMCDIS0_PADDR 0x132D0000
#define SYSMMU_FIMCDIS1_PADDR 0x132E0000
#define SYSMMU_FIMC3DNR_PADDR 0x132F0000
#define AS_A_MFC0_PADDR       0x13620000
#define AS_A_ISP0_PADDR       0x13640000
#define AS_A_ISP1_PADDR       0x13650000
#define AS_A_RIGHT1_PADDR     0x13670000
#define FIMCLT0_PADDR         0x13C00000
#define FIMCLT1_PADDR         0x13C10000
#define MIPI_CSI0_PADDR       0x13C20000
#define MIPI_CSI1_PADDR       0x13C30000
#define SYSMMU_FIMCLT0_PADDR  0x13C40000
#define SYSMMU_FIMCLT1_PADDR  0x13C50000
#define FIMCLT2_PADDR         0x13C90000
#define SYSMMU_FIMCLT2_PADDR  0x13CA0000
#define GSCALER0_PADDR        0x13E00000
#define GSCALER1_PADDR        0x13E10000
#define GSCALER2_PADDR        0x13E20000
#define GSCALER3_PADDR        0x13E30000
#define AS_A_GS0_PADDR        0x13E40000
#define AS_A_GS1_PADDR        0x13E50000
#define AS_A_GS2_PADDR        0x13E60000
#define AS_A_GS3_PADDR        0x13E70000
#define SYSMMU_GSCALER0_PADDR 0x13E80000
#define SYSMMU_GSCALER1_PADDR 0x13E90000
#define SYSMMU_GSCALER2_PADDR 0x13EA0000
#define SYSMMU_GSCALER3_PADDR 0x13EB0000
#define AS_A_GSCALER_PADDR    0x14220000
#define DISP1_MIX_PADDR       0x14400000
#define DISP1_ENH_PADDR       0x14410000
#define DISP1_CTRL_PADDR      0x14420000
#define MIE_PADDR             0x14430000
#define TV_MIXER_PADDR        0x14450000
#define MIPI_DSI1_PADDR       0x14500000
#define DP1_PADDR             0x14510000
#define HDMI_0_PADDR          0x14530000
#define HDMI_1_PADDR          0x14540000
#define HDMI_2_PADDR          0x14550000
#define HDMI_3_PADDR          0x14560000
#define HDMI_4_PADDR          0x14570000
#define HDMI_5_PADDR          0x14580000
#define HDMI_6_PADDR          0x14590000
#define DP1_1_PADDR           0x145B0000
#define HDMI_0_PHY_PADDR      0x145D0000
#define SYSMMU_DISP1_PADDR    0x14640000
#define SYSMMU_TV_PADDR       0x14650000
#define AS_A_TV_PADDR         0x146D0000
#define AES0EF0_PADDR         0x18000000
#define EFCON0_SFR_PADDR      0x18200000
#define AES0_SFR_PADDR        0x18300000
#define AES1EF1_PADDR         0x18400000
#define EFCON1_SFR_PADDR      0x18600000
#define NS_NDMA_PADDR         0x18680000
#define S_NDMA_PADDR          0x18690000
#define AES1_SFR_PADDR        0x18700000

#define AUDSS_PADDR           0x03810000

/* SoC specific devices */
#if defined(CONFIG_PLAT_EXYNOS5250)   /* Arndale */
#define GPIO_LEFT_PADDR       0x11400000
#define GPIO_RIGHT_PADDR      0x13400000
#define C2C_GPIO_PADDR        0x10D10000
#define AUDIO_GPIO_PADDR      0x03860000

#elif defined(CONFIG_PLAT_EXYNOS54XX) /* Odroid XU and XU3 */
#define GPIO_LEFT_PADDR       0x14000000
#define GPIO_RIGHT_PADDR      0x13400000
#define C2C_GPIO_PADDR        0x10D10000
#define AUDIO_GPIO_PADDR      0x03860000

#else  /* PLAT_EXYNOS5XXX */
#error Unidentified EXYNOS5 SoC
#endif /* PLAT_EXYNOS5XXX */

#endif /* !__PLAT_MACHINE_DEVICES_H */
/*
 * Copyright 2016, 2017 Hesham Almatary, Data61/CSIRO <hesham.almatary@data61.csiro.au>
 * Copyright 2015-2016 Hesham Almatary <heshamelmatary@gmail.com>
 * Author: Hesham Almatary <heshamelmatary@gmail.com>
 */

