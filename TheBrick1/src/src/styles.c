#include "styles.h"
#include "images.h"
#include "fonts.h"

#include "ui.h"
#include "screens.h"

//
// Style: button_default
//

void init_style_button_default_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_radius(style, 2);
    lv_style_set_bg_color(style, lv_color_hex(0xffe4572e));
    lv_style_set_bg_grad_dir(style, LV_GRAD_DIR_NONE);
};

lv_style_t *get_style_button_default_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_default_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_button_default_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_radius(style, 2);
};

lv_style_t *get_style_button_default_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_default_MAIN_CHECKED(style);
    }
    return style;
};

void init_style_button_default_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_radius(style, 2);
};

lv_style_t *get_style_button_default_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_default_MAIN_PRESSED(style);
    }
    return style;
};

void init_style_button_default_MAIN_CHECKED_PRESSED(lv_style_t *style) {
    lv_style_set_radius(style, 2);
};

lv_style_t *get_style_button_default_MAIN_CHECKED_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_default_MAIN_CHECKED_PRESSED(style);
    }
    return style;
};

void init_style_button_default_MAIN_DISABLED(lv_style_t *style) {
    lv_style_set_radius(style, 2);
};

lv_style_t *get_style_button_default_MAIN_DISABLED() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_default_MAIN_DISABLED(style);
    }
    return style;
};

void init_style_button_default_MAIN_FOCUSED(lv_style_t *style) {
    lv_style_set_radius(style, 2);
};

lv_style_t *get_style_button_default_MAIN_FOCUSED() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_default_MAIN_FOCUSED(style);
    }
    return style;
};

void init_style_button_default_MAIN_FOCUS_KEY(lv_style_t *style) {
    lv_style_set_radius(style, 2);
};

lv_style_t *get_style_button_default_MAIN_FOCUS_KEY() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_default_MAIN_FOCUS_KEY(style);
    }
    return style;
};

void init_style_button_default_MAIN_EDITED(lv_style_t *style) {
    lv_style_set_radius(style, 2);
};

lv_style_t *get_style_button_default_MAIN_EDITED() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_default_MAIN_EDITED(style);
    }
    return style;
};

void init_style_button_default_MAIN_SCROLLED(lv_style_t *style) {
    lv_style_set_radius(style, 2);
};

lv_style_t *get_style_button_default_MAIN_SCROLLED() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_default_MAIN_SCROLLED(style);
    }
    return style;
};

void add_style_button_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_button_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_button_default_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_add_style(obj, get_style_button_default_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_style(obj, get_style_button_default_MAIN_CHECKED_PRESSED(), LV_PART_MAIN | LV_STATE_CHECKED | LV_STATE_PRESSED);
    lv_obj_add_style(obj, get_style_button_default_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
    lv_obj_add_style(obj, get_style_button_default_MAIN_FOCUSED(), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_add_style(obj, get_style_button_default_MAIN_FOCUS_KEY(), LV_PART_MAIN | LV_STATE_FOCUS_KEY);
    lv_obj_add_style(obj, get_style_button_default_MAIN_EDITED(), LV_PART_MAIN | LV_STATE_EDITED);
    lv_obj_add_style(obj, get_style_button_default_MAIN_SCROLLED(), LV_PART_MAIN | LV_STATE_SCROLLED);
};

void remove_style_button_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_button_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_button_default_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_remove_style(obj, get_style_button_default_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_remove_style(obj, get_style_button_default_MAIN_CHECKED_PRESSED(), LV_PART_MAIN | LV_STATE_CHECKED | LV_STATE_PRESSED);
    lv_obj_remove_style(obj, get_style_button_default_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
    lv_obj_remove_style(obj, get_style_button_default_MAIN_FOCUSED(), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_remove_style(obj, get_style_button_default_MAIN_FOCUS_KEY(), LV_PART_MAIN | LV_STATE_FOCUS_KEY);
    lv_obj_remove_style(obj, get_style_button_default_MAIN_EDITED(), LV_PART_MAIN | LV_STATE_EDITED);
    lv_obj_remove_style(obj, get_style_button_default_MAIN_SCROLLED(), LV_PART_MAIN | LV_STATE_SCROLLED);
};

//
// Style: screen_main
//

void init_style_screen_main_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff000000));
};

lv_style_t *get_style_screen_main_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_screen_main_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_screen_main(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_screen_main_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_screen_main(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_screen_main_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: bottom_panel
//

void init_style_bottom_panel_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff000000));
    lv_style_set_border_color(style, lv_color_hex(0xff000000));
};

lv_style_t *get_style_bottom_panel_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_bottom_panel_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_bottom_panel(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_bottom_panel_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_bottom_panel(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_bottom_panel_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: text_area
//

void init_style_text_area_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_border_width(style, 4);
    lv_style_set_radius(style, 2);
    lv_style_set_opa(style, 255);
};

lv_style_t *get_style_text_area_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_text_area_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_text_area_MAIN_FOCUSED(lv_style_t *style) {
    lv_style_set_border_color(style, lv_color_hex(0xffff0000));
};

lv_style_t *get_style_text_area_MAIN_FOCUSED() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_text_area_MAIN_FOCUSED(style);
    }
    return style;
};

void add_style_text_area(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_text_area_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_text_area_MAIN_FOCUSED(), LV_PART_MAIN | LV_STATE_FOCUSED);
};

void remove_style_text_area(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_text_area_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_text_area_MAIN_FOCUSED(), LV_PART_MAIN | LV_STATE_FOCUSED);
};

//
// Style: list
//

void init_style_list_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_border_width(style, 4);
    lv_style_set_border_color(style, lv_color_hex(0xffe0e0e0));
};

lv_style_t *get_style_list_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_list_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_list(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_list_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_list(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_list_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
//
//

void add_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*AddStyleFunc)(lv_obj_t *obj);
    static const AddStyleFunc add_style_funcs[] = {
        add_style_button_default,
        add_style_screen_main,
        add_style_bottom_panel,
        add_style_text_area,
        add_style_list,
    };
    add_style_funcs[styleIndex](obj);
}

void remove_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*RemoveStyleFunc)(lv_obj_t *obj);
    static const RemoveStyleFunc remove_style_funcs[] = {
        remove_style_button_default,
        remove_style_screen_main,
        remove_style_bottom_panel,
        remove_style_text_area,
        remove_style_list,
    };
    remove_style_funcs[styleIndex](obj);
}

