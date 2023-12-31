/*
 * Copyright (C) 2019 GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "pmsis.h"

#include "bsp/bsp.h"
#include "pmsis/drivers/gpio.h"
#include "pmsis/drivers/pad.h"
#include "bsp/gapuino.h"
#include "bsp/display/ili9341.h"
#include "bsp/flash/hyperflash.h"
#include "bsp/ram/hyperram.h"
#include "bsp/ram/spiram.h"
#include "bsp/transport/nina_w10.h"

#include "bsp/camera/himax.h"
#include "bsp/camera/hm0360.h"
#include "bsp/camera/ov7670.h"
#include "bsp/camera/gc0308.h"
#include "bsp/camera/ov5640.h"
#include "bsp/camera/pixart.h"


static int __bsp_init_pads_done = 0;

static void __bsp_init_pads()
{
    if (!__bsp_init_pads_done)
    {
        __bsp_init_pads_done = 1;
        uint32_t pads_value[] = {0x00055500, 0x0f000000, 0x003fffff, 0x00000000};
        pi_pad_init(pads_value);
    }
}



void bsp_hyperram_conf_init(struct pi_hyperram_conf *conf)
{
    conf->ram_start = CONFIG_HYPERRAM_START;
    conf->ram_size = CONFIG_HYPERRAM_SIZE;
    conf->skip_pads_config = 0;
    conf->hyper_itf = CONFIG_HYPERRAM_HYPER_ITF;
    conf->hyper_cs = CONFIG_HYPERRAM_HYPER_CS;
}

int bsp_hyperram_open(struct pi_hyperram_conf *conf)
{
    //__bsp_init_pads();
    return 0;
}

void bsp_hyperflash_conf_init(struct pi_hyperflash_conf *conf)
{
    conf->hyper_itf = CONFIG_HYPERFLASH_HYPER_ITF;
    conf->hyper_cs = CONFIG_HYPERFLASH_HYPER_CS;
}

int bsp_hyperflash_open(struct pi_hyperflash_conf *conf)
{
    //__bsp_init_pads();
    return 0;
}


void bsp_spiflash_conf_init(struct pi_spiflash_conf *conf)
{
    conf->size = CONFIG_SPIFLASH_SIZE;
    // sector size is in number of KB
    conf->sector_size = CONFIG_SPIFLASH_SECTOR_SIZE;
    conf->spi_itf = CONFIG_SPIFLASH_SPI_ITF;
    conf->spi_cs = CONFIG_SPIFLASH_SPI_CS;
}

int bsp_spiflash_open(struct pi_spiflash_conf *conf)
{
    return 0;
}

void bsp_spiram_conf_init(struct pi_spiram_conf *conf)
{
    conf->ram_start = CONFIG_SPIRAM_START;
    conf->ram_size = CONFIG_SPIRAM_SIZE;
    conf->skip_pads_config = 0;
    conf->spi_itf = CONFIG_SPIRAM_SPI_ITF;
    conf->spi_cs = CONFIG_SPIRAM_SPI_CS;
}

int bsp_spiram_open(struct pi_spiram_conf *conf)
{
    //__bsp_init_pads();
    return 0;
}


void bsp_himax_conf_init(struct pi_himax_conf *conf)
{
    //__bsp_init_pads();
    conf->i2c_itf = CONFIG_HIMAX_I2C_ITF;
    conf->cpi_itf = CONFIG_HIMAX_CPI_ITF;
}

int bsp_himax_open(struct pi_himax_conf *conf)
{
    //__bsp_init_pads();
    return 0;
}

void bsp_hm0360_conf_init(struct pi_hm0360_conf *conf)
{
    __bsp_init_pads();
    conf->i2c_itf = CONFIG_HM0360_I2C_ITF;
    conf->cpi_itf = CONFIG_HM0360_CPI_ITF;
}

int bsp_hm0360_open(struct pi_hm0360_conf *conf)
{
    __bsp_init_pads();
    if (!conf->skip_pads_config)
    {
        printf("Initialize pad function\n");
        pi_pad_set_function(PI_PAD_31_B11_TIMER0_CH0, PI_PAD_31_B11_GPIO_A17_FUNC1);
    }
    return 0;
}

void bsp_ov7670_conf_init(struct pi_ov7670_conf *conf)
{
    __bsp_init_pads();
    conf->i2c_itf = CONFIG_OV7670_I2C_ITF;
    conf->cpi_itf = CONFIG_OV7670_CPI_ITF;
}

int bsp_ov7670_open(struct pi_ov7670_conf *conf)
{
    __bsp_init_pads();
    return 0;
}

void bsp_gc0308_conf_init(struct pi_gc0308_conf *conf)
{
    //__bsp_init_pads();
    conf->skip_pads_config = 0;
    conf->i2c_itf = CONFIG_GC0308_I2C_ITF;
    conf->cpi_itf = CONFIG_GC0308_CPI_ITF;
    //conf->reset_gpio = PI_GPIO_A18_PAD_32_A13; //This is not connected on gapuino
    conf->pwdn_gpio  = PI_GPIO_A17_PAD_31_B11;
}

int bsp_gc0308_open(struct pi_gc0308_conf *conf)
{
    //__bsp_init_pads();
    if (!conf->skip_pads_config)
    {   
           //Set camera PAD CPI + I2C
        pi_pad_set_function(PI_PAD_18_A43_CAM_PCLK ,PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_20_B39_CAM_DATA0,PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_21_A42_CAM_DATA1,PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_22_B38_CAM_DATA2,PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_23_A41_CAM_DATA3,PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_24_B37_CAM_DATA4,PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_25_A40_CAM_DATA5,PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_26_B36_CAM_DATA6,PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_27_A38_CAM_DATA7,PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_28_A36_CAM_VSYNC,PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_19_A37_CAM_HSYNC,PI_PAD_FUNC0);

        pi_pad_set_function(PI_PAD_30_D1_CAM_SCL_FUNC0 ,PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_29_B34_CAM_SDA_FUNC0 ,PI_PAD_FUNC0);

        pi_pad_set_function(PI_PAD_31_B11_TIMER0_CH0, PI_PAD_31_B11_GPIO_A17_FUNC1);        
        pi_gpio_pin_configure(0,conf->pwdn_gpio, PI_GPIO_OUTPUT | PI_GPIO_PULL_DISABLE);

    }
    return 0;
}

void bsp_ov5640_conf_init(struct pi_ov5640_conf *conf)
{
    __bsp_init_pads();
    conf->i2c_itf = CONFIG_OV5640_I2C_ID;
    conf->cpi_itf = CONFIG_OV5640_CPI_ID;
}

int bsp_ov5640_open(struct pi_ov5640_conf *conf)
{
    __bsp_init_pads();
    return 0;
}

void bsp_ili9341_conf_init(struct pi_ili9341_conf *conf)
{
    conf->gpio = CONFIG_ILI9341_GPIO;
    conf->spi_itf = CONFIG_ILI9341_SPI_ITF;
    conf->spi_cs = CONFIG_ILI9341_SPI_CS;

}

void bsp_pixart_conf_init(struct pi_pixart_conf *conf)
{
    conf->cpi_itf = CONFIG_PIXART_CPI_ITF;
    conf->spi_itf = CONFIG_PIXART_SPI_ITF;
    conf->spi_cs = CONFIG_PIXART_SPI_CS;
    conf->pwm_id = CONFIG_PIXART_PWM_ID;
    conf->pwm_channel = CONFIG_PIXART_PWM_CH;
    conf->gpio_ctl.gpio_power_4V = CONFIG_PIXART_GPIO_PWR_4V;
    conf->gpio_ctl.gpio_power_2V5 = CONFIG_PIXART_GPIO_PWR_2V5;
    conf->gpio_ctl.gpio_reset = CONFIG_PIXART_GPIO_RESET;
}

int bsp_pixart_open(struct pi_pixart_conf *conf)
{
    __bsp_init_pads();
    return 0;
}

int bsp_ili9341_open(struct pi_ili9341_conf *conf)
{
    __bsp_init_pads();

    if (!conf->skip_pads_config)
    {
        pi_pad_set_function(CONFIG_ILI9341_GPIO_PAD, CONFIG_ILI9341_GPIO_PAD_FUNC);
    }

    return 0;
}


void pi_bsp_init_profile(int profile)
{
    __bsp_init_pads();

    if (profile == PI_BSP_PROFILE_DEFAULT)
    {
        /* Special for I2S1, use alternative pad for SDI signal. */
        pi_pad_set_function(PI_PAD_35_B13_I2S1_SCK, PI_PAD_35_B13_I2S1_SDI_FUNC3);
        pi_pad_set_function(PI_PAD_37_B14_I2S1_SDI, PI_PAD_37_B14_HYPER_CK_FUNC3);

#ifndef __ZEPHYR__
        pi_i2s_setup(PI_I2S_SETUP_SINGLE_CLOCK);
#endif
    }
}



void bsp_nina_w10_conf_init(struct pi_nina_w10_conf *conf)
{
    conf->spi_itf = CONFIG_NINA_W10_SPI_ITF;
    conf->spi_cs = CONFIG_NINA_W10_SPI_CS;
}

int bsp_nina_w10_open(struct pi_nina_w10_conf *conf)
{
    __bsp_init_pads();
    return 0;
}



void pi_bsp_init()
{
    pi_bsp_init_profile(PI_BSP_PROFILE_DEFAULT);
}
