#ifndef EEZ_LVGL_UI_STYLES_H
#define EEZ_LVGL_UI_STYLES_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Style: button_default
lv_style_t *get_style_button_default_MAIN_DEFAULT();
lv_style_t *get_style_button_default_MAIN_CHECKED();
lv_style_t *get_style_button_default_MAIN_PRESSED();
lv_style_t *get_style_button_default_MAIN_CHECKED_PRESSED();
lv_style_t *get_style_button_default_MAIN_DISABLED();
lv_style_t *get_style_button_default_MAIN_FOCUSED();
lv_style_t *get_style_button_default_MAIN_FOCUS_KEY();
lv_style_t *get_style_button_default_MAIN_EDITED();
lv_style_t *get_style_button_default_MAIN_SCROLLED();
void add_style_button_default(lv_obj_t *obj);
void remove_style_button_default(lv_obj_t *obj);

// Style: screen_main
lv_style_t *get_style_screen_main_MAIN_DEFAULT();
void add_style_screen_main(lv_obj_t *obj);
void remove_style_screen_main(lv_obj_t *obj);

// Style: bottom_panel
lv_style_t *get_style_bottom_panel_MAIN_DEFAULT();
void add_style_bottom_panel(lv_obj_t *obj);
void remove_style_bottom_panel(lv_obj_t *obj);

// Style: text_area
lv_style_t *get_style_text_area_MAIN_DEFAULT();
lv_style_t *get_style_text_area_MAIN_FOCUSED();
void add_style_text_area(lv_obj_t *obj);
void remove_style_text_area(lv_obj_t *obj);

// Style: list
lv_style_t *get_style_list_MAIN_DEFAULT();
void add_style_list(lv_obj_t *obj);
void remove_style_list(lv_obj_t *obj);



#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_STYLES_H*/