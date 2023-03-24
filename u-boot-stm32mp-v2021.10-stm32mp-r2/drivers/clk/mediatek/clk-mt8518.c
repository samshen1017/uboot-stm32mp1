// SPDX-License-Identifier: GPL-2.0
/*
 * MediaTek clock driver for MT8518 SoC
 *
 * Copyright (C) 2019 BayLibre, SAS
 * Author: Chen Zhong <chen.zhong@mediatek.com>
 */

#include <common.h>
#include <dm.h>
#include <asm/io.h>
#include <dt-bindings/clock/mt8518-clk.h>
#include <linux/bitops.h>

#include "clk-mtk.h"

#define MT8518_PLL_FMAX		(3000UL * MHZ)
#define MT8518_CON0_RST_BAR	BIT(27)

/* apmixedsys */
#define PLL(_id, _reg, _pwr_reg, _en_mask, _flags, _pcwbits, _pd_reg,	\
	    _pd_shift, _pcw_reg, _pcw_shift) {				\
		.id = _id,						\
		.reg = _reg,						\
		.pwr_reg = _pwr_reg,					\
		.en_mask = _en_mask,					\
		.rst_bar_mask = MT8518_CON0_RST_BAR,			\
		.fmax = MT8518_PLL_FMAX,				\
		.flags = _flags,					\
		.pcwbits = _pcwbits,					\
		.pd_reg = _pd_reg,					\
		.pd_shift = _pd_shift,					\
		.pcw_reg = _pcw_reg,					\
		.pcw_shift = _pcw_shift,				\
	}

static const struct mtk_pll_data apmixed_plls[] = {
	PLL(CLK_APMIXED_ARMPLL, 0x0100, 0x0110, 0x00000001,
	    0, 21, 0x0104, 24, 0x0104, 0),
	PLL(CLK_APMIXED_MAINPLL, 0x0120, 0x0130, 0x00000001,
	    HAVE_RST_BAR, 21, 0x0124, 24, 0x0124, 0),
	PLL(CLK_APMIXED_UNIVPLL, 0x0140, 0x0150, 0x30000001,
	    HAVE_RST_BAR, 7, 0x0144, 24, 0x0144, 0),
	PLL(CLK_APMIXED_MMPLL, 0x0160, 0x0170, 0x00000001,
	    0, 21, 0x0164, 24, 0x0164, 0),
	PLL(CLK_APMIXED_APLL1, 0x0180, 0x0190, 0x00000001,
	    0, 31, 0x0180, 1, 0x0184, 0),
	PLL(CLK_APMIXED_APLL2, 0x01A0, 0x01B0, 0x00000001,
	    0, 31, 0x01A0, 1, 0x01A4, 0),
	PLL(CLK_APMIXED_TVDPLL, 0x01C0, 0x01D0, 0x00000001,
	    0, 21, 0x01C4, 24, 0x01C4, 0),
};

/* topckgen */
#define FACTOR0(_id, _parent, _mult, _div)	\
	FACTOR(_id, _parent, _mult, _div, CLK_PARENT_APMIXED)

#define FACTOR1(_id, _parent, _mult, _div)	\
	FACTOR(_id, _parent, _mult, _div, CLK_PARENT_TOPCKGEN)

#define FACTOR2(_id, _parent, _mult, _div)	\
	FACTOR(_id, _parent, _mult, _div, 0)

static const struct mtk_fixed_clk top_fixed_clks[] = {
	FIXED_CLK(CLK_TOP_CLK_NULL, CLK_XTAL, 26000000),
	FIXED_CLK(CLK_TOP_FQ_TRNG_OUT0, CLK_TOP_CLK_NULL, 500000000),
	FIXED_CLK(CLK_TOP_FQ_TRNG_OUT1, CLK_TOP_CLK_NULL, 500000000),
	FIXED_CLK(CLK_TOP_CLK32K, CLK_XTAL, 32000),
};

static const struct mtk_fixed_factor top_fixed_divs[] = {
	FACTOR2(CLK_TOP_DMPLL, CLK_XTAL, 1, 1),
	FACTOR0(CLK_TOP_MAINPLL_D4, CLK_APMIXED_MAINPLL, 1, 4),
	FACTOR0(CLK_TOP_MAINPLL_D8, CLK_APMIXED_MAINPLL, 1, 8),
	FACTOR0(CLK_TOP_MAINPLL_D16, CLK_APMIXED_MAINPLL, 1, 16),
	FACTOR0(CLK_TOP_MAINPLL_D11, CLK_APMIXED_MAINPLL, 1, 11),
	FACTOR0(CLK_TOP_MAINPLL_D22, CLK_APMIXED_MAINPLL, 1, 22),
	FACTOR0(CLK_TOP_MAINPLL_D3, CLK_APMIXED_MAINPLL, 1, 3),
	FACTOR0(CLK_TOP_MAINPLL_D6, CLK_APMIXED_MAINPLL, 1, 6),
	FACTOR0(CLK_TOP_MAINPLL_D12, CLK_APMIXED_MAINPLL, 1, 12),
	FACTOR0(CLK_TOP_MAINPLL_D5, CLK_APMIXED_MAINPLL, 1, 5),
	FACTOR0(CLK_TOP_MAINPLL_D10, CLK_APMIXED_MAINPLL, 1, 10),
	FACTOR0(CLK_TOP_MAINPLL_D20, CLK_APMIXED_MAINPLL, 1, 20),
	FACTOR0(CLK_TOP_MAINPLL_D40, CLK_APMIXED_MAINPLL, 1, 40),
	FACTOR0(CLK_TOP_MAINPLL_D7, CLK_APMIXED_MAINPLL, 1, 7),
	FACTOR0(CLK_TOP_MAINPLL_D14, CLK_APMIXED_MAINPLL, 1, 14),
	FACTOR0(CLK_TOP_UNIVPLL_D2, CLK_APMIXED_UNIVPLL, 1, 2),
	FACTOR0(CLK_TOP_UNIVPLL_D4, CLK_APMIXED_UNIVPLL, 1, 4),
	FACTOR0(CLK_TOP_UNIVPLL_D8, CLK_APMIXED_UNIVPLL, 1, 8),
	FACTOR0(CLK_TOP_UNIVPLL_D16, CLK_APMIXED_UNIVPLL, 1, 16),
	FACTOR0(CLK_TOP_UNIVPLL_D3, CLK_APMIXED_UNIVPLL, 1, 3),
	FACTOR0(CLK_TOP_UNIVPLL_D6, CLK_APMIXED_UNIVPLL, 1, 6),
	FACTOR0(CLK_TOP_UNIVPLL_D12, CLK_APMIXED_UNIVPLL, 1, 12),
	FACTOR0(CLK_TOP_UNIVPLL_D24, CLK_APMIXED_UNIVPLL, 1, 24),
	FACTOR0(CLK_TOP_UNIVPLL_D5, CLK_APMIXED_UNIVPLL, 1, 5),
	FACTOR0(CLK_TOP_UNIVPLL_D20, CLK_APMIXED_UNIVPLL, 1, 20),
	FACTOR0(CLK_TOP_UNIVPLL_D10, CLK_APMIXED_UNIVPLL, 1, 10),
	FACTOR0(CLK_TOP_MMPLL_D2, CLK_APMIXED_MMPLL, 1, 2),
	FACTOR0(CLK_TOP_USB20_48M, CLK_APMIXED_UNIVPLL, 1, 26),
	FACTOR0(CLK_TOP_APLL1, CLK_APMIXED_APLL1, 1, 1),
	FACTOR1(CLK_TOP_APLL1_D4, CLK_TOP_APLL1, 1, 4),
	FACTOR0(CLK_TOP_APLL2, CLK_APMIXED_APLL2, 1, 1),
	FACTOR1(CLK_TOP_APLL2_D2, CLK_TOP_APLL2, 1, 2),
	FACTOR1(CLK_TOP_APLL2_D3, CLK_TOP_APLL2, 1, 3),
	FACTOR1(CLK_TOP_APLL2_D4, CLK_TOP_APLL2, 1, 4),
	FACTOR1(CLK_TOP_APLL2_D8, CLK_TOP_APLL2, 1, 8),
	FACTOR2(CLK_TOP_CLK26M, CLK_XTAL, 1, 1),
	FACTOR2(CLK_TOP_CLK26M_D2, CLK_XTAL, 1, 2),
	FACTOR2(CLK_TOP_CLK26M_D4, CLK_XTAL, 1, 4),
	FACTOR2(CLK_TOP_CLK26M_D8, CLK_XTAL, 1, 8),
	FACTOR2(CLK_TOP_CLK26M_D793, CLK_XTAL, 1, 793),
	FACTOR0(CLK_TOP_TVDPLL, CLK_APMIXED_TVDPLL, 1, 1),
	FACTOR1(CLK_TOP_TVDPLL_D2, CLK_TOP_TVDPLL, 1, 2),
	FACTOR1(CLK_TOP_TVDPLL_D4, CLK_TOP_TVDPLL, 1, 4),
	FACTOR1(CLK_TOP_TVDPLL_D8, CLK_TOP_TVDPLL, 1, 8),
	FACTOR1(CLK_TOP_TVDPLL_D16, CLK_TOP_TVDPLL, 1, 16),
	FACTOR1(CLK_TOP_USB20_CLK480M, CLK_TOP_CLK_NULL, 1, 1),
	FACTOR1(CLK_TOP_RG_APLL1_D2, CLK_TOP_APLL1_SRC_SEL, 1, 2),
	FACTOR1(CLK_TOP_RG_APLL1_D4, CLK_TOP_APLL1_SRC_SEL, 1, 4),
	FACTOR1(CLK_TOP_RG_APLL1_D8, CLK_TOP_APLL1_SRC_SEL, 1, 8),
	FACTOR1(CLK_TOP_RG_APLL1_D16, CLK_TOP_APLL1_SRC_SEL, 1, 16),
	FACTOR1(CLK_TOP_RG_APLL1_D3, CLK_TOP_APLL1_SRC_SEL, 1, 3),
	FACTOR1(CLK_TOP_RG_APLL2_D2, CLK_TOP_APLL2_SRC_SEL, 1, 2),
	FACTOR1(CLK_TOP_RG_APLL2_D4, CLK_TOP_APLL2_SRC_SEL, 1, 4),
	FACTOR1(CLK_TOP_RG_APLL2_D8, CLK_TOP_APLL2_SRC_SEL, 1, 8),
	FACTOR1(CLK_TOP_RG_APLL2_D16, CLK_TOP_APLL2_SRC_SEL, 1, 16),
	FACTOR1(CLK_TOP_RG_APLL2_D3, CLK_TOP_APLL2_SRC_SEL, 1, 3),
	FACTOR1(CLK_TOP_NFI1X_INFRA_BCLK, CLK_TOP_NFI2X_SEL, 1, 2),
	FACTOR1(CLK_TOP_AHB_INFRA_D2, CLK_TOP_AXIBUS_SEL, 1, 2),
};

static const int uart0_parents[] = {
	CLK_TOP_CLK26M,
	CLK_TOP_UNIVPLL_D24
};

static const int emi1x_parents[] = {
	CLK_TOP_CLK26M,
	CLK_TOP_DMPLL
};

static const int emi_ddrphy_parents[] = {
	CLK_TOP_EMI1X_SEL,
	CLK_TOP_EMI1X_SEL
};

static const int msdc1_parents[] = {
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK26M,
	CLK_TOP_UNIVPLL_D6,
	CLK_TOP_CLK_NULL,
	CLK_TOP_MAINPLL_D8,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_UNIVPLL_D8,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_MAINPLL_D16,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_MMPLL_D2,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_MAINPLL_D12
};

static const int pwm_mm_parents[] = {
	CLK_TOP_CLK26M,
	CLK_TOP_UNIVPLL_D12
};

static const int pmicspi_parents[] = {
	CLK_TOP_UNIVPLL_D20,
	CLK_TOP_USB20_48M,
	CLK_TOP_UNIVPLL_D16,
	CLK_TOP_CLK26M,
	CLK_TOP_CLK26M_D2
};

static const int nfi2x_parents[] = {
	CLK_TOP_CLK26M,
	CLK_TOP_MAINPLL_D4,
	CLK_TOP_MAINPLL_D5,
	CLK_TOP_MAINPLL_D6,
	CLK_TOP_MAINPLL_D7,
	CLK_TOP_MAINPLL_D8,
	CLK_TOP_MAINPLL_D10,
	CLK_TOP_MAINPLL_D12
};

static const int ddrphycfg_parents[] = {
	CLK_TOP_CLK26M,
	CLK_TOP_MAINPLL_D16
};

static const int smi_parents[] = {
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK26M,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_UNIVPLL_D4,
	CLK_TOP_MAINPLL_D7,
	CLK_TOP_CLK_NULL,
	CLK_TOP_MAINPLL_D14
};

static const int usb_parents[] = {
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK26M,
	CLK_TOP_UNIVPLL_D16,
	CLK_TOP_CLK_NULL,
	CLK_TOP_MAINPLL_D20
};

static const int spinor_parents[] = {
	CLK_TOP_CLK26M_D2,
	CLK_TOP_CLK26M,
	CLK_TOP_MAINPLL_D40,
	CLK_TOP_UNIVPLL_D24,
	CLK_TOP_UNIVPLL_D20,
	CLK_TOP_MAINPLL_D20,
	CLK_TOP_MAINPLL_D16,
	CLK_TOP_UNIVPLL_D12
};

static const int eth_parents[] = {
	CLK_TOP_CLK26M,
	CLK_TOP_MAINPLL_D40,
	CLK_TOP_UNIVPLL_D24,
	CLK_TOP_UNIVPLL_D20,
	CLK_TOP_MAINPLL_D20
};

static const int aud1_parents[] = {
	CLK_TOP_CLK26M,
	CLK_TOP_APLL1_SRC_SEL
};

static const int aud2_parents[] = {
	CLK_TOP_CLK26M,
	CLK_TOP_APLL2_SRC_SEL
};

static const int i2c_parents[] = {
	CLK_TOP_CLK26M,
	CLK_TOP_USB20_48M,
	CLK_TOP_UNIVPLL_D12,
	CLK_TOP_UNIVPLL_D10,
	CLK_TOP_UNIVPLL_D8
};

static const int aud_i2s0_m_parents[] = {
	CLK_TOP_AUD1,
	CLK_TOP_AUD2
};

static const int aud_spdifin_parents[] = {
	CLK_TOP_CLK26M,
	CLK_TOP_UNIVPLL_D2,
	CLK_TOP_TVDPLL
};

static const int dbg_atclk_parents[] = {
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK26M,
	CLK_TOP_MAINPLL_D5,
	CLK_TOP_CLK_NULL,
	CLK_TOP_UNIVPLL_D5
};

static const int png_sys_parents[] = {
	CLK_TOP_CLK26M,
	CLK_TOP_UNIVPLL_D8,
	CLK_TOP_MAINPLL_D7,
	CLK_TOP_MAINPLL_D6,
	CLK_TOP_MAINPLL_D5,
	CLK_TOP_UNIVPLL_D3
};

static const int sej_13m_parents[] = {
	CLK_TOP_CLK26M,
	CLK_TOP_CLK26M_D2
};

static const int imgrz_sys_parents[] = {
	CLK_TOP_CLK26M,
	CLK_TOP_MAINPLL_D6,
	CLK_TOP_MAINPLL_D7,
	CLK_TOP_MAINPLL_D5,
	CLK_TOP_UNIVPLL_D4,
	CLK_TOP_UNIVPLL_D10,
	CLK_TOP_UNIVPLL_D5,
	CLK_TOP_UNIVPLL_D6
};

static const int graph_eclk_parents[] = {
	CLK_TOP_CLK26M,
	CLK_TOP_MAINPLL_D6,
	CLK_TOP_UNIVPLL_D8,
	CLK_TOP_UNIVPLL_D16,
	CLK_TOP_MAINPLL_D7,
	CLK_TOP_UNIVPLL_D4,
	CLK_TOP_UNIVPLL_D10,
	CLK_TOP_UNIVPLL_D24,
	CLK_TOP_MAINPLL_D8
};

static const int fdbi_parents[] = {
	CLK_TOP_CLK26M,
	CLK_TOP_MAINPLL_D12,
	CLK_TOP_MAINPLL_D14,
	CLK_TOP_MAINPLL_D16,
	CLK_TOP_UNIVPLL_D10,
	CLK_TOP_UNIVPLL_D12,
	CLK_TOP_UNIVPLL_D16,
	CLK_TOP_UNIVPLL_D24,
	CLK_TOP_TVDPLL_D2,
	CLK_TOP_TVDPLL_D4,
	CLK_TOP_TVDPLL_D8,
	CLK_TOP_TVDPLL_D16
};

static const int faudio_parents[] = {
	CLK_TOP_CLK26M,
	CLK_TOP_UNIVPLL_D24,
	CLK_TOP_APLL1_D4,
	CLK_TOP_APLL2_D4
};

static const int fa2sys_parents[] = {
	CLK_TOP_CLK26M,
	CLK_TOP_APLL1_SRC_SEL,
	CLK_TOP_RG_APLL1_D2,
	CLK_TOP_RG_APLL1_D4,
	CLK_TOP_RG_APLL1_D8,
	CLK_TOP_RG_APLL1_D16,
	CLK_TOP_CLK26M_D2,
	CLK_TOP_RG_APLL1_D3
};

static const int fa1sys_parents[] = {
	CLK_TOP_CLK26M,
	CLK_TOP_APLL2_SRC_SEL,
	CLK_TOP_RG_APLL2_D2,
	CLK_TOP_RG_APLL2_D4,
	CLK_TOP_RG_APLL2_D8,
	CLK_TOP_RG_APLL2_D16,
	CLK_TOP_CLK26M_D2,
	CLK_TOP_RG_APLL2_D3
};

static const int fasm_m_parents[] = {
	CLK_TOP_CLK26M,
	CLK_TOP_UNIVPLL_D12,
	CLK_TOP_UNIVPLL_D6,
	CLK_TOP_MAINPLL_D7
};

static const int fecc_ck_parents[] = {
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK26M,
	CLK_TOP_UNIVPLL_D6,
	CLK_TOP_CLK_NULL,
	CLK_TOP_UNIVPLL_D4,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_UNIVPLL_D3,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_MAINPLL_D3
};

static const int pe2_mac_parents[] = {
	CLK_TOP_CLK26M,
	CLK_TOP_MAINPLL_D11,
	CLK_TOP_MAINPLL_D16,
	CLK_TOP_UNIVPLL_D12,
	CLK_TOP_UNIVPLL_D10
};

static const int cmsys_parents[] = {
	CLK_TOP_CLK26M,
	CLK_TOP_UNIVPLL_D5,
	CLK_TOP_UNIVPLL_D6,
	CLK_TOP_MAINPLL_D5,
	CLK_TOP_APLL2,
	CLK_TOP_APLL2_D2,
	CLK_TOP_APLL2_D4,
	CLK_TOP_APLL2_D3
};

static const int gcpu_parents[] = {
	CLK_TOP_CLK26M,
	CLK_TOP_MAINPLL_D4,
	CLK_TOP_MAINPLL_D5,
	CLK_TOP_MAINPLL_D6,
	CLK_TOP_MAINPLL_D7,
	CLK_TOP_UNIVPLL_D4,
	CLK_TOP_UNIVPLL_D10,
	CLK_TOP_UNIVPLL_D3
};

static const int spis_ck_parents[] = {
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK26M,
	CLK_TOP_UNIVPLL_D12,
	CLK_TOP_CLK_NULL,
	CLK_TOP_UNIVPLL_D8,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_UNIVPLL_D6,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_UNIVPLL_D5,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_UNIVPLL_D4,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_MAINPLL_D4,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_UNIVPLL_D3
};

static const int apll1_ref_parents[] = {
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL
};

static const int int_32k_parents[] = {
	CLK_TOP_CLK32K,
	CLK_TOP_CLK26M_D793
};

static const int apll1_src_parents[] = {
	CLK_TOP_APLL1,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL
};

static const int apll2_src_parents[] = {
	CLK_TOP_APLL2,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL
};

static const int faud_intbus_parents[] = {
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK26M,
	CLK_TOP_MAINPLL_D11,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK26M,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_UNIVPLL_D10,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_RG_APLL2_D8,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK26M_D2,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_RG_APLL1_D8,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_UNIVPLL_D20
};

static const int axibus_parents[] = {
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK26M,
	CLK_TOP_MAINPLL_D11,
	CLK_TOP_CLK_NULL,
	CLK_TOP_MAINPLL_D12,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_UNIVPLL_D10,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK26M_D2,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_APLL2_D8
};

static const int hapll1_parents[] = {
	CLK_TOP_CLK26M,
	CLK_TOP_APLL1_SRC_SEL,
	CLK_TOP_RG_APLL1_D2,
	CLK_TOP_RG_APLL1_D4,
	CLK_TOP_RG_APLL1_D8,
	CLK_TOP_RG_APLL1_D16,
	CLK_TOP_CLK26M_D2,
	CLK_TOP_CLK26M_D8,
	CLK_TOP_RG_APLL1_D3
};

static const int hapll2_parents[] = {
	CLK_TOP_CLK26M,
	CLK_TOP_APLL2_SRC_SEL,
	CLK_TOP_RG_APLL2_D2,
	CLK_TOP_RG_APLL2_D4,
	CLK_TOP_RG_APLL2_D8,
	CLK_TOP_RG_APLL2_D16,
	CLK_TOP_CLK26M_D2,
	CLK_TOP_CLK26M_D4,
	CLK_TOP_RG_APLL2_D3
};

static const int spinfi_parents[] = {
	CLK_TOP_CLK26M,
	CLK_TOP_UNIVPLL_D24,
	CLK_TOP_UNIVPLL_D20,
	CLK_TOP_MAINPLL_D22,
	CLK_TOP_UNIVPLL_D16,
	CLK_TOP_MAINPLL_D16,
	CLK_TOP_UNIVPLL_D12,
	CLK_TOP_UNIVPLL_D10,
	CLK_TOP_MAINPLL_D11
};

static const int msdc0_parents[] = {
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK26M,
	CLK_TOP_UNIVPLL_D6,
	CLK_TOP_CLK_NULL,
	CLK_TOP_MAINPLL_D8,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_UNIVPLL_D8,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_MAINPLL_D16,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_MAINPLL_D12,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_APMIXED_MMPLL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_MMPLL_D2
};

static const int msdc0_clk50_parents[] = {
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK26M,
	CLK_TOP_UNIVPLL_D6,
	CLK_TOP_CLK_NULL,
	CLK_TOP_MAINPLL_D8,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_UNIVPLL_D8,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_MAINPLL_D6
};

static const int msdc2_parents[] = {
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK26M,
	CLK_TOP_UNIVPLL_D6,
	CLK_TOP_CLK_NULL,
	CLK_TOP_MAINPLL_D8,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_UNIVPLL_D8,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_MAINPLL_D16,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_MMPLL_D2,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_MAINPLL_D12,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_APMIXED_MMPLL
};

static const int disp_dpi_ck_parents[] = {
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK26M,
	CLK_TOP_TVDPLL_D2,
	CLK_TOP_CLK_NULL,
	CLK_TOP_TVDPLL_D4,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_TVDPLL_D8,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_CLK_NULL,
	CLK_TOP_TVDPLL_D16
};

static const struct mtk_composite top_muxes[] = {
	/* CLK_MUX_SEL0 */
	MUX(CLK_TOP_UART0_SEL, uart0_parents, 0x000, 0, 1),
	MUX(CLK_TOP_EMI1X_SEL, emi1x_parents, 0x000, 1, 1),
	MUX(CLK_TOP_EMI_DDRPHY_SEL, emi_ddrphy_parents, 0x000, 2, 1),
	MUX(CLK_TOP_MSDC1_SEL, msdc1_parents, 0x000, 4, 8),
	MUX(CLK_TOP_PWM_MM_SEL, pwm_mm_parents, 0x000, 18, 1),
	MUX(CLK_TOP_UART1_SEL, uart0_parents, 0x000, 19, 1),
	MUX(CLK_TOP_SPM_52M_SEL, uart0_parents, 0x000, 22, 1),
	MUX(CLK_TOP_PMICSPI_SEL, pmicspi_parents, 0x000, 23, 3),
	/* CLK_MUX_SEL1 */
	MUX(CLK_TOP_NFI2X_SEL, nfi2x_parents, 0x004, 0, 3),
	MUX(CLK_TOP_DDRPHYCFG_SEL, ddrphycfg_parents, 0x004, 15, 1),
	MUX(CLK_TOP_SMI_SEL, smi_parents, 0x004, 16, 4),
	MUX(CLK_TOP_USB_SEL, usb_parents, 0x004, 20, 3),
	/* CLK_MUX_SEL8 */
	MUX(CLK_TOP_SPINOR_SEL, spinor_parents, 0x040, 0, 3),
	MUX(CLK_TOP_ETH_SEL, eth_parents, 0x040, 6, 3),
	MUX(CLK_TOP_AUD1_SEL, aud1_parents, 0x040, 22, 1),
	MUX(CLK_TOP_AUD2_SEL, aud2_parents, 0x040, 23, 1),
	MUX(CLK_TOP_I2C_SEL, i2c_parents, 0x040, 28, 3),
	/* CLK_SEL_9 */
	MUX(CLK_TOP_AUD_I2S0_M_SEL, aud_i2s0_m_parents, 0x044, 12, 1),
	MUX(CLK_TOP_AUD_I2S3_M_SEL, aud_i2s0_m_parents, 0x044, 15, 1),
	MUX(CLK_TOP_AUD_I2S4_M_SEL, aud_i2s0_m_parents, 0x044, 16, 1),
	MUX(CLK_TOP_AUD_I2S6_M_SEL, aud_i2s0_m_parents, 0x044, 18, 1),
	/* CLK_MUX_SEL13 */
	MUX(CLK_TOP_PWM_SEL, pwm_mm_parents, 0x07c, 0, 1),
	MUX(CLK_TOP_AUD_SPDIFIN_SEL, aud_spdifin_parents, 0x07c, 2, 2),
	MUX(CLK_TOP_UART2_SEL, uart0_parents, 0x07c, 4, 1),
	MUX(CLK_TOP_DBG_ATCLK_SEL, dbg_atclk_parents, 0x07c, 7, 3),
	MUX(CLK_TOP_PNG_SYS_SEL, png_sys_parents, 0x07c, 16, 3),
	MUX(CLK_TOP_SEJ_13M_SEL, sej_13m_parents, 0x07c, 22, 1),
	/* CLK_MUX_SEL14 */
	MUX(CLK_TOP_IMGRZ_SYS_SEL, imgrz_sys_parents, 0xc0, 0, 3),
	MUX(CLK_TOP_GRAPH_ECLK_SEL, graph_eclk_parents, 0xc0, 8, 4),
	MUX(CLK_TOP_FDBI_SEL, fdbi_parents, 0xc0, 12, 4),
	MUX(CLK_TOP_FAUDIO_SEL, faudio_parents, 0xc0, 16, 2),
	MUX(CLK_TOP_FA2SYS_SEL, fa2sys_parents, 0xc0, 24, 3),
	MUX(CLK_TOP_FA1SYS_SEL, fa1sys_parents, 0xc0, 27, 3),
	MUX(CLK_TOP_FASM_M_SEL, fasm_m_parents, 0xc0, 30, 2),
	/* CLK_MUX_SEL15 */
	MUX(CLK_TOP_FASM_H_SEL, fasm_m_parents, 0xC4, 0, 2),
	MUX(CLK_TOP_FASM_L_SEL, fasm_m_parents, 0xC4, 2, 2),
	MUX(CLK_TOP_FECC_CK_SEL, fecc_ck_parents, 0xC4, 18, 6),
	MUX(CLK_TOP_PE2_MAC_SEL, pe2_mac_parents, 0xC4, 24, 3),
	MUX(CLK_TOP_CMSYS_SEL, cmsys_parents, 0xC4, 28, 3),
	/* CLK_MUX_SEL16 */
	MUX(CLK_TOP_GCPU_SEL, gcpu_parents, 0xC8, 0, 3),
	MUX(CLK_TOP_SPIS_CK_SEL, spis_ck_parents, 0xC8, 4, 8),
	/* CLK_MUX_SEL17 */
	MUX(CLK_TOP_APLL1_REF_SEL, apll1_ref_parents, 0xCC, 6, 3),
	MUX(CLK_TOP_APLL2_REF_SEL, apll1_ref_parents, 0xCC, 9, 3),
	MUX(CLK_TOP_INT_32K_SEL, int_32k_parents, 0xCC, 12, 1),
	MUX(CLK_TOP_APLL1_SRC_SEL, apll1_src_parents, 0xCC, 13, 2),
	MUX(CLK_TOP_APLL2_SRC_SEL, apll2_src_parents, 0xCC, 15, 2),
	/* CLK_MUX_SEL19 */
	MUX(CLK_TOP_FAUD_INTBUS_SEL, faud_intbus_parents, 0xD4, 8, 8),
	MUX(CLK_TOP_AXIBUS_SEL, axibus_parents, 0xD4, 24, 8),
	/* CLK_MUX_SEL21 */
	MUX(CLK_TOP_HAPLL1_SEL, hapll1_parents, 0xDC, 0, 4),
	MUX(CLK_TOP_HAPLL2_SEL, hapll2_parents, 0xDC, 4, 4),
	MUX(CLK_TOP_SPINFI_SEL, spinfi_parents, 0xDC, 8, 4),
	/* CLK_MUX_SEL22 */
	MUX(CLK_TOP_MSDC0_SEL, msdc0_parents, 0xF4, 0, 8),
	MUX(CLK_TOP_MSDC0_CLK50_SEL, msdc0_clk50_parents, 0xF4, 8, 6),
	MUX(CLK_TOP_MSDC2_SEL, msdc2_parents, 0xF4, 15, 8),
	MUX(CLK_TOP_MSDC2_CLK50_SEL, msdc0_clk50_parents, 0xF4, 23, 6),
	/* CLK_MUX_SEL23 */
	MUX(CLK_TOP_DISP_DPI_CK_SEL, disp_dpi_ck_parents, 0xF8, 0, 6),
	MUX(CLK_TOP_SPI1_SEL, spis_ck_parents, 0xF8, 6, 8),
	MUX(CLK_TOP_SPI2_SEL, spis_ck_parents, 0xF8, 14, 8),
	MUX(CLK_TOP_SPI3_SEL, spis_ck_parents, 0xF8, 22, 8),
};

static const struct mtk_gate_regs top0_cg_regs = {
	.set_ofs = 0x50,
	.clr_ofs = 0x80,
	.sta_ofs = 0x20,
};

static const struct mtk_gate_regs top1_cg_regs = {
	.set_ofs = 0x54,
	.clr_ofs = 0x84,
	.sta_ofs = 0x24,
};

static const struct mtk_gate_regs top2_cg_regs = {
	.set_ofs = 0x6c,
	.clr_ofs = 0x9c,
	.sta_ofs = 0x3c,
};

static const struct mtk_gate_regs top3_cg_regs = {
	.set_ofs = 0x44,
	.clr_ofs = 0x44,
	.sta_ofs = 0x44,
};

static const struct mtk_gate_regs top4_cg_regs = {
	.set_ofs = 0xa0,
	.clr_ofs = 0xb0,
	.sta_ofs = 0x70,
};

static const struct mtk_gate_regs top5_cg_regs = {
	.set_ofs = 0x120,
	.clr_ofs = 0x140,
	.sta_ofs = 0xe0,
};

static const struct mtk_gate_regs top6_cg_regs = {
	.set_ofs = 0x128,
	.clr_ofs = 0x148,
	.sta_ofs = 0xe8,
};

static const struct mtk_gate_regs top7_cg_regs = {
	.set_ofs = 0x12c,
	.clr_ofs = 0x14c,
	.sta_ofs = 0xec,
};

#define GATE_TOP0(_id, _parent, _shift) {			\
		.id = _id,					\
		.parent = _parent,				\
		.regs = &top0_cg_regs,				\
		.shift = _shift,				\
		.flags = CLK_GATE_SETCLR | CLK_PARENT_TOPCKGEN,	\
	}

#define GATE_TOP1(_id, _parent, _shift) {			\
		.id = _id,					\
		.parent = _parent,				\
		.regs = &top1_cg_regs,				\
		.shift = _shift,				\
		.flags = CLK_GATE_SETCLR | CLK_PARENT_TOPCKGEN,	\
	}

#define GATE_TOP2(_id, _parent, _shift) {			\
		.id = _id,					\
		.parent = _parent,				\
		.regs = &top2_cg_regs,				\
		.shift = _shift,				\
		.flags = CLK_GATE_SETCLR | CLK_PARENT_TOPCKGEN,	\
	}

#define GATE_TOP2_I(_id, _parent, _shift) {				\
		.id = _id,						\
		.parent = _parent,					\
		.regs = &top2_cg_regs,					\
		.shift = _shift,					\
		.flags = CLK_GATE_SETCLR_INV | CLK_PARENT_TOPCKGEN,	\
	}

#define GATE_TOP3(_id, _parent, _shift) {			\
		.id = _id,					\
		.parent = _parent,				\
		.regs = &top3_cg_regs,				\
		.shift = _shift,				\
		.flags = CLK_GATE_NO_SETCLR | CLK_PARENT_TOPCKGEN,	\
	}

#define GATE_TOP4(_id, _parent, _shift) {				\
		.id = _id,						\
		.parent = _parent,					\
		.regs = &top4_cg_regs,					\
		.shift = _shift,					\
		.flags = CLK_GATE_SETCLR | CLK_PARENT_TOPCKGEN,	\
	}

#define GATE_TOP5(_id, _parent, _shift) {				\
		.id = _id,						\
		.parent = _parent,					\
		.regs = &top5_cg_regs,					\
		.shift = _shift,					\
		.flags = CLK_GATE_SETCLR | CLK_PARENT_TOPCKGEN,	\
	}

#define GATE_TOP5_I(_id, _parent, _shift) {				\
		.id = _id,						\
		.parent = _parent,					\
		.regs = &top5_cg_regs,					\
		.shift = _shift,					\
		.flags = CLK_GATE_SETCLR_INV | CLK_PARENT_TOPCKGEN,	\
	}

#define GATE_TOP6(_id, _parent, _shift) {				\
		.id = _id,						\
		.parent = _parent,					\
		.regs = &top6_cg_regs,					\
		.shift = _shift,					\
		.flags = CLK_GATE_SETCLR | CLK_PARENT_TOPCKGEN,	\
	}

#define GATE_TOP7(_id, _parent, _shift) {				\
		.id = _id,						\
		.parent = _parent,					\
		.regs = &top7_cg_regs,					\
		.shift = _shift,					\
		.flags = CLK_GATE_SETCLR_INV | CLK_PARENT_TOPCKGEN,	\
	}

static const struct mtk_gate top_clks[] = {
	/* TOP0 */
	GATE_TOP0(CLK_TOP_PWM_MM, CLK_TOP_PWM_MM_SEL, 0),
	GATE_TOP0(CLK_TOP_SMI, CLK_TOP_SMI_SEL, 9),
	GATE_TOP0(CLK_TOP_SPI2, CLK_TOP_SPI2_SEL, 10),
	GATE_TOP0(CLK_TOP_SPI3, CLK_TOP_SPI3_SEL, 11),
	GATE_TOP0(CLK_TOP_SPINFI, CLK_TOP_SPINFI_SEL, 12),
	GATE_TOP0(CLK_TOP_26M_DEBUG, CLK_TOP_CLK26M, 16),
	GATE_TOP0(CLK_TOP_USB_48M_DEBUG, CLK_TOP_USB20_48M, 17),
	GATE_TOP0(CLK_TOP_52M_DEBUG, CLK_TOP_UNIVPLL_D24, 18),
	GATE_TOP0(CLK_TOP_32K_DEBUG, CLK_TOP_INT_32K_SEL, 19),
	/* TOP1 */
	GATE_TOP1(CLK_TOP_THERM, CLK_TOP_AXIBUS_SEL, 1),
	GATE_TOP1(CLK_TOP_APDMA, CLK_TOP_AXIBUS_SEL, 2),
	GATE_TOP1(CLK_TOP_I2C0, CLK_TOP_AHB_INFRA_D2, 3),
	GATE_TOP1(CLK_TOP_I2C1, CLK_TOP_AHB_INFRA_D2, 4),
	GATE_TOP1(CLK_TOP_AUXADC1, CLK_TOP_CLK26M, 5),
	GATE_TOP1(CLK_TOP_NFI, CLK_TOP_NFI1X_INFRA_BCLK, 6),
	GATE_TOP1(CLK_TOP_NFIECC, CLK_TOP_AXIBUS_SEL, 7),
	GATE_TOP1(CLK_TOP_DEBUGSYS, CLK_TOP_DBG_ATCLK_SEL, 8),
	GATE_TOP1(CLK_TOP_PWM, CLK_TOP_AXIBUS_SEL, 9),
	GATE_TOP1(CLK_TOP_UART0, CLK_TOP_UART0_SEL, 10),
	GATE_TOP1(CLK_TOP_UART1, CLK_TOP_UART1_SEL, 11),
	GATE_TOP1(CLK_TOP_USB, CLK_TOP_USB_B, 13),
	GATE_TOP1(CLK_TOP_FLASHIF_26M, CLK_TOP_CLK26M, 14),
	GATE_TOP1(CLK_TOP_AUXADC2, CLK_TOP_CLK26M, 15),
	GATE_TOP1(CLK_TOP_I2C2, CLK_TOP_AHB_INFRA_D2, 16),
	GATE_TOP1(CLK_TOP_MSDC0, CLK_TOP_MSDC0_SEL, 17),
	GATE_TOP1(CLK_TOP_MSDC1, CLK_TOP_MSDC1_SEL, 18),
	GATE_TOP1(CLK_TOP_NFI2X, CLK_TOP_NFI2X_SEL, 19),
	GATE_TOP1(CLK_TOP_MEMSLP_DLYER, CLK_TOP_CLK26M, 22),
	GATE_TOP1(CLK_TOP_SPI, CLK_TOP_SPI1_SEL, 23),
	GATE_TOP1(CLK_TOP_APXGPT, CLK_TOP_CLK26M, 24),
	GATE_TOP1(CLK_TOP_PMICWRAP_MD, CLK_TOP_CLK26M, 27),
	GATE_TOP1(CLK_TOP_PMICWRAP_CONN, CLK_TOP_PMICSPI_SEL, 28),
	GATE_TOP1(CLK_TOP_PMIC_SYSCK, CLK_TOP_CLK26M, 29),
	GATE_TOP1(CLK_TOP_AUX_ADC, CLK_TOP_CLK26M, 30),
	GATE_TOP1(CLK_TOP_AUX_TP, CLK_TOP_CLK26M, 31),
	/* TOP2 */
	GATE_TOP2(CLK_TOP_RBIST, CLK_TOP_UNIVPLL_D12, 1),
	GATE_TOP2(CLK_TOP_NFI_BUS, CLK_TOP_AXIBUS_SEL, 2),
	GATE_TOP2(CLK_TOP_GCE, CLK_TOP_AXIBUS_SEL, 4),
	GATE_TOP2(CLK_TOP_TRNG, CLK_TOP_AXIBUS_SEL, 5),
	GATE_TOP2(CLK_TOP_PWM_B, CLK_TOP_PWM_SEL, 8),
	GATE_TOP2(CLK_TOP_PWM1_FB, CLK_TOP_PWM_SEL, 9),
	GATE_TOP2(CLK_TOP_PWM2_FB, CLK_TOP_PWM_SEL, 10),
	GATE_TOP2(CLK_TOP_PWM3_FB, CLK_TOP_PWM_SEL, 11),
	GATE_TOP2(CLK_TOP_PWM4_FB, CLK_TOP_PWM_SEL, 12),
	GATE_TOP2(CLK_TOP_PWM5_FB, CLK_TOP_PWM_SEL, 13),
	GATE_TOP2(CLK_TOP_FLASHIF_FREERUN, CLK_TOP_AXIBUS_SEL, 15),
	GATE_TOP2(CLK_TOP_CQDMA, CLK_TOP_AXIBUS_SEL, 17),
	GATE_TOP2(CLK_TOP_66M_ETH, CLK_TOP_AXIBUS_SEL, 19),
	GATE_TOP2(CLK_TOP_133M_ETH, CLK_TOP_AXIBUS_SEL, 20),
	GATE_TOP2(CLK_TOP_FLASHIF_AXI, CLK_TOP_SPI1_SEL, 23),
	GATE_TOP2(CLK_TOP_USBIF, CLK_TOP_AXIBUS_SEL, 24),
	GATE_TOP2(CLK_TOP_UART2, CLK_TOP_RG_UART2, 25),
	GATE_TOP2(CLK_TOP_GCPU_B, CLK_TOP_AXIBUS_SEL, 27),
	GATE_TOP2_I(CLK_TOP_MSDC0_B, CLK_TOP_MSDC0, 28),
	GATE_TOP2_I(CLK_TOP_MSDC1_B, CLK_TOP_MSDC1, 29),
	GATE_TOP2_I(CLK_TOP_MSDC2_B, CLK_TOP_MSDC2, 30),
	GATE_TOP2(CLK_TOP_USB_B, CLK_TOP_USB_SEL, 31),
	/* TOP3 */
	GATE_TOP3(CLK_TOP_APLL12_DIV0, CLK_TOP_APLL12_CK_DIV0, 0),
	GATE_TOP3(CLK_TOP_APLL12_DIV3, CLK_TOP_APLL12_CK_DIV3, 3),
	GATE_TOP3(CLK_TOP_APLL12_DIV4, CLK_TOP_APLL12_CK_DIV4, 4),
	GATE_TOP3(CLK_TOP_APLL12_DIV6, CLK_TOP_APLL12_CK_DIV6, 8),
	/* TOP4 */
	GATE_TOP4(CLK_TOP_SPINOR, CLK_TOP_SPINOR_SEL, 0),
	GATE_TOP4(CLK_TOP_MSDC2, CLK_TOP_MSDC2_SEL, 1),
	GATE_TOP4(CLK_TOP_ETH, CLK_TOP_ETH_SEL, 2),
	GATE_TOP4(CLK_TOP_AUD1, CLK_TOP_AUD1_SEL, 8),
	GATE_TOP4(CLK_TOP_AUD2, CLK_TOP_AUD2_SEL, 9),
	GATE_TOP4(CLK_TOP_I2C, CLK_TOP_I2C_SEL, 12),
	GATE_TOP4(CLK_TOP_PWM_INFRA, CLK_TOP_PWM_SEL, 13),
	GATE_TOP4(CLK_TOP_AUD_SPDIF_IN, CLK_TOP_AUD_SPDIFIN_SEL, 14),
	GATE_TOP4(CLK_TOP_RG_UART2, CLK_TOP_UART2_SEL, 15),
	GATE_TOP4(CLK_TOP_DBG_AT, CLK_TOP_DBG_ATCLK_SEL, 17),
	/* TOP5 */
	GATE_TOP5_I(CLK_TOP_IMGRZ_SYS, CLK_TOP_IMGRZ_SYS_SEL, 0),
	GATE_TOP5_I(CLK_TOP_PNG_SYS, CLK_TOP_PNG_SYS_SEL, 1),
	GATE_TOP5_I(CLK_TOP_GRAPH_E, CLK_TOP_GRAPH_ECLK_SEL, 2),
	GATE_TOP5_I(CLK_TOP_FDBI, CLK_TOP_FDBI_SEL, 3),
	GATE_TOP5_I(CLK_TOP_FAUDIO, CLK_TOP_FAUDIO_SEL, 4),
	GATE_TOP5_I(CLK_TOP_FAUD_INTBUS, CLK_TOP_FAUD_INTBUS_SEL, 5),
	GATE_TOP5_I(CLK_TOP_HAPLL1, CLK_TOP_HAPLL1_SEL, 6),
	GATE_TOP5_I(CLK_TOP_HAPLL2, CLK_TOP_HAPLL2_SEL, 7),
	GATE_TOP5_I(CLK_TOP_FA2SYS, CLK_TOP_FA2SYS_SEL, 8),
	GATE_TOP5_I(CLK_TOP_FA1SYS, CLK_TOP_FA1SYS_SEL, 9),
	GATE_TOP5_I(CLK_TOP_FASM_L, CLK_TOP_FASM_L_SEL, 10),
	GATE_TOP5_I(CLK_TOP_FASM_M, CLK_TOP_FASM_M_SEL, 11),
	GATE_TOP5_I(CLK_TOP_FASM_H, CLK_TOP_FASM_H_SEL, 12),
	GATE_TOP5_I(CLK_TOP_FECC, CLK_TOP_FECC_CK_SEL, 23),
	GATE_TOP5_I(CLK_TOP_PE2_MAC, CLK_TOP_PE2_MAC_SEL, 24),
	GATE_TOP5_I(CLK_TOP_CMSYS, CLK_TOP_CMSYS_SEL, 25),
	GATE_TOP5_I(CLK_TOP_GCPU, CLK_TOP_GCPU_SEL, 26),
	GATE_TOP5(CLK_TOP_SPIS, CLK_TOP_SPIS_CK_SEL, 27),
	/* TOP6 */
	GATE_TOP6(CLK_TOP_I2C3, CLK_TOP_AHB_INFRA_D2, 0),
	GATE_TOP6(CLK_TOP_SPI_SLV_B, CLK_TOP_SPIS_CK_SEL, 1),
	GATE_TOP6(CLK_TOP_SPI_SLV_BUS, CLK_TOP_AXIBUS_SEL, 2),
	GATE_TOP6(CLK_TOP_PCIE_MAC_BUS, CLK_TOP_AXIBUS_SEL, 3),
	GATE_TOP6(CLK_TOP_CMSYS_BUS, CLK_TOP_AXIBUS_SEL, 4),
	GATE_TOP6(CLK_TOP_ECC_B, CLK_TOP_AXIBUS_SEL, 5),
	GATE_TOP6(CLK_TOP_PCIE_PHY_BUS, CLK_TOP_CLK26M, 6),
	GATE_TOP6(CLK_TOP_PCIE_AUX, CLK_TOP_CLK26M, 7),
	/* TOP7 */
	GATE_TOP7(CLK_TOP_DISP_DPI, CLK_TOP_DISP_DPI_CK_SEL, 0),
};

static const struct mtk_clk_tree mt8518_clk_tree = {
	.xtal_rate = 26 * MHZ,
	.xtal2_rate = 26 * MHZ,
	.fdivs_offs = CLK_TOP_DMPLL,
	.muxes_offs = CLK_TOP_UART0_SEL,
	.plls = apmixed_plls,
	.fclks = top_fixed_clks,
	.fdivs = top_fixed_divs,
	.muxes = top_muxes,
};

static int mt8518_apmixedsys_probe(struct udevice *dev)
{
	return mtk_common_clk_init(dev, &mt8518_clk_tree);
}

static int mt8518_topckgen_probe(struct udevice *dev)
{
	return mtk_common_clk_init(dev, &mt8518_clk_tree);
}

static int mt8518_topckgen_cg_probe(struct udevice *dev)
{
	return mtk_common_clk_gate_init(dev, &mt8518_clk_tree, top_clks);
}

static const struct udevice_id mt8518_apmixed_compat[] = {
	{ .compatible = "mediatek,mt8518-apmixedsys", },
	{ }
};

static const struct udevice_id mt8518_topckgen_compat[] = {
	{ .compatible = "mediatek,mt8518-topckgen", },
	{ }
};

static const struct udevice_id mt8518_topckgen_cg_compat[] = {
	{ .compatible = "mediatek,mt8518-topckgen-cg", },
	{ }
};

U_BOOT_DRIVER(mtk_clk_apmixedsys) = {
	.name = "mt8518-apmixedsys",
	.id = UCLASS_CLK,
	.of_match = mt8518_apmixed_compat,
	.probe = mt8518_apmixedsys_probe,
	.priv_auto	= sizeof(struct mtk_clk_priv),
	.ops = &mtk_clk_apmixedsys_ops,
	.flags = DM_FLAG_PRE_RELOC,
};

U_BOOT_DRIVER(mtk_clk_topckgen) = {
	.name = "mt8518-topckgen",
	.id = UCLASS_CLK,
	.of_match = mt8518_topckgen_compat,
	.probe = mt8518_topckgen_probe,
	.priv_auto	= sizeof(struct mtk_clk_priv),
	.ops = &mtk_clk_topckgen_ops,
	.flags = DM_FLAG_PRE_RELOC,
};

U_BOOT_DRIVER(mtk_clk_topckgen_cg) = {
	.name = "mt8518-topckgen-cg",
	.id = UCLASS_CLK,
	.of_match = mt8518_topckgen_cg_compat,
	.probe = mt8518_topckgen_cg_probe,
	.priv_auto	= sizeof(struct mtk_cg_priv),
	.ops = &mtk_clk_gate_ops,
	.flags = DM_FLAG_PRE_RELOC,
};
