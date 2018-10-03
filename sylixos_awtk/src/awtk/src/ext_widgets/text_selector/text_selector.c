﻿/**
 * File:   text_selector.h
 * Author: AWTK Develop Team
 * Brief:  text_selector
 *
 * Copyright (c) 2018 - 2018  Guangzhou ZHIYUAN Electronics Co.,Ltd.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * License file for more details.
 *
 */

/**
 * History:
 * ================================================================
 * 2018-09-25 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "base/mem.h"
#include "base/utf8.h"
#include "base/utils.h"
#include "base/button.h"
#include "base/layout.h"
#include "base/popup.h"
#include "base/window.h"
#include "base/tokenizer.h"
#include "text_selector/text_selector.h"
#include "widget_animators/widget_animator_scroll.h"

#define ANIMATING_TIME 500

const char* s_text_selector_properties[] = {
    WIDGET_PROP_TEXT,           WIDGET_PROP_VALUE,
    WIDGET_PROP_OPTIONS,        TEXT_SELECTOR_PROP_VISIBLE_NR,
    WIDGET_PROP_SELECTED_INDEX, NULL};

static ret_t text_selector_paint_self(widget_t* widget, canvas_t* c) {
  text_selector_option_t* iter = NULL;
  text_selector_t* text_selector = TEXT_SELECTOR(widget);

  uint32_t y = 0;
  uint32_t i = 0;
  style_t* style = &(widget->style);
  color_t trans = color_init(0, 0, 0, 0);
  int32_t yoffset = text_selector->yoffset;
  int32_t item_height = widget->h / text_selector->visible_nr;
  color_t fc = style_get_color(style, STYLE_ID_FG_COLOR, trans);
  rect_t r = rect_init(0, 0, widget->w, item_height);

  widget_prepare_text_style(widget, c);
  iter = text_selector->option_items;

  if (fc.rgba.a) {
    canvas_set_stroke_color(c, fc);
    for (i = 1; i < text_selector->visible_nr; i++) {
      y = i * item_height;
      canvas_draw_hline(c, 0, y, widget->w);
    }
    y = 0;
    i = 0;
  }

  while (iter != NULL) {
    r.y = y - yoffset;
    if ((r.y + item_height) >= 0 && r.y < widget->h) {
      canvas_draw_text_in_rect(c, iter->text, wcslen(iter->text), &r);
    }

    i++;
    iter = iter->next;
    y += item_height;
  }

  return RET_OK;
}

static ret_t text_selector_on_paint_self(widget_t* widget, canvas_t* c) {
  rect_t r_save;
  rect_t r = rect_init(c->ox, c->oy, widget->w, widget->h);

  canvas_get_clip_rect(c, &r_save);
  r = rect_intersect(&r_save, &r);
  if (r.w > 0 && r.h > 0) {
    canvas_set_clip_rect(c, &r);
    text_selector_paint_self(widget, c);
    canvas_set_clip_rect(c, &r_save);
  }

  return RET_OK;
}

static ret_t text_selector_destroy(widget_t* widget) {
  text_selector_t* text_selector = TEXT_SELECTOR(widget);

  str_reset(&(text_selector->text));
  text_selector_reset_options(widget);

  return RET_OK;
}

ret_t text_selector_parse_options(widget_t* widget, const char* str) {
  int32_t i = 0;
  tokenizer_t tokenizer;
  tokenizer_t* t = &tokenizer;
  text_selector_t* text_selector = TEXT_SELECTOR(widget);

  text_selector_reset_options(widget);
  text_selector->options = tk_strdup(str);
  tokenizer_init(t, str, strlen(str), ";");

  while (tokenizer_has_more(t)) {
    int32_t value = 0;
    const char* token = tokenizer_next(t);
    if (*token) {
      const char* text = strchr(token, ':');
      if (text != NULL) {
        text++;
        value = tk_atoi(token);
      } else {
        text = token;
        value = i;
      }

      text_selector_append_option(widget, value, text);
      i++;
    }
  }
  tokenizer_deinit(t);

  return RET_OK;
}

ret_t text_selector_set_options(widget_t* widget, const char* options) {
  return_value_if_fail(widget != NULL && options != NULL, RET_BAD_PARAMS);

  if (strchr(options, ':') == NULL && strchr(options, '-') != NULL) {
    int start = 0;
    int end = 0;
    tk_sscanf(options, "%d-%d", &start, &end);
    return text_selector_set_range_options(widget, start, end - start + 1, 1);
  } else {
    return text_selector_parse_options(widget, options);
  }
}

ret_t text_selector_set_range_options(widget_t* widget, int32_t start, uint32_t nr, int32_t step) {
  char text[32];
  uint32_t i = 0;
  return_value_if_fail(widget != NULL && nr < 300, RET_BAD_PARAMS);

  for (i = 0; i < nr; i++) {
    int32_t value = start + i * step;
    tk_itoa(text, sizeof(text), value);
    text_selector_append_option(widget, value, text);
  }

  return RET_OK;
}

static ret_t text_selector_get_prop(widget_t* widget, const char* name, value_t* v) {
  text_selector_t* text_selector = TEXT_SELECTOR(widget);

  if (tk_str_eq(name, WIDGET_PROP_TEXT)) {
    value_set_str(v, text_selector_get_text(widget));
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_VALUE)) {
    value_set_int(v, text_selector_get_value(widget));
    return RET_OK;
  } else if (tk_str_eq(name, TEXT_SELECTOR_PROP_VISIBLE_NR)) {
    value_set_int(v, text_selector->visible_nr);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_SELECTED_INDEX)) {
    value_set_int(v, text_selector->selected_index);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_XOFFSET)) {
    value_set_int(v, 0);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_YOFFSET)) {
    value_set_int(v, text_selector->yoffset);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_OPTIONS)) {
    value_set_str(v, text_selector->options);
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t text_selector_set_prop(widget_t* widget, const char* name, const value_t* v) {
  text_selector_t* text_selector = TEXT_SELECTOR(widget);

  if (tk_str_eq(name, WIDGET_PROP_VALUE)) {
    text_selector_set_value(widget, value_int(v));
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_TEXT)) {
    text_selector_set_text(widget, value_str(v));
    return RET_OK;
  } else if (tk_str_eq(name, TEXT_SELECTOR_PROP_VISIBLE_NR)) {
    text_selector_set_visible_nr(widget, value_int(v));
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_SELECTED_INDEX)) {
    text_selector_set_selected_index(widget, value_int(v));
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_YOFFSET)) {
    text_selector->yoffset = value_int(v);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_XOFFSET)) {
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_OPTIONS)) {
    text_selector_set_options(widget, value_str(v));
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t text_selector_on_pointer_down(text_selector_t* text_selector, pointer_event_t* e) {
  velocity_t* v = &(text_selector->velocity);

  velocity_reset(v);
  text_selector->ydown = e->y;
  text_selector->yoffset_save = text_selector->yoffset;

  velocity_update(v, e->e.time, e->x, e->y);

  return RET_OK;
}
static ret_t text_selector_on_pointer_move(text_selector_t* text_selector, pointer_event_t* e) {
  velocity_t* v = &(text_selector->velocity);
  int32_t dy = e->y - text_selector->ydown;
  velocity_update(v, e->e.time, e->x, e->y);

  if (text_selector->wa == NULL && dy) {
    text_selector->yoffset = text_selector->yoffset_save - dy;
  }

  return RET_OK;
}

static ret_t text_selector_sync_selected_index_with_yoffset(text_selector_t* text_selector) {
  widget_t* widget = WIDGET(text_selector);
  int32_t mid_index = text_selector->visible_nr / 2;
  int32_t item_height = widget->h / text_selector->visible_nr;

  text_selector->selected_index = text_selector->yoffset / item_height + mid_index;

  widget_invalidate(widget, NULL);

  return RET_OK;
}

static ret_t text_selector_sync_yoffset_with_selected_index(text_selector_t* text_selector) {
  widget_t* widget = WIDGET(text_selector);
  int32_t mid_index = text_selector->visible_nr / 2;
  int32_t item_height = widget->h / text_selector->visible_nr;

  text_selector->yoffset = (text_selector->selected_index - mid_index) * item_height;

  widget_invalidate(widget, NULL);

  return RET_OK;
}

static ret_t text_selector_on_scroll_done(void* ctx, event_t* e) {
  widget_t* widget = WIDGET(ctx);
  text_selector_t* text_selector = TEXT_SELECTOR(ctx);
  int32_t selected_index = text_selector->selected_index;

  text_selector->wa = NULL;
  text_selector_sync_selected_index_with_yoffset(text_selector);

  if (selected_index != text_selector->selected_index) {
    event_t e = event_init(EVT_VALUE_CHANGED, widget);
    widget_dispatch(widget, &e);
    log_debug("selected_index: %d => %d %s\n", selected_index, text_selector->selected_index,
              text_selector_get_text(widget));
  }

  return RET_REMOVE;
}

static ret_t text_selector_scroll_to(widget_t* widget, int32_t yoffset_end) {
  int32_t yoffset = 0;
  text_selector_t* text_selector = TEXT_SELECTOR(widget);
  return_value_if_fail(widget != NULL, RET_FAIL);

  yoffset = text_selector->yoffset;
  if (yoffset == yoffset_end) {
    return RET_OK;
  }

  text_selector->wa = widget_animator_scroll_create(widget, ANIMATING_TIME, 0, EASING_SIN_INOUT);
  return_value_if_fail(text_selector->wa != NULL, RET_OOM);

  widget_animator_scroll_set_params(text_selector->wa, 0, yoffset, 0, yoffset_end);
  widget_animator_on(text_selector->wa, EVT_ANIM_END, text_selector_on_scroll_done, text_selector);
  widget_animator_start(text_selector->wa);

  return RET_OK;
}

static ret_t text_selector_on_pointer_up(text_selector_t* text_selector, pointer_event_t* e) {
  int32_t yoffset_end = 0;
  widget_t* widget = WIDGET(text_selector);
  velocity_t* v = &(text_selector->velocity);
  int32_t options_nr = text_selector_count_options(widget);
  int32_t item_height = widget->h / text_selector->visible_nr;
  int32_t empty_item_height = (text_selector->visible_nr / 2) * item_height;
  int32_t min_yoffset = -empty_item_height;
  int32_t max_yoffset = (options_nr * item_height + empty_item_height) - widget->h;

  velocity_update(v, e->e.time, e->x, e->y);
  yoffset_end = text_selector->yoffset - v->yv;

  if (e->y == text_selector->ydown) {
    /*click*/
    int32_t index = 0;
    int32_t mid_index = text_selector->visible_nr / 2;
    point_t p = {e->x, e->y};
    widget_to_local(widget, &p);
    index = p.y / item_height;
    if (index == mid_index) {
      return RET_OK;
    } else {
      yoffset_end = text_selector->yoffset + item_height * (index - mid_index);
    }
  }

  if (yoffset_end < min_yoffset) {
    yoffset_end = min_yoffset;
  }

  if (yoffset_end > (max_yoffset)) {
    yoffset_end = max_yoffset;
  }

  yoffset_end = round((float)yoffset_end / (float)item_height) * item_height;
  text_selector_scroll_to(widget, yoffset_end);

  return RET_OK;
}

static ret_t text_selector_on_event(widget_t* widget, event_t* e) {
  uint16_t type = e->type;
  text_selector_t* text_selector = TEXT_SELECTOR(widget);

  switch (type) {
    case EVT_POINTER_DOWN:
      widget_grab(widget->parent, widget);
      text_selector_on_pointer_down(text_selector, (pointer_event_t*)e);
      break;
    case EVT_POINTER_UP: {
      text_selector_on_pointer_up(text_selector, (pointer_event_t*)e);
      widget_ungrab(widget->parent, widget);
      break;
    }
    case EVT_POINTER_MOVE: {
      pointer_event_t* evt = (pointer_event_t*)e;
      if (evt->pressed) {
        text_selector_on_pointer_move(text_selector, evt);
        widget_invalidate(widget, NULL);
      }
      break;
    }
    default:
      break;
  }

  return RET_OK;
}

static const widget_vtable_t s_text_selector_vtable = {
    .size = sizeof(text_selector_t),
    .type = WIDGET_TYPE_TEXT_SELECTOR,
    .clone_properties = s_text_selector_properties,
    .persistent_properties = s_text_selector_properties,
    .create = text_selector_create,
    .on_paint_self = text_selector_on_paint_self,
    .set_prop = text_selector_set_prop,
    .get_prop = text_selector_get_prop,
    .destroy = text_selector_destroy,
    .on_event = text_selector_on_event};

widget_t* text_selector_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  text_selector_t* text_selector = TKMEM_ZALLOC(text_selector_t);
  widget_t* widget = WIDGET(text_selector);
  return_value_if_fail(text_selector != NULL, NULL);

  text_selector->visible_nr = 5;

  return widget_init(widget, parent, &s_text_selector_vtable, x, y, w, h);
}

ret_t text_selector_reset_options(widget_t* widget) {
  text_selector_option_t* iter = NULL;
  text_selector_option_t* next = NULL;
  text_selector_t* text_selector = TEXT_SELECTOR(widget);
  return_value_if_fail(text_selector != NULL, RET_BAD_PARAMS);

  iter = text_selector->option_items;
  while (iter != NULL) {
    next = iter->next;
    TKMEM_FREE(iter);
    iter = next;
  }
  text_selector->option_items = NULL;
  TKMEM_FREE(text_selector->options);

  return RET_OK;
}

ret_t text_selector_append_option(widget_t* widget, int32_t value, const char* text) {
  int32_t size = 0;
  text_selector_option_t* iter = NULL;
  text_selector_option_t* option = NULL;
  text_selector_t* text_selector = TEXT_SELECTOR(widget);

  return_value_if_fail(text_selector != NULL && text != NULL, RET_BAD_PARAMS);
  size = sizeof(text_selector_option_t) + (strlen(text) + 1) * sizeof(wchar_t);

  option = (text_selector_option_t*)TKMEM_ALLOC(size);
  return_value_if_fail(option != NULL, RET_OOM);

  memset(option, 0x00, size);

  option->value = value;
  utf8_to_utf16(text, option->text, strlen(text) + 1);

  if (text_selector->option_items != NULL) {
    iter = text_selector->option_items;
    while (iter->next != NULL) iter = iter->next;
    iter->next = option;
  } else {
    text_selector->option_items = option;
  }

  return RET_OK;
}

int32_t text_selector_count_options(widget_t* widget) {
  int32_t nr = 0;
  text_selector_option_t* iter = NULL;
  text_selector_t* text_selector = TEXT_SELECTOR(widget);
  return_value_if_fail(text_selector != NULL, nr);

  iter = text_selector->option_items;
  while (iter != NULL) {
    nr++;
    iter = iter->next;
  }

  return nr;
}

int32_t text_selector_get_option_by_value(widget_t* widget, int32_t value) {
  uint32_t i = 0;
  text_selector_option_t* iter = NULL;
  text_selector_t* text_selector = TEXT_SELECTOR(widget);
  return_value_if_fail(text_selector != NULL, -1);

  iter = text_selector->option_items;
  while (iter != NULL) {
    if (value == iter->value) {
      return i;
    }

    i++;
    iter = iter->next;
  }

  return -1;
}

int32_t text_selector_get_option_by_text(widget_t* widget, const char* text) {
  wstr_t str;
  uint32_t i = 0;
  text_selector_option_t* iter = NULL;
  text_selector_t* text_selector = TEXT_SELECTOR(widget);
  return_value_if_fail(text_selector != NULL, -1);

  wstr_init(&str, 0);
  wstr_set_utf8(&str, text);
  iter = text_selector->option_items;
  while (iter != NULL) {
    if (wcscmp(str.str, iter->text) == 0) {
      wstr_reset(&str);
      return i;
    }

    i++;
    iter = iter->next;
  }
  wstr_reset(&str);

  return -1;
}

text_selector_option_t* text_selector_get_option(widget_t* widget, uint32_t index) {
  uint32_t i = 0;
  text_selector_option_t* iter = NULL;
  text_selector_t* text_selector = TEXT_SELECTOR(widget);
  return_value_if_fail(text_selector != NULL, NULL);

  iter = text_selector->option_items;
  while (iter != NULL) {
    if (i == index) {
      return iter;
    }

    i++;
    iter = iter->next;
  }

  return NULL;
}

ret_t text_selector_set_selected_index(widget_t* widget, uint32_t index) {
  text_selector_t* text_selector = TEXT_SELECTOR(widget);
  text_selector_option_t* option = text_selector_get_option(widget, index);
  return_value_if_fail(text_selector != NULL, RET_BAD_PARAMS);
  return_value_if_fail(option != NULL, RET_BAD_PARAMS);

  text_selector->selected_index = index;
  text_selector_sync_yoffset_with_selected_index(text_selector);

  return RET_OK;
}

ret_t text_selector_set_visible_nr(widget_t* widget, uint32_t visible_nr) {
  text_selector_t* text_selector = TEXT_SELECTOR(widget);
  return_value_if_fail(text_selector != NULL, RET_BAD_PARAMS);

  text_selector->visible_nr = visible_nr == 3 ? 3 : 5;
  text_selector_sync_yoffset_with_selected_index(text_selector);

  return RET_OK;
}

int32_t text_selector_get_value(widget_t* widget) {
  text_selector_option_t* option = NULL;
  text_selector_t* text_selector = TEXT_SELECTOR(widget);
  return_value_if_fail(text_selector != NULL, -1);

  option = text_selector_get_option(widget, text_selector->selected_index);

  return option != NULL ? option->value : -1;
}

ret_t text_selector_set_value(widget_t* widget, int32_t value) {
  int32_t index = text_selector_get_option_by_value(widget, value);
  text_selector_t* text_selector = TEXT_SELECTOR(widget);
  return_value_if_fail(text_selector != NULL && index >= 0, RET_BAD_PARAMS);

  return text_selector_set_selected_index(widget, index);
}

const char* text_selector_get_text(widget_t* widget) {
  text_selector_option_t* option = NULL;
  text_selector_t* text_selector = TEXT_SELECTOR(widget);
  return_value_if_fail(text_selector != NULL, NULL);

  option = text_selector_get_option(widget, text_selector->selected_index);

  if (option != NULL) {
    str_from_wstr(&(text_selector->text), option->text);

    return text_selector->text.str;
  }

  return NULL;
}

ret_t text_selector_set_text(widget_t* widget, const char* text) {
  int32_t index = text_selector_get_option_by_text(widget, text);
  text_selector_t* text_selector = TEXT_SELECTOR(widget);
  return_value_if_fail(text_selector != NULL && index >= 0, RET_BAD_PARAMS);

  return text_selector_set_selected_index(widget, index);
}

widget_t* text_selector_cast(widget_t* widget) {
  return_value_if_fail(widget != NULL && widget->vt == &s_text_selector_vtable, NULL);

  return widget;
}
