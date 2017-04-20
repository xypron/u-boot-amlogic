/*
 * drivers/display/lcd/lcd_extern/mipi_KD080D13.c
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the named License,
 * or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 */

#include <common.h>
#include <malloc.h>
#include <asm/arch/gpio.h>
#ifdef CONFIG_OF_LIBFDT
#include <libfdt.h>
#endif
#include <amlogic/aml_lcd.h>
#include <amlogic/aml_lcd_extern.h>
#include "lcd_extern.h"

#define LCD_EXTERN_INDEX		1
#define LCD_EXTERN_NAME			"mipi_KD080D13"
#define LCD_EXTERN_TYPE			LCD_EXTERN_MIPI

//******************** mipi command ********************//
//format:  data_type, num, data....
//special: data_type=0xff, num<0xff means delay ms, num=0xff means ending.
//******************************************************//
static unsigned char mipi_init_on_table[] = {
	0x39,3,0xf0,0x5a,0x5a,
	0x39,3,0xf1,0x5a,0x5a,
	0x39,3,0xfc,0xa5,0xa5,
	0x39,3,0xd0,0x00,0x10,
	0x15,2,0xb1,0x10,
	0x39,5,0xb2,0x14,0x22,0x2f,0x04,
	0x39,6,0xf2,0x02,0x08,0x08,0x40,0x10,
	0x15,2,0xb0,0x03,
	0x39,3,0xfd,0x23,0x09,
	0x39,11,0xf3,0x01,0x93,0x20,0x22,0x80,0x05,0x25,0x3c,0x26,0x00,
	0x39,46,0xf4,0x00,0x02,0x03,0x26,0x03,0x02,0x09,0x00,0x07,0x16,0x16,0x03,0x00,0x08,0x08,0x03,0x0E,0x0F,0x12,0x1C,0x1D,0x1E,0x0C,0x09,0x01,0x04,0x02,0x61,0x74,0x75,0x72,0x83,0x80,0x80,0xB0,0x00,0x01,0x01,0x28,0x04,0x03,0x28,0x01,0xD1,0x32,
	0x39,27,0xf5,0x84,0x2F,0x2F,0x5F,0xAB,0x98,0x52,0x0F,0x33,0x43,0x04,0x59,0x54,0x52,0x05,0x40,0x60,0x4E,0x60,0x40,0x27,0x26,0x52,0x25,0x6D,0x18,
	0x39,9,0xee,0x25,0x00,0x25,0x00,0x25,0x00,0x25,0x00,
	0x39,9,0xef,0x34,0x12,0x98,0xBA,0x20,0x00,0x24,0x80,
	0x39,33,0xf7,0x0E,0x0E,0x0A,0x0A,0x0F,0x0F,0x0B,0x0B,0x05,0x07,0x01,0x01,0x01,0x01,0x01,0x01,0x0C,0x0C,0x08,0x08,0x0D,0x0D,0x09,0x09,0x04,0x06,0x01,0x01,0x01,0x01,0x01,0x01,
	0x39,4,0xbc,0x01,0x4e,0x0a,
	0x39,6,0xe1,0x03,0x10,0x1c,0xa0,0x10,
	0x39,7,0xf6,0x60,0x21,0xA6,0x00,0x00,0x00,
	0x39,7,0xfe,0x00,0x0D,0x03,0x21,0x80,0x48,
	0x15,2,0xb0,0x22,
	0x39,18,0xfa,0x02,0x34,0x09,0x13,0x0B,0x0F,0x16,0x16,0x17,0x1E,0x1D,0x1C,0x1E,0x1D,0x1D,0x1F,0x24,
	0x15,2,0xb0,0x22,
	0x39,18,0xfb,0x00,0x34,0x07,0x11,0x09,0x0D,0x14,0x14,0x15,0x1C,0x1F,0x1C,0x1D,0x1D,0x1D,0x20,0x26,
	0x15,2,0xb0,0x11,
	0x39,18,0xfa,0x20,0x34,0x24,0x27,0x19,0x1B,0x1F,0x1E,0x1B,0x1F,0x21,0x1F,0x1E,0x20,0x1E,0x1E,0x21,
	0x15,2,0xb0,0x11,
	0x39,18,0xfb,0x1E,0x34,0x22,0x25,0x17,0x19,0x1D,0x1A,0x19,0x20,0x1F,0x1E,0x20,0x1E,0x1E,0x1F,0x22,
	0x39,18,0xfa,0x1C,0x34,0x1C,0x1F,0x13,0x17,0x1A,0x18,0x18,0x1E,0x20,0x21,0x21,0x21,0x23,0x22,0x2A,
	0x39,18,0xfb,0x1A,0x34,0x1A,0x1D,0x11,0x15,0x18,0x16,0x16,0x1C,0x20,0x20,0x20,0x1F,0x23,0x23,0x2B,

	0x05,1,0x11,
	0xff,20,
	0x39,4,0xc3,0x40,0x00,0x28,
	0xff,200,
	0x15,2,0x35,0x00,
	0x05,1,0x29,
	0xff,30,     //delay 30ms
	0xff,0xff,   //ending flag
};

static unsigned char mipi_init_off_table[] = {
	0x05,1,0x28, //display off
	0xff,30,     //delay 30ms
	0x05,1,0x10, //sleep in
	0xff,30,     //delay 30ms
	0x39,4,0xc3,0x40,0x00,0x20,
	0xff,10,
	0xff,0xff,   //ending flag
};

static int lcd_extern_driver_update(struct aml_lcd_extern_driver_s *ext_drv)
{
	if (ext_drv == NULL) {
		EXTERR("%s driver is null\n", LCD_EXTERN_NAME);
		return -1;
	}

	if (ext_drv->config.type == LCD_EXTERN_MAX) { //default for no dt
		ext_drv->config.index = LCD_EXTERN_INDEX;
		ext_drv->config.type = LCD_EXTERN_TYPE;
		strcpy(ext_drv->config.name, LCD_EXTERN_NAME);
	}
	ext_drv->init_on_cmd_8  = &mipi_init_on_table[0];
	ext_drv->init_off_cmd_8 = &mipi_init_off_table[0];

	return 0;
}

int aml_lcd_extern_mipi_KD080D13_get_default_index(void)
{
	return LCD_EXTERN_INDEX;
}

int aml_lcd_extern_mipi_KD080D13_probe(struct aml_lcd_extern_driver_s *ext_drv)
{
	int ret = 0;

	ret = lcd_extern_driver_update(ext_drv);

	if (lcd_debug_print_flag)
		EXTPR("%s: %d\n", __func__, ret);
	return ret;
}
