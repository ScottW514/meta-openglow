/*
 * Copyright (C) 2018, Scott Wiederhold <s.e.wiederhold@gmail.com>
 * Copyright (C) 2013, Boundary Devices <info@boundarydevices.com>
 * Copyright (C) 2010-2013 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/iomux.h>
#include <asm/arch/sys_proto.h>
#include <malloc.h>
#include <asm/arch/mx6-pins.h>
#include <linux/errno.h>
#include <asm/gpio.h>
#include <i2c.h>
#include <pwm.h>
#include <asm/mach-imx/iomux-v3.h>
#include <asm/mach-imx/spi.h>
#include <asm/mach-imx/boot_mode.h>
#include <asm/mach-imx/mxc_i2c.h>
#include <mmc.h>
#include <fsl_esdhc.h>
#include <asm/arch/crm_regs.h>
#include <input.h>

DECLARE_GLOBAL_DATA_PTR;

#define FAN_CONTROLLER_ADDR	0x2C
#define LID_LED_PWM		3

#define I2C_PAD_CTRL	(PAD_CTL_PUS_100K_UP |			\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm | PAD_CTL_HYS |	\
	PAD_CTL_ODE | PAD_CTL_SRE_FAST)

#define UART_PAD_CTRL  (PAD_CTL_PUS_100K_UP |			\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm |			\
	PAD_CTL_SRE_FAST  | PAD_CTL_HYS)

#define USDHC_PAD_CTRL (PAD_CTL_PUS_47K_UP |			\
	PAD_CTL_SPEED_LOW | PAD_CTL_DSE_80ohm |			\
	PAD_CTL_SRE_FAST  | PAD_CTL_HYS)

#define PWM_PAD_CTRL (PAD_CTL_SPEED_MED |  			\
	PAD_CTL_DSE_40ohm | PAD_CTL_SRE_FAST | 			\
	PAD_CTL_PUE | PAD_CTL_PKE)

#define SPI_PAD_CTRL (PAD_CTL_HYS | PAD_CTL_SPEED_MED |		\
	PAD_CTL_DSE_40ohm     | PAD_CTL_SRE_FAST)

#define WEAK_PULLUP	(PAD_CTL_PUS_100K_UP |			\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm | PAD_CTL_HYS |	\
	PAD_CTL_SRE_SLOW)

#define WEAK_PULLDOWN	(PAD_CTL_PUS_100K_DOWN |		\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm |			\
	PAD_CTL_HYS | PAD_CTL_SRE_SLOW)

#define I2C_PAD MUX_PAD_CTRL(I2C_PAD_CTRL)

int dram_init(void)
{
	gd->ram_size = ((ulong)CONFIG_DDR_MB * 1024 * 1024);

	return 0;
}


static iomux_v3_cfg_t const uart1_pads[] = {
	MX6_PAD_SD3_DAT6__UART1_RX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX6_PAD_SD3_DAT7__UART1_TX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL),
};

static iomux_v3_cfg_t const usdhc3_pads[] = {
	MX6_PAD_SD3_CLK__SD3_CLK   | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_CMD__SD3_CMD   | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DAT0__SD3_DATA0 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DAT1__SD3_DATA1 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DAT2__SD3_DATA2 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DAT3__SD3_DATA3 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_EIM_DA6__GPIO3_IO06    | MUX_PAD_CTRL(NO_PAD_CTRL), /* CD */
};

static iomux_v3_cfg_t const usdhc4_pads[] = {
	MX6_PAD_SD4_CLK__SD4_CLK | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD4_CMD__SD4_CMD | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD4_DAT0__SD4_DATA0 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD4_DAT1__SD4_DATA1 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD4_DAT2__SD4_DATA2 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD4_DAT3__SD4_DATA3 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD4_DAT4__SD4_DATA4 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD4_DAT5__SD4_DATA5 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD4_DAT6__SD4_DATA6 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD4_DAT7__SD4_DATA7 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_NANDF_D0__GPIO2_IO00 | MUX_PAD_CTRL(WEAK_PULLUP), /* RESET */
};

static void setup_iomux_uart(void)
{
	imx_iomux_v3_setup_multiple_pads(uart1_pads, ARRAY_SIZE(uart1_pads));
}

#ifdef CONFIG_SYS_I2C_MXC

#define I2C_PADS(name, scl_i2c, scl_gpio, scl_gp, sda_i2c, sda_gpio, sda_gp) \
	struct i2c_pads_info i2c_pad_info##name = {		\
		.scl = {.i2c_mode = scl_i2c | I2C_PAD, .gpio_mode = scl_gpio | I2C_PAD, .gp = scl_gp}, \
		.sda = {.i2c_mode = sda_i2c | I2C_PAD, .gpio_mode = sda_gpio | I2C_PAD, .gp = sda_gp}};
#define I2C_PADS_INFO(name) &i2c_pad_info##name

I2C_PADS(I2C_1,
	MX6_PAD_EIM_D21__I2C1_SCL, MX6_PAD_EIM_D21__GPIO3_IO21, IMX_GPIO_NR(3, 21),
	MX6_PAD_EIM_D28__I2C1_SDA, MX6_PAD_EIM_D28__GPIO3_IO28, IMX_GPIO_NR(3, 28))
I2C_PADS(I2C_2,
	MX6_PAD_KEY_COL3__I2C2_SCL, MX6_PAD_KEY_COL3__GPIO4_IO12, IMX_GPIO_NR(4, 12),
	MX6_PAD_KEY_ROW3__I2C2_SDA, MX6_PAD_KEY_ROW3__GPIO4_IO13, IMX_GPIO_NR(4, 13))
I2C_PADS(I2C_3,
	MX6_PAD_GPIO_3__I2C3_SCL, MX6_PAD_GPIO_3__GPIO1_IO03, IMX_GPIO_NR(1, 3),
	MX6_PAD_GPIO_16__I2C3_SDA, MX6_PAD_GPIO_16__GPIO7_IO11, IMX_GPIO_NR(7, 11))

#ifdef FAN_CONTROLLER_ADDR
int i2c_fan_device_init(void)
{
	uint8_t data;
	int err;

	i2c_set_bus_num(2);
	if (err) {
		printf("Failure changing I2C bus number (%d)\n", err);
	}
	i2c_read(FAN_CONTROLLER_ADDR, 0xfe, 1, &data, 1);
	if (data != 0x5D) {
		printf("Fan Controller I2C Manufacturer ID invalid (%x)\n", data);
		return -ENODEV;
	}
	i2c_read(FAN_CONTROLLER_ADDR, 0xfd, 1, &data, 1);
	if (data != 0x35) {
		printf("Fan Controller I2C Product ID invalid (%x)\n", data);
		return -ENODEV;
	}

	/* PWM Outputs to Push-Pull */
	data = 0x07;
	err = i2c_write(FAN_CONTROLLER_ADDR, 0x2b, 1, &data, 1);
	/* Set all fan outputs to 0 */
	data = 0x00;
	err += i2c_write(FAN_CONTROLLER_ADDR, 0x30, 1, &data, 1);
	err += i2c_write(FAN_CONTROLLER_ADDR, 0x40, 1, &data, 1);
	err += i2c_write(FAN_CONTROLLER_ADDR, 0x50, 1, &data, 1);
	if (err) {
		printf("Failed to initialize Fan Controller\n");
		return -ENODEV;
	}

	printf("Fans:  Initialized\n");
	return 0;
}
#endif /* FAN_CONTROLLER_ADDR */

#endif /* CONFIG_SYS_I2C_MXC */

#ifdef LID_LED_PWM
static iomux_v3_cfg_t const pwm4_pads[] = {
	MX6_PAD_SD1_CMD__PWM4_OUT | MUX_PAD_CTRL(PWM_PAD_CTRL)};

int pwm_lid_led_init(void)
{
	imx_iomux_v3_setup_multiple_pads(pwm4_pads, ARRAY_SIZE(pwm4_pads));
	/* 500Hz, 10% duty cycle */
	if (pwm_config(LID_LED_PWM, 200000, 2000000)) {
		printf("Failed to config Lid LED PWM\n");
		return -ENODEV;
	}
	if (pwm_enable(LID_LED_PWM)) {
		printf("Failed to enable Lid LED PWM\n");
		return -ENODEV;
	}
	printf("Lid LED: Initialized\n");
	return 0;
}
#endif /* LID_LED_PWM */

#ifdef CONFIG_FSL_ESDHC
static struct fsl_esdhc_cfg usdhc_cfg[2] = {
	{USDHC3_BASE_ADDR},
	{USDHC4_BASE_ADDR},
};


int board_mmc_getcd(struct mmc *mmc)
{
	struct fsl_esdhc_cfg *cfg = (struct fsl_esdhc_cfg *)mmc->priv;
	if (cfg->esdhc_base == USDHC3_BASE_ADDR) {
		int gp_cd = IMX_GPIO_NR(3, 6);
		gpio_direction_input(gp_cd);
		return !gpio_get_value(gp_cd);
	} else {
		return 1;
	}
}


int board_mmc_init(bd_t *bis)
{
	int ret;
	u32 index = 0;

	usdhc_cfg[0].sdhc_clk = mxc_get_clock(MXC_ESDHC3_CLK);
	usdhc_cfg[1].sdhc_clk = mxc_get_clock(MXC_ESDHC4_CLK);

	usdhc_cfg[0].max_bus_width = 4;
	usdhc_cfg[1].max_bus_width = 8;

	for (index = 0; index < CONFIG_SYS_FSL_USDHC_NUM; ++index) {
		switch (index) {
		case 0:
			imx_iomux_v3_setup_multiple_pads(
				usdhc3_pads, ARRAY_SIZE(usdhc3_pads));
			break;
		case 1:
		       imx_iomux_v3_setup_multiple_pads(
			       usdhc4_pads, ARRAY_SIZE(usdhc4_pads));
		       break;
		default:
		       printf("Warning: you configured more USDHC controllers"
			       "(%d) then supported by the board (%d)\n",
			       index + 1, CONFIG_SYS_FSL_USDHC_NUM);
		       return -EINVAL;
		}

		ret = fsl_esdhc_initialize(bis, &usdhc_cfg[index]);
		if (ret)
			return ret;
	}

	return 0;
}
#endif


#ifdef CONFIG_MXC_SPI
int board_spi_cs_gpio(unsigned bus, unsigned cs)
{
	return (bus == 0 && cs == 0) ? (IMX_GPIO_NR(3, 19)) : -1;
}

static iomux_v3_cfg_t const ecspi1_pads[] = {
	/* SS1 */
	MX6_PAD_EIM_D19__GPIO3_IO19  | MUX_PAD_CTRL(NO_PAD_CTRL),
	MX6_PAD_EIM_D17__ECSPI1_MISO | MUX_PAD_CTRL(SPI_PAD_CTRL),
	MX6_PAD_EIM_D18__ECSPI1_MOSI | MUX_PAD_CTRL(SPI_PAD_CTRL),
	MX6_PAD_EIM_D16__ECSPI1_SCLK | MUX_PAD_CTRL(SPI_PAD_CTRL),
};

static void setup_spi(void)
{
	imx_iomux_v3_setup_multiple_pads(ecspi1_pads,
					 ARRAY_SIZE(ecspi1_pads));
}
#endif

static unsigned gpios_out_low[] = {
	IMX_GPIO_NR(3, 0),	/* disable HIGH VOLTAGE Watch Dog*/
	IMX_GPIO_NR(7, 12),	/* disable HIGH VOLTAGE FIRE*/
	IMX_GPIO_NR(1, 21),	/* disable HIGH VOLTAGE PWM */
	IMX_GPIO_NR(3, 2),	/* disable 40VDC */
	IMX_GPIO_NR(1, 0),	/* disable USB otg power */
	IMX_GPIO_NR(1, 2),	/* disable wireless */
	IMX_GPIO_NR(5, 19),	/* disable bluetooth */
	IMX_GPIO_NR(1, 17),	/* disable Water Sensor Heater PWM */
	IMX_GPIO_NR(2, 23),	/* disable TEC */
};

static unsigned gpios_out_high[] = {
	IMX_GPIO_NR(3, 23),	/* enable Water Pump */
	IMX_GPIO_NR(2, 2),	/* enable 12VDC */
};

static void set_gpios(unsigned *p, int cnt, int val)
{
	int i;

	for (i = 0; i < cnt; i++)
		gpio_direction_output(*p++, val);
}


int board_early_init_f(void)
{
	setup_iomux_uart();

	set_gpios(gpios_out_high, ARRAY_SIZE(gpios_out_high), 1);
	set_gpios(gpios_out_low, ARRAY_SIZE(gpios_out_low), 0);

	return 0;
}

/*
 * Do not overwrite the console
 * Use always serial for U-Boot console
 */
int overwrite_console(void)
{
	return 1;
}


int board_init(void)
{
	struct iomuxc *const iomuxc_regs = (struct iomuxc *)IOMUXC_BASE_ADDR;

	clrsetbits_le32(&iomuxc_regs->gpr[1],
			IOMUXC_GPR1_OTG_ID_MASK,
			IOMUXC_GPR1_OTG_ID_GPIO1);

#ifdef CONFIG_SYS_I2C_MXC
	setup_i2c(0, CONFIG_SYS_I2C_SPEED, 0x7f, I2C_PADS_INFO(I2C_1));
	setup_i2c(1, CONFIG_SYS_I2C_SPEED, 0x7f, I2C_PADS_INFO(I2C_2));
	setup_i2c(2, CONFIG_SYS_I2C_SPEED, 0x7f, I2C_PADS_INFO(I2C_3));

#ifdef FAN_CONTROLLER_ADDR
	i2c_fan_device_init();
#endif

#endif /* CONFIG_SYS_I2C_MXC */

#ifdef LID_LED_PWM
	pwm_lid_led_init();
#endif

	/* address of boot parameters */
	gd->bd->bi_boot_params = PHYS_SDRAM + 0x100;

#ifdef CONFIG_MXC_SPI
	setup_spi();
#endif

	return 0;
}


int checkboard(void)
{
	puts("Board: OpenGlow STD Prototype 2\n");
	return 0;
}


#ifdef CONFIG_CMD_BMODE
static const struct boot_mode board_boot_modes[] = {
	/* 4 bit bus width */
	{"mmc0",	MAKE_CFGVAL(0x40, 0x30, 0x00, 0x00)},
	{"mmc1",	MAKE_CFGVAL(0x40, 0x38, 0x00, 0x00)},
	{NULL,		0},
};
#endif


int misc_init_r(void)
{
#ifdef CONFIG_CMD_BMODE
	add_board_boot_modes(board_boot_modes);
#endif
	env_set_hex("reset_cause", get_imx_reset_cause());
	return 0;
}
