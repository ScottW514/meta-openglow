/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * include/linux/platform_data/dma-imx-sdma.h
 *
 * ForgeFIRM: re-created for linux-fslc 6.12 (mainline removed this header and
 * folded the SDMA platform data into <linux/dma/imx-dma.h> + imx-sdma.c).
 *
 * Exposes the subset of the i.MX SDMA engine that glowforge.ko's cnc
 * step-stream driver needs: the per-channel context register layout plus the
 * helper API implemented and EXPORT_SYMBOL'd in drivers/dma/imx-sdma.c by the
 * ForgeFIRM expose patch.
 *
 * The struct layouts mirror imx-sdma.c exactly (they are passed across the
 * module boundary by pointer). struct sdma_engine / sdma_channel are kept
 * opaque here; glowforge.ko only holds handles to them.
 *
 * Copyright 2018-2026 514 LLC d/b/a OpenGlow
 * Written by Scott Wiederhold
 * Portions Copyright (C) 2015-2018 Glowforge, Inc. <opensource@glowforge.com>
 */
#ifndef __LINUX_PLATFORM_DATA_DMA_IMX_SDMA_H__
#define __LINUX_PLATFORM_DATA_DMA_IMX_SDMA_H__

#include <linux/types.h>
#include <linux/dmaengine.h>

/* Contract guard: both this header's mirrored context struct and the
 * driver's own definition must be exactly this many bytes (32 words); each
 * side BUILD_BUG_ONs against it, so layout drift on either side breaks the
 * build instead of silently corrupting partial context loads/fetches. */
#define SDMA_CONTEXT_DATA_EXPECTED_SIZE 128

/* imx-sdma.c defines its own identically-named struct; it defines this guard
 * macro before including us so only the prototypes (and the size contract
 * above) are shared with the driver itself. */
#ifndef IMX_SDMA_DRIVER_HAS_CONTEXT_STRUCT

/* SDMA channel context register file (mirrors drivers/dma/imx-sdma.c). */
struct sdma_state_registers {
	u32 pc     :14;
	u32 unused1: 1;
	u32 t      : 1;
	u32 rpc    :14;
	u32 unused0: 1;
	u32 sf     : 1;
	u32 spc    :14;
	u32 unused2: 1;
	u32 df     : 1;
	u32 epc    :14;
	u32 lm     : 2;
} __attribute__ ((packed));

struct sdma_context_data {
	struct sdma_state_registers  channel_state;
	u32  gReg[8];
	u32  mda;
	u32  msa;
	u32  ms;
	u32  md;
	u32  pda;
	u32  psa;
	u32  ps;
	u32  pd;
	u32  ca;
	u32  cs;
	u32  dda;
	u32  dsa;
	u32  ds;
	u32  dd;
	u32  scratch0;
	u32  scratch1;
	u32  scratch2;
	u32  scratch3;
	u32  scratch4;
	u32  scratch5;
	u32  scratch6;
	u32  scratch7;
} __attribute__ ((packed));

#endif /* IMX_SDMA_DRIVER_HAS_CONTEXT_STRUCT */

/* Opaque to consumers; full definitions live in drivers/dma/imx-sdma.c. */
struct sdma_engine;
struct sdma_channel;

struct sdma_engine *sdma_engine_get(void);
struct sdma_channel *sdma_get_channel(struct sdma_engine *sdma, int channel);
void sdma_put_channel(struct sdma_channel *sdmac);
void sdma_setup_channel(struct sdma_channel *sdmac, bool external);
void sdma_event_enable(struct sdma_channel *sdmac, unsigned int event);
void sdma_event_disable(struct sdma_channel *sdmac, unsigned int event);
int  sdma_set_channel_priority(struct sdma_channel *sdmac, unsigned int priority);
void sdma_set_channel_interrupt_callback(struct sdma_channel *sdmac,
		dma_async_tx_callback int_cb, void *cb_param);
int  sdma_load_script(struct sdma_engine *sdma, void *buf, int size, u32 address);
int  sdma_load_partial_context(struct sdma_channel *sdmac,
		struct sdma_context_data *context, u32 byte_offset, u32 num_bytes);
int  sdma_fetch_partial_context(struct sdma_channel *sdmac, void *buf,
		u32 byte_offset, u32 num_bytes);
int  sdma_write_datamem(struct sdma_engine *sdma, void *buf, int size, u32 address);
int  sdma_fetch_datamem(struct sdma_engine *sdma, void *buf, int size, u32 address);
int  sdma_config_ownership(struct sdma_channel *sdmac,
		bool event_override, bool mcu_override, bool dsp_override);
ssize_t sdma_print_context(struct sdma_engine *sdma, int channel, char *buf);

#endif /* __LINUX_PLATFORM_DATA_DMA_IMX_SDMA_H__ */
