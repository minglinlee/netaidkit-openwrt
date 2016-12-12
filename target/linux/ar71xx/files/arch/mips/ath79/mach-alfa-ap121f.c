/*
 *  ALFA NETWORK AP121F board support
 *
 *  Copyright (C) 2011-2012 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/gpio.h>

#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"

#define AP121F_GPIO_LED_WLAN	0
#define AP121F_GPIO_LED_LAN     17
#define AP121F_GPIO_LED_SYS     27

#define AP121F_GPIO_BTN_RESET	12
#define AP121F_GPIO_USB_POWER	26

#define AP121F_KEYS_POLL_INTERVAL	20	/* msecs */
#define AP121F_KEYS_DEBOUNCE_INTERVAL	(3 * AP121F_KEYS_POLL_INTERVAL)

#define AP121F_MAC0_OFFSET		0x0000
#define AP121F_MAC1_OFFSET		0x0006
#define AP121F_CALDATA_OFFSET	0x1000

static struct gpio_led ap121f_leds_gpio[] __initdata = {
	{
		.name		= "alfa:yellow:lan",
		.gpio		= AP121F_GPIO_LED_LAN,
		.active_low	= 1,
	},
	{
		.name		= "alfa:yellow:wlan",
		.gpio		= AP121F_GPIO_LED_WLAN,
		.active_low	= 0,
	},
	{
		.name		= "alfa:yellow:sys",
		.gpio		= AP121F_GPIO_LED_SYS,
		.active_low	= 1,
	},
};

static struct gpio_keys_button ap121f_gpio_keys[] __initdata = {
	{
		.desc		= "Reset button",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = AP121F_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= AP121F_GPIO_BTN_RESET,
		.active_low	= 1,
	}
};

static void __init ap121f_gpio_setup(void)
{
	u32 t;

	ath79_gpio_function_disable(AR933X_GPIO_FUNC_ETH_SWITCH_LED0_EN |
				     AR933X_GPIO_FUNC_ETH_SWITCH_LED1_EN |
				     AR933X_GPIO_FUNC_ETH_SWITCH_LED2_EN |
				     AR933X_GPIO_FUNC_ETH_SWITCH_LED3_EN |
				     AR933X_GPIO_FUNC_ETH_SWITCH_LED4_EN);

	t = ath79_reset_rr(AR933X_RESET_REG_BOOTSTRAP);
	t |= AR933X_BOOTSTRAP_MDIO_GPIO_EN;
	ath79_reset_wr(AR933X_RESET_REG_BOOTSTRAP, t);

	gpio_request_one(AP121F_GPIO_USB_POWER,
			 GPIOF_OUT_INIT_HIGH | GPIOF_EXPORT_DIR_FIXED,
			 "USB power");

}

static void __init ap121f_setup(void)
{
	u8 *art = (u8 *) KSEG1ADDR(0x1fff0000);

	ap121f_gpio_setup();

	ath79_register_m25p80(NULL);
	ath79_register_leds_gpio(-1, ARRAY_SIZE(ap121f_leds_gpio),
					ap121f_leds_gpio);
	ath79_register_gpio_keys_polled(-1, AP121F_KEYS_POLL_INTERVAL,
					 ARRAY_SIZE(ap121f_gpio_keys),
					 ap121f_gpio_keys);

	ath79_init_mac(ath79_eth1_data.mac_addr,
			art + AP121F_MAC0_OFFSET, 0);
	ath79_init_mac(ath79_eth0_data.mac_addr,
			art + AP121F_MAC1_OFFSET, 0);

	ath79_register_mdio(0, 0x0);

	ath79_register_eth(1);
	ath79_register_eth(0);

	ath79_register_wmac(art + AP121F_CALDATA_OFFSET, NULL);
	ath79_register_usb();
}

MIPS_MACHINE(ATH79_MACH_ALFA_AP121F, "ALFA-AP121F", "ALFA AP121F", ap121f_setup);
