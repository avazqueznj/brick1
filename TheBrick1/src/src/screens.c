#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;
lv_obj_t *tick_value_change_obj;
uint32_t active_theme_index = 0;

void create_screen_main() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 800, 480);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SNAPPABLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(obj, LV_DIR_VER);
    add_style_screen_main(obj);
    lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // CLOCK
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.clock = obj;
            lv_obj_set_pos(obj, 21, -211);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "1972/6/22 12:8:25");
        }
        {
            // driver_name_main
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.driver_name_main = obj;
            lv_obj_set_pos(obj, 21, -196);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "driver name");
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 431);
            lv_obj_set_size(obj, 800, 50);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            add_style_bottom_panel(obj);
        }
        {
            // logo1
            lv_obj_t *obj = lv_img_create(parent_obj);
            objects.logo1 = obj;
            lv_obj_set_pos(obj, 324, 21);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_img_set_src(obj, &img_brick_logo);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
        }
        {
            // doInspectButton
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.do_inspect_button = obj;
            lv_obj_set_pos(obj, 243, 147);
            lv_obj_set_size(obj, 314, 136);
            lv_obj_add_event_cb(obj, action_main_event_dispatcher, LV_EVENT_PRESSED, (void *)0);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE);
            add_style_button_default(obj);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_FOCUSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 18, 42);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_DEFAULT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Check / Inspect");
                }
            }
        }
        {
            // doSync
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.do_sync = obj;
            lv_obj_set_pos(obj, 591, 190);
            lv_obj_set_size(obj, 188, 50);
            lv_obj_add_event_cb(obj, action_main_event_dispatcher, LV_EVENT_PRESSED, (void *)0);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE);
            add_style_button_default(obj);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_FOCUSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Sync  \uF079");
                }
            }
        }
        {
            // logout
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.logout = obj;
            lv_obj_set_pos(obj, 591, 381);
            lv_obj_set_size(obj, 188, 50);
            lv_obj_add_event_cb(obj, action_main_event_dispatcher, LV_EVENT_PRESSED, (void *)0);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE);
            add_style_button_default(obj);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_FOCUSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "logout \uF093");
                }
            }
        }
    }
    
    tick_screen_main();
}

void tick_screen_main() {
}

void create_screen_select_asset_screen() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.select_asset_screen = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 800, 480);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SNAPPABLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
    add_style_screen_main(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 1, 431);
            lv_obj_set_size(obj, 800, 50);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE);
            add_style_bottom_panel(obj);
        }
        {
            // logo1_1
            lv_obj_t *obj = lv_img_create(parent_obj);
            objects.logo1_1 = obj;
            lv_obj_set_pos(obj, 324, 21);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_img_set_src(obj, &img_brick_logo);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
        }
        {
            // doSelectInspectionType
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.do_select_inspection_type = obj;
            lv_obj_set_pos(obj, 690, 432);
            lv_obj_set_size(obj, 94, 40);
            lv_obj_add_event_cb(obj, action_main_event_dispatcher, LV_EVENT_PRESSED, (void *)0);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE);
            add_style_button_default(obj);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_FOCUSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "\uF054");
                }
            }
        }
        {
            // backFromSelectAsset
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.back_from_select_asset = obj;
            lv_obj_set_pos(obj, 21, 432);
            lv_obj_set_size(obj, 94, 40);
            lv_obj_add_event_cb(obj, action_main_event_dispatcher, LV_EVENT_PRESSED, (void *)0);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE);
            add_style_button_default(obj);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_FOCUSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "\uF053");
                }
            }
        }
        {
            // asset_list
            lv_obj_t *obj = lv_list_create(parent_obj);
            objects.asset_list = obj;
            lv_obj_set_pos(obj, 21, 140);
            lv_obj_set_size(obj, 332, 277);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_ON);
            lv_obj_set_scroll_dir(obj, LV_DIR_VER);
            add_style_list(obj);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_pad_left(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.obj0 = obj;
                    lv_obj_set_pos(obj, 503, 42);
                    lv_obj_set_size(obj, 293, 50);
                    lv_obj_add_event_cb(obj, action_main_event_dispatcher, LV_EVENT_PRESSED, (void *)0);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_CHECKABLE);
                    add_style_button_default(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Asset123");
                        }
                    }
                }
            }
        }
        {
            // selected_asset_list
            lv_obj_t *obj = lv_list_create(parent_obj);
            objects.selected_asset_list = obj;
            lv_obj_set_pos(obj, 451, 140);
            lv_obj_set_size(obj, 333, 277);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_ON);
            lv_obj_set_scroll_dir(obj, LV_DIR_VER);
            lv_obj_set_style_pad_left(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.obj1 = obj;
                    lv_obj_set_pos(obj, 503, 42);
                    lv_obj_set_size(obj, 282, 50);
                    add_style_button_default(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "ButtonSelected");
                        }
                    }
                }
            }
        }
        {
            // deSelectAsset
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.de_select_asset = obj;
            lv_obj_set_pos(obj, 361, 310);
            lv_obj_set_size(obj, 79, 56);
            lv_obj_add_event_cb(obj, action_main_event_dispatcher, LV_EVENT_PRESSED, (void *)0);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE);
            add_style_button_default(obj);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_FOCUSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "\uF053 *");
                }
            }
        }
        {
            // selectAsset
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.select_asset = obj;
            lv_obj_set_pos(obj, 361, 192);
            lv_obj_set_size(obj, 79, 56);
            lv_obj_add_event_cb(obj, action_main_event_dispatcher, LV_EVENT_PRESSED, (void *)0);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE);
            add_style_button_default(obj);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_FOCUSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "# \uF054");
                }
            }
        }
        {
            // searchAsset
            lv_obj_t *obj = lv_textarea_create(parent_obj);
            objects.search_asset = obj;
            lv_obj_set_pos(obj, 47, 88);
            lv_obj_set_size(obj, 216, 46);
            lv_textarea_set_max_length(obj, 128);
            lv_textarea_set_one_line(obj, true);
            lv_textarea_set_password_mode(obj, false);
            lv_obj_add_event_cb(obj, action_main_event_dispatcher, LV_EVENT_VALUE_CHANGED, (void *)0);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_add_state(obj, LV_STATE_FOCUSED);
            add_style_text_area(obj);
            lv_obj_set_style_pad_left(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // searchAssetClear
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.search_asset_clear = obj;
            lv_obj_set_pos(obj, 275, 88);
            lv_obj_set_size(obj, 49, 46);
            lv_obj_add_event_cb(obj, action_main_event_dispatcher, LV_EVENT_PRESSED, (void *)0);
            add_style_button_default(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "\uF00D");
                }
            }
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj2 = obj;
            lv_obj_set_pos(obj, 243, 442);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "search or tap id tag");
        }
        {
            // clock_asset
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.clock_asset = obj;
            lv_obj_set_pos(obj, 21, -211);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "1972/6/22 12:8:25");
        }
        {
            // driver_name_asset
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.driver_name_asset = obj;
            lv_obj_set_pos(obj, 21, -196);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "driver name");
        }
    }
    
    tick_screen_select_asset_screen();
}

void tick_screen_select_asset_screen() {
}

void create_screen_select_inspection_type() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.select_inspection_type = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 800, 480);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SNAPPABLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
    add_style_screen_main(obj);
    lv_obj_set_style_border_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_FOCUSED);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 1, 431);
            lv_obj_set_size(obj, 800, 50);
            add_style_bottom_panel(obj);
        }
        {
            // logo1_3
            lv_obj_t *obj = lv_img_create(parent_obj);
            objects.logo1_3 = obj;
            lv_obj_set_pos(obj, 324, 21);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_img_set_src(obj, &img_brick_logo);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
        }
        {
            // inspectionTypes
            lv_obj_t *obj = lv_list_create(parent_obj);
            objects.inspection_types = obj;
            lv_obj_set_pos(obj, 171, 87);
            lv_obj_set_size(obj, 459, 332);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_ON);
            lv_obj_set_scroll_dir(obj, LV_DIR_VER);
            add_style_list(obj);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_FOCUSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.obj3 = obj;
                    lv_obj_set_pos(obj, 369, 42);
                    lv_obj_set_size(obj, 411, 84);
                    lv_obj_add_event_cb(obj, action_main_event_dispatcher, LV_EVENT_PRESSED, (void *)0);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_CHECKABLE);
                    add_style_button_default(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Asset123");
                        }
                    }
                }
            }
        }
        {
            // doInspectionForm
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.do_inspection_form = obj;
            lv_obj_set_pos(obj, 680, 437);
            lv_obj_set_size(obj, 94, 40);
            lv_obj_add_event_cb(obj, action_main_event_dispatcher, LV_EVENT_PRESSED, (void *)0);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE);
            add_style_button_default(obj);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_FOCUSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "\uF054");
                }
            }
        }
        {
            // backFromSelectInsp
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.back_from_select_insp = obj;
            lv_obj_set_pos(obj, 21, 437);
            lv_obj_set_size(obj, 94, 40);
            lv_obj_add_event_cb(obj, action_main_event_dispatcher, LV_EVENT_PRESSED, (void *)0);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE);
            add_style_button_default(obj);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_FOCUSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "\uF053");
                }
            }
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj4 = obj;
            lv_obj_set_pos(obj, 283, 437);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "select check list");
        }
        {
            // clock_insptype
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.clock_insptype = obj;
            lv_obj_set_pos(obj, 21, -211);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "1972/6/22 12:8:25");
        }
        {
            // driver_name_insptype
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.driver_name_insptype = obj;
            lv_obj_set_pos(obj, 21, -196);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "driver name");
        }
    }
    
    tick_screen_select_inspection_type();
}

void tick_screen_select_inspection_type() {
}

void create_screen_inspection_form() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.inspection_form = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 800, 480);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SNAPPABLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
    add_style_screen_main(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 1, 431);
            lv_obj_set_size(obj, 800, 50);
            add_style_bottom_panel(obj);
        }
        {
            // logo1_4
            lv_obj_t *obj = lv_img_create(parent_obj);
            objects.logo1_4 = obj;
            lv_obj_set_pos(obj, 324, 21);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_img_set_src(obj, &img_brick_logo);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
        }
        {
            // doZones
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.do_zones = obj;
            lv_obj_set_pos(obj, 687, 436);
            lv_obj_set_size(obj, 94, 40);
            lv_obj_add_event_cb(obj, action_main_event_dispatcher, LV_EVENT_PRESSED, (void *)0);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE);
            add_style_button_default(obj);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_FOCUSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "\uF054");
                }
            }
        }
        {
            // backFromFormFields
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.back_from_form_fields = obj;
            lv_obj_set_pos(obj, 17, 436);
            lv_obj_set_size(obj, 94, 40);
            lv_obj_add_event_cb(obj, action_main_event_dispatcher, LV_EVENT_PRESSED, (void *)0);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE);
            add_style_button_default(obj);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_FOCUSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // backFromForm
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.back_from_form = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "\uF053");
                }
            }
        }
        {
            // formFields
            lv_obj_t *obj = lv_list_create(parent_obj);
            objects.form_fields = obj;
            lv_obj_set_pos(obj, 17, 126);
            lv_obj_set_size(obj, 764, 302);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_AUTO);
            lv_obj_set_scroll_dir(obj, LV_DIR_VER);
            add_style_list(obj);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_FOCUSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 43, 140);
                    lv_obj_set_size(obj, 575, 72);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_top(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Text");
                        }
                        {
                            lv_obj_t *obj = lv_textarea_create(parent_obj);
                            objects.obj5 = obj;
                            lv_obj_set_pos(obj, 2, 7);
                            lv_obj_set_size(obj, 388, 54);
                            lv_textarea_set_max_length(obj, 128);
                            lv_textarea_set_text(obj, "123");
                            lv_textarea_set_one_line(obj, true);
                            lv_textarea_set_password_mode(obj, false);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff7095c8), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_border_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_FOCUSED);
                        }
                    }
                }
            }
        }
        {
            // InspectionTypeName
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.inspection_type_name = obj;
            lv_obj_set_pos(obj, 18, 66);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "<inspection name>");
        }
        {
            // formsInfoLabel
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.forms_info_label = obj;
            lv_obj_set_pos(obj, 18, 101);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Use tab (C,D) to select fields, * to delete");
        }
        {
            // clock_form
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.clock_form = obj;
            lv_obj_set_pos(obj, 21, -211);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "1972/6/22 12:8:25");
        }
        {
            // driver_name_form
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.driver_name_form = obj;
            lv_obj_set_pos(obj, 21, -196);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "driver name");
        }
    }
    
    tick_screen_inspection_form();
}

void tick_screen_inspection_form() {
}

void create_screen_inspection_zones() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.inspection_zones = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 800, 480);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SNAPPABLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
    add_style_screen_main(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // clock_zones
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.clock_zones = obj;
            lv_obj_set_pos(obj, 21, -211);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "1972/6/22 12:8:25");
        }
        {
            // driver_name_zones
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.driver_name_zones = obj;
            lv_obj_set_pos(obj, 21, -196);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "driver name");
        }
        {
            // allOkButton
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.all_ok_button = obj;
            lv_obj_set_pos(obj, 411, 90);
            lv_obj_set_size(obj, 120, 55);
            lv_obj_add_event_cb(obj, action_main_event_dispatcher, LV_EVENT_PRESSED, (void *)0);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE);
            add_style_button_default(obj);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_FOCUSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "1 all ok");
                }
            }
        }
        {
            // compOkButton
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.comp_ok_button = obj;
            lv_obj_set_pos(obj, 538, 90);
            lv_obj_set_size(obj, 120, 55);
            lv_obj_add_event_cb(obj, action_main_event_dispatcher, LV_EVENT_PRESSED, (void *)0);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE);
            add_style_button_default(obj);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_FOCUSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "2 ok");
                }
            }
        }
        {
            // defectButton
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.defect_button = obj;
            lv_obj_set_pos(obj, 665, 90);
            lv_obj_set_size(obj, 120, 55);
            lv_obj_add_event_cb(obj, action_main_event_dispatcher, LV_EVENT_PRESSED, (void *)0);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE);
            add_style_button_default(obj);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_FOCUSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "3 defect");
                }
            }
        }
        {
            // zoneAssetList
            lv_obj_t *obj = lv_list_create(parent_obj);
            objects.zone_asset_list = obj;
            lv_obj_set_pos(obj, 14, 56);
            lv_obj_set_size(obj, 296, 115);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_ON);
            lv_obj_set_scroll_dir(obj, LV_DIR_VER);
            add_style_list(obj);
            lv_obj_set_style_pad_left(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_FOCUSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // assetTemplate
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.asset_template = obj;
                    lv_obj_set_pos(obj, 369, 42);
                    lv_obj_set_size(obj, 182, 45);
                    lv_obj_add_event_cb(obj, action_main_event_dispatcher, LV_EVENT_PRESSED, (void *)0);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_CHECKABLE);
                    add_style_button_default(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff2196f3), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Asset123");
                        }
                    }
                }
            }
        }
        {
            // zoneList
            lv_obj_t *obj = lv_list_create(parent_obj);
            objects.zone_list = obj;
            lv_obj_set_pos(obj, 12, 179);
            lv_obj_set_size(obj, 244, 246);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_ON);
            lv_obj_set_scroll_dir(obj, LV_DIR_VER);
            add_style_list(obj);
            lv_obj_set_style_pad_top(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_FOCUSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // zoneTemplate
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.zone_template = obj;
                    lv_obj_set_pos(obj, 503, 42);
                    lv_obj_set_size(obj, 230, 50);
                    lv_obj_add_event_cb(obj, action_main_event_dispatcher, LV_EVENT_PRESSED, (void *)0);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_CHECKABLE);
                    add_style_button_default(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffdddddd), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "1 Front");
                        }
                    }
                }
            }
        }
        {
            // zoneComponentList
            lv_obj_t *obj = lv_list_create(parent_obj);
            objects.zone_component_list = obj;
            lv_obj_set_pos(obj, 268, 179);
            lv_obj_set_size(obj, 247, 246);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_ON);
            lv_obj_set_scroll_dir(obj, LV_DIR_VER);
            add_style_list(obj);
            lv_obj_set_style_pad_top(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_FOCUSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // compoTemplate
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.compo_template = obj;
                    lv_obj_set_pos(obj, 503, 42);
                    lv_obj_set_size(obj, 280, 50);
                    lv_obj_add_event_cb(obj, action_main_event_dispatcher, LV_EVENT_PRESSED, (void *)0);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_CHECKABLE);
                    add_style_button_default(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffdddddd), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Left light");
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 1, 431);
            lv_obj_set_size(obj, 800, 50);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE);
            add_style_bottom_panel(obj);
        }
        {
            // arrow
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.arrow = obj;
            lv_obj_set_pos(obj, 321, -125);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "\uF077\n\uF078");
        }
        {
            // logo1_5
            lv_obj_t *obj = lv_img_create(parent_obj);
            objects.logo1_5 = obj;
            lv_obj_set_pos(obj, 324, 21);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_img_set_src(obj, &img_brick_logo);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
        }
        {
            // submit
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.submit = obj;
            lv_obj_set_pos(obj, 496, 436);
            lv_obj_set_size(obj, 289, 40);
            lv_obj_add_event_cb(obj, action_main_event_dispatcher, LV_EVENT_PRESSED, (void *)0);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE);
            add_style_button_default(obj);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_FOCUSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // submitLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.submit_label = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "4 save & submit \uF124");
                }
            }
        }
        {
            // backFromFormZones
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.back_from_form_zones = obj;
            lv_obj_set_pos(obj, 21, 437);
            lv_obj_set_size(obj, 94, 40);
            lv_obj_add_event_cb(obj, action_main_event_dispatcher, LV_EVENT_PRESSED, (void *)0);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE);
            add_style_button_default(obj);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_FOCUSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // backFromForm_1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.back_from_form_1 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "\uF053");
                }
            }
        }
        {
            lv_obj_t *obj = lv_textarea_create(parent_obj);
            lv_obj_set_pos(obj, 523, 179);
            lv_obj_set_size(obj, 262, 168);
            lv_textarea_set_max_length(obj, 128);
            lv_textarea_set_one_line(obj, false);
            lv_textarea_set_password_mode(obj, false);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_ON);
            lv_obj_set_scroll_dir(obj, LV_DIR_VER);
        }
        {
            // save_insp
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.save_insp = obj;
            lv_obj_set_pos(obj, 331, 436);
            lv_obj_set_size(obj, 157, 40);
            lv_obj_add_event_cb(obj, action_main_event_dispatcher, LV_EVENT_PRESSED, (void *)0);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE);
            add_style_button_default(obj);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_FOCUSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // submitLabel_1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.submit_label_1 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "5 save \uF019");
                }
            }
        }
        {
            // inspection_zones_overlay
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.inspection_zones_overlay = obj;
            lv_obj_set_pos(obj, -23, -20);
            lv_obj_set_size(obj, 846, 538);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // defectDialog
                    lv_obj_t *obj = lv_msgbox_create(parent_obj, "", "", 0, true);
                    objects.defect_dialog = obj;
                    lv_obj_set_pos(obj, 72, 41);
                    lv_obj_set_size(obj, 657, 401);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_ON);
                    lv_obj_set_scroll_dir(obj, LV_DIR_VER);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0xffe4572e), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_FULL, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        static lv_coord_t dsc[] = {LV_GRID_TEMPLATE_LAST};
                        lv_obj_set_style_grid_row_dsc_array(obj, dsc, LV_PART_MAIN | LV_STATE_DEFAULT);
                    }
                    {
                        static lv_coord_t dsc[] = {LV_GRID_TEMPLATE_LAST};
                        lv_obj_set_style_grid_column_dsc_array(obj, dsc, LV_PART_MAIN | LV_STATE_DEFAULT);
                    }
                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_layout(obj, LV_LAYOUT_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // defect_dialog_close_btn_v2
                            lv_obj_t *obj = lv_btn_create(parent_obj);
                            objects.defect_dialog_close_btn_v2 = obj;
                            lv_obj_set_pos(obj, 541, 1);
                            lv_obj_set_size(obj, 69, 58);
                            lv_obj_add_event_cb(obj, action_main_event_dispatcher, LV_EVENT_PRESSED, (void *)0);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE);
                            add_style_button_default(obj);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_FOCUSED);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "4 \uF00d");
                                }
                            }
                        }
                        {
                            // component_label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.component_label = obj;
                            lv_obj_set_pos(obj, -6, -10);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "component:");
                        }
                        {
                            // defect_dialog_title
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.defect_dialog_title = obj;
                            lv_obj_set_pos(obj, -6, 20);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xff6f88c6), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "the component goes here  XXXX  123");
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj6 = obj;
                            lv_obj_set_pos(obj, 279, 69);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "defect notes:");
                        }
                        {
                            // defect_dialog_notes
                            lv_obj_t *obj = lv_textarea_create(parent_obj);
                            objects.defect_dialog_notes = obj;
                            lv_obj_set_pos(obj, 279, 109);
                            lv_obj_set_size(obj, 326, 174);
                            lv_textarea_set_max_length(obj, 128);
                            lv_textarea_set_one_line(obj, false);
                            lv_textarea_set_password_mode(obj, false);
                            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_ON);
                            lv_obj_set_scroll_dir(obj, LV_DIR_VER);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            // defect_dialog_list
                            lv_obj_t *obj = lv_list_create(parent_obj);
                            objects.defect_dialog_list = obj;
                            lv_obj_set_pos(obj, -6, 53);
                            lv_obj_set_size(obj, 270, 230);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC);
                            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_ON);
                            lv_obj_set_scroll_dir(obj, LV_DIR_VER);
                            add_style_list(obj);
                            lv_obj_set_style_pad_top(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_left(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_right(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_border_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_FOCUSED);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // zoneTemplate_1
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.zone_template_1 = obj;
                                    lv_obj_set_pos(obj, 503, 42);
                                    lv_obj_set_size(obj, 230, 50);
                                    lv_obj_add_event_cb(obj, action_main_event_dispatcher, LV_EVENT_PRESSED, (void *)0);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_CHECKABLE);
                                    add_style_button_default(obj);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffdddddd), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "1 Front");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // defect_dialog_delete
                            lv_obj_t *obj = lv_btn_create(parent_obj);
                            objects.defect_dialog_delete = obj;
                            lv_obj_set_pos(obj, -6, 301);
                            lv_obj_set_size(obj, 199, 60);
                            lv_obj_add_event_cb(obj, action_main_event_dispatcher, LV_EVENT_PRESSED, (void *)0);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE);
                            add_style_button_default(obj);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_FOCUSED);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "1 delete \uF00d");
                                }
                            }
                        }
                        {
                            // defect_dialog_minor
                            lv_obj_t *obj = lv_btn_create(parent_obj);
                            objects.defect_dialog_minor = obj;
                            lv_obj_set_pos(obj, 210, 301);
                            lv_obj_set_size(obj, 188, 60);
                            lv_obj_add_event_cb(obj, action_main_event_dispatcher, LV_EVENT_PRESSED, (void *)0);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE);
                            add_style_button_default(obj);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_FOCUSED);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "2 minor \uF071");
                                }
                            }
                        }
                        {
                            // defect_dialog_major
                            lv_obj_t *obj = lv_btn_create(parent_obj);
                            objects.defect_dialog_major = obj;
                            lv_obj_set_pos(obj, 414, 301);
                            lv_obj_set_size(obj, 196, 60);
                            lv_obj_add_event_cb(obj, action_main_event_dispatcher, LV_EVENT_PRESSED, (void *)0);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE);
                            add_style_button_default(obj);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_FOCUSED);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "3 major \uF071\uF071");
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    tick_screen_inspection_zones();
}

void tick_screen_inspection_zones() {
}

void create_screen_login_screen() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.login_screen = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 800, 480);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SNAPPABLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
    add_style_screen_main(obj);
    lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // version_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.version_label = obj;
            lv_obj_set_pos(obj, -20, 215);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_RIGHT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "v 0.9.5");
        }
        {
            // CLOCK_LOGIN
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.clock_login = obj;
            lv_obj_set_pos(obj, 12, -211);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "1972/6/22 12 12:12:12");
        }
        {
            // logo1_6
            lv_obj_t *obj = lv_img_create(parent_obj);
            objects.logo1_6 = obj;
            lv_obj_set_pos(obj, 324, 21);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_img_set_src(obj, &img_brick_logo);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
        }
        {
            // login
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.login = obj;
            lv_obj_set_pos(obj, 320, 270);
            lv_obj_set_size(obj, 161, 50);
            lv_obj_add_event_cb(obj, action_main_event_dispatcher, LV_EVENT_PRESSED, (void *)0);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE);
            add_style_button_default(obj);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_FOCUSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "# Login");
                }
            }
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 216, 123);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "user");
        }
        {
            // login_username
            lv_obj_t *obj = lv_textarea_create(parent_obj);
            objects.login_username = obj;
            lv_obj_set_pos(obj, 292, 117);
            lv_obj_set_size(obj, 217, 60);
            lv_textarea_set_accepted_chars(obj, "0123456789");
            lv_textarea_set_max_length(obj, 6);
            lv_textarea_set_one_line(obj, true);
            lv_textarea_set_password_mode(obj, false);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
            add_style_text_area(obj);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff7095c8), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_FOCUSED);
        }
        {
            // doSettings_2
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.do_settings_2 = obj;
            lv_obj_set_pos(obj, 593, 194);
            lv_obj_set_size(obj, 188, 50);
            lv_obj_add_event_cb(obj, action_main_event_dispatcher, LV_EVENT_PRESSED, (void *)0);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE);
            add_style_button_default(obj);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_FOCUSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Settings");
                }
            }
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 141, 192);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "password");
        }
        {
            // login_password
            lv_obj_t *obj = lv_textarea_create(parent_obj);
            objects.login_password = obj;
            lv_obj_set_pos(obj, 292, 191);
            lv_obj_set_size(obj, 217, 60);
            lv_textarea_set_accepted_chars(obj, "0123456789");
            lv_textarea_set_max_length(obj, 6);
            lv_textarea_set_one_line(obj, true);
            lv_textarea_set_password_mode(obj, true);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
            add_style_text_area(obj);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff7095c8), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_FOCUSED);
        }
        {
            // doSync_2
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.do_sync_2 = obj;
            lv_obj_set_pos(obj, 592, 122);
            lv_obj_set_size(obj, 188, 50);
            lv_obj_add_event_cb(obj, action_main_event_dispatcher, LV_EVENT_PRESSED, (void *)0);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE);
            add_style_button_default(obj);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_FOCUSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Sync  \uF079");
                }
            }
        }
    }
    
    tick_screen_login_screen();
}

void tick_screen_login_screen() {
}

void create_screen_settings() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.settings = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 800, 480);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SNAPPABLE|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_ON);
    lv_obj_set_scroll_dir(obj, LV_DIR_VER);
    add_style_screen_main(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // logo1_2
            lv_obj_t *obj = lv_img_create(parent_obj);
            objects.logo1_2 = obj;
            lv_obj_set_pos(obj, 324, 21);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_img_set_src(obj, &img_brick_logo);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
        }
        {
            // backFromSettings
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.back_from_settings = obj;
            lv_obj_set_pos(obj, 31, 417);
            lv_obj_set_size(obj, 94, 40);
            lv_obj_add_event_cb(obj, action_main_event_dispatcher, LV_EVENT_PRESSED, (void *)0);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE);
            add_style_button_default(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "\uF053");
                }
            }
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj7 = obj;
            lv_obj_set_pos(obj, 32, 66);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Company");
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj8 = obj;
            lv_obj_set_pos(obj, 573, 66);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Brick Server");
        }
        {
            // setting_wifi_name
            lv_obj_t *obj = lv_textarea_create(parent_obj);
            objects.setting_wifi_name = obj;
            lv_obj_set_pos(obj, 400, 202);
            lv_obj_set_size(obj, 343, 60);
            lv_textarea_set_max_length(obj, 128);
            lv_textarea_set_one_line(obj, true);
            lv_textarea_set_password_mode(obj, false);
            add_style_text_area(obj);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // setting_wifi_password
            lv_obj_t *obj = lv_textarea_create(parent_obj);
            objects.setting_wifi_password = obj;
            lv_obj_set_pos(obj, 400, 270);
            lv_obj_set_size(obj, 345, 60);
            lv_textarea_set_max_length(obj, 128);
            lv_textarea_set_one_line(obj, true);
            lv_textarea_set_password_mode(obj, false);
            add_style_text_area(obj);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // setting_server_url
            lv_obj_t *obj = lv_textarea_create(parent_obj);
            objects.setting_server_url = obj;
            lv_obj_set_pos(obj, 402, 96);
            lv_obj_set_size(obj, 343, 60);
            lv_textarea_set_max_length(obj, 128);
            lv_textarea_set_one_line(obj, true);
            lv_textarea_set_password_mode(obj, false);
            add_style_text_area(obj);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj9 = obj;
            lv_obj_set_pos(obj, 461, 167);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Wifi  User /  Passwd");
        }
        {
            // setting_company
            lv_obj_t *obj = lv_textarea_create(parent_obj);
            objects.setting_company = obj;
            lv_obj_set_pos(obj, 31, 96);
            lv_obj_set_size(obj, 343, 60);
            lv_textarea_set_max_length(obj, 128);
            lv_textarea_set_one_line(obj, true);
            lv_textarea_set_password_mode(obj, false);
            add_style_text_area(obj);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj10 = obj;
            lv_obj_set_pos(obj, 31, 167);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Timezone");
        }
        {
            // settings_tz
            lv_obj_t *obj = lv_dropdown_create(parent_obj);
            objects.settings_tz = obj;
            lv_obj_set_pos(obj, 30, 206);
            lv_obj_set_size(obj, 344, 52);
            lv_dropdown_set_options(obj, "(UTC-10:00) Pacific/Honolulu\n(UTC-09:00) America/Anchorage\n(UTC-08:00) America/Los_Angeles\n(UTC-07:00) America/Denver\n(UTC-07:00) America/Phoenix\n(UTC-06:00) America/Chicago\n(UTC-05:00) America/New_York\n(UTC-08:00) America/Vancouver\n(UTC-07:00) America/Edmonton\n(UTC-06:00) America/Winnipeg\n(UTC-05:00) America/Toronto\n(UTC-04:00) America/Halifax\n(UTC-03:30) America/St_Johns\n(UTC-08:00) America/Tijuana\n(UTC-07:00) America/Chihuahua\n(UTC-06:00) America/Mexico_City\n(UTC-05:00) America/Cancun\n(UTC-04:00) America/Puerto_Rico\n(UTC-05:00) America/Jamaica\n(UTC-04:00) America/Barbados\n(UTC-05:00) America/Nassau\n(UTC-05:00) America/Lima\n(UTC-05:00) America/Bogota\n(UTC-04:00) America/Caracas\n(UTC-04:00) America/Santiago\n(UTC-03:00) America/Argentina/Buenos_Aires\n(UTC-03:00) America/Sao_Paulo");
            lv_dropdown_set_selected(obj, 0);
            lv_obj_add_event_cb(obj, action_main_event_dispatcher, LV_EVENT_VALUE_CHANGED, (void *)0);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_FOCUSED);
            lv_obj_set_style_border_width(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // DST
            lv_obj_t *obj = lv_switch_create(parent_obj);
            objects.dst = obj;
            lv_obj_set_pos(obj, 39, 286);
            lv_obj_set_size(obj, 50, 25);
            lv_obj_add_event_cb(obj, action_main_event_dispatcher, LV_EVENT_VALUE_CHANGED, (void *)0);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff747a81), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffe4572e), LV_PART_INDICATOR | LV_STATE_CHECKED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0000), LV_PART_KNOB | LV_STATE_FOCUSED);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj11 = obj;
            lv_obj_set_pos(obj, 109, 285);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "DST");
        }
        {
            // CLOCK_settings
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.clock_settings = obj;
            lv_obj_set_pos(obj, 35, -211);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "1972/6/22 12:8:25");
        }
        {
            // CLOCK_settings_1
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.clock_settings_1 = obj;
            lv_obj_set_pos(obj, 30, 114);
            lv_obj_set_size(obj, 344, 47);
            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "If clock settings are changed, you will need to sync for the clock to take the settings.");
        }
    }
    
    tick_screen_settings();
}

void tick_screen_settings() {
}

void create_screen_sandbox() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.sandbox = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 800, 480);
    
    tick_screen_sandbox();
}

void tick_screen_sandbox() {
}



typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
    tick_screen_select_asset_screen,
    tick_screen_select_inspection_type,
    tick_screen_inspection_form,
    tick_screen_inspection_zones,
    tick_screen_login_screen,
    tick_screen_settings,
    tick_screen_sandbox,
};
void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen_funcs[screenId - 1]();
}

void create_screens() {
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_main();
    create_screen_select_asset_screen();
    create_screen_select_inspection_type();
    create_screen_inspection_form();
    create_screen_inspection_zones();
    create_screen_login_screen();
    create_screen_settings();
    create_screen_sandbox();
}
