﻿/**
 * File:   lcd_mem_rgba8888.h
 * Author: AWTK Develop Team
 * Brief:  rgba8888 mem lcd.
 *
 * Copyright (c) 2018 - 2018  Guangzhou ZHIYUAN Electronics Co.,Ltd.
 *
 * this program is distributed in the hope that it will be useful,
 * but without any warranty; without even the implied warranty of
 * merchantability or fitness for a particular purpose.  see the
 * license file for more details.
 *
 */

/**
 * history:
 * ================================================================
 * 2018-05-27 li xianjing <xianjimli@hotmail.com> created
 *
 */

#ifndef TK_LCD_MEM8888_H
#define TK_LCD_MEM8888_H

#include "lcd/lcd_mem.h"

BEGIN_C_DECLS

lcd_t* lcd_mem_rgba8888_create(wh_t w, wh_t h, bool_t alloc);
lcd_t* lcd_mem_rgba8888_create_single_fb(wh_t w, wh_t h, uint8_t* fbuff);
lcd_t* lcd_mem_rgba8888_create_double_fb(wh_t w, wh_t h, uint8_t* online_fb, uint8_t* offline_fb);
lcd_t* lcd_mem_rgba8888_create_three_fb(wh_t w, wh_t h, uint8_t* online_fb, uint8_t* offline_fb,
                                        uint8_t* next_fb);

END_C_DECLS

#endif /*TK_LCD_MEM8888_H*/
