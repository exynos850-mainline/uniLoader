/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2026, schoosh212 <superaviation001@gmail.com>
 */
#include <board.h>
#include <util.h>
#include <string.h>
#include <drivers/framework.h>
#include <lib/simplefb.h>
#include <drivers/samsung/exynos-acpm.h>
#include "s2mpu12-pmic.h"

#define DECON_F_BASE            0x13060000
#define HW_SW_TRIG_CONTROL      0x70

/* PMIC */
#define EXYNOS_MAILBOX_AP2APM	0x11900000
#define S2MPU12_REG_ENABLE		(0x03 << 0x06)
#define S2MPU12_PM_ADDR                 0x1

/* BUCK 1M_2M_3M */
#define S2MPU12_BUCK_MIN1		300000
#define S2MPU12_BUCK_STEP1		6250
/* BUCK 4M */
#define S2MPU12_BUCK_MIN2		600000
#define S2MPU12_BUCK_STEP2		12500
/* BUCK 5M */
#define S2MPU12_BUCK_MIN3		600000
#define S2MPU12_BUCK_STEP3		12500
/* LDO 6M */
#define S2MPU12_LDO_MIN1		400000
#define S2MPU12_LDO_STEP1		12500
/* LDO 1M_3M_5M_7M_12M_13M_17M_18M_19M_29M_DLDO_BUF(36M) */
#define S2MPU12_LDO_MIN2		700000
#define S2MPU12_LDO_STEP2		12500
/* LDO 8M_9M */
#define S2MPU12_LDO_MIN3		300000
#define S2MPU12_LDO_STEP3		25000
/* LDO 2M_4M_14M_20M_28M_30M_ALDO1(33M)_DLDO_CORE(35M) */
#define S2MPU12_LDO_MIN4		700000
#define S2MPU12_LDO_STEP4		25000
/* LDO 10M_11M_15M_16M_21M_22M_23M_24M_25M_26M_27M_31M_32M_ALDO2(34M) */
#define S2MPU12_LDO_MIN5		1800000
#define S2MPU12_LDO_STEP5		25000

void pmic_init(void)
{
	unsigned char reg;
	void *mailbox_base = (void *)EXYNOS_MAILBOX_AP2APM;

	spmi_read(mailbox_base, MAIN_PMIC_ID, S2MPU12_PM_ADDR, S2MPU12_PMIC_B2CTRL, &reg); /* buck 2 */
	reg = S2MPU12_REG_ENABLE + (1150000 - S2MPU12_BUCK_MIN1) / S2MPU12_BUCK_STEP1;	/* 1.15V/1150000 uV */
	spmi_write(mailbox_base, MAIN_PMIC_ID, S2MPU12_PM_ADDR, S2MPU12_PMIC_B2CTRL, reg);

        spmi_read(mailbox_base, MAIN_PMIC_ID, S2MPU12_PM_ADDR, S2MPU12_PMIC_L28CTRL, &reg); /* ldo 28 */
        reg = S2MPU12_REG_ENABLE + (1800000 - S2MPU12_LDO_MIN4) / S2MPU12_LDO_STEP4;   /* 1.8V/1800000 uV */
        spmi_write(mailbox_base, MAIN_PMIC_ID, S2MPU12_PM_ADDR, S2MPU12_PMIC_L28CTRL, reg);
}

int a13_init(void)
{
	*(int*) (DECON_F_BASE + HW_SW_TRIG_CONTROL) = 0x1281;

	pmic_init();

	return 0;
}

static struct video_info a13_fb = {
	.format = FB_FORMAT_ARGB8888,
	.width = 1080,
	.height = 2408,
	.stride = 4,
	.address = (void *)0xfa000000
};

static const struct device a13_devices[] = {
	{ "simplefb", &a13_fb, "fb" },
};

struct board_data board_ops = {
	.name = "samsung-a13",
	.ops = {
		.early_init = a13_init,
	},
	.devices = a13_devices,
	.num_devices = ARRAY_SIZE(a13_devices),
	.quirks = 0
};
