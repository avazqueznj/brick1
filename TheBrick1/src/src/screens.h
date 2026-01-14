#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *select_asset_screen;
    lv_obj_t *select_inspection_type;
    lv_obj_t *inspection_form;
    lv_obj_t *inspection_zones;
    lv_obj_t *login_screen;
    lv_obj_t *settings;
    lv_obj_t *sandbox;
    lv_obj_t *inspection_history;
    lv_obj_t *clock;
    lv_obj_t *driver_name_main;
    lv_obj_t *battery_main;
    lv_obj_t *logo1;
    lv_obj_t *do_inspect_button;
    lv_obj_t *do_sync;
    lv_obj_t *logout;
    lv_obj_t *do_history;
    lv_obj_t *logo1_1;
    lv_obj_t *battery_asset;
    lv_obj_t *temp_asset_button;
    lv_obj_t *do_select_inspection_type;
    lv_obj_t *back_from_select_asset;
    lv_obj_t *asset_list;
    lv_obj_t *obj0;
    lv_obj_t *obj1;
    lv_obj_t *selected_asset_list;
    lv_obj_t *obj2;
    lv_obj_t *de_select_asset;
    lv_obj_t *select_asset;
    lv_obj_t *search_asset;
    lv_obj_t *search_asset_clear;
    lv_obj_t *clock_asset;
    lv_obj_t *driver_name_asset;
    lv_obj_t *temp_asset_overlay;
    lv_obj_t *temp_asset_dialog;
    lv_obj_t *temp_asset_layout_label;
    lv_obj_t *temp_asset_id_label;
    lv_obj_t *temp_asset_id;
    lv_obj_t *temp_asset_layouts;
    lv_obj_t *zone_template_5;
    lv_obj_t *zone_template_6;
    lv_obj_t *temp_asset_ok;
    lv_obj_t *temp_asset_cancel;
    lv_obj_t *battery_type;
    lv_obj_t *logo1_3;
    lv_obj_t *inspection_types;
    lv_obj_t *obj3;
    lv_obj_t *obj4;
    lv_obj_t *do_inspection_form;
    lv_obj_t *back_from_select_insp;
    lv_obj_t *obj5;
    lv_obj_t *clock_insptype;
    lv_obj_t *driver_name_insptype;
    lv_obj_t *logo1_4;
    lv_obj_t *do_zones;
    lv_obj_t *back_from_form_fields;
    lv_obj_t *back_from_form;
    lv_obj_t *form_fields;
    lv_obj_t *obj6;
    lv_obj_t *inspection_type_name;
    lv_obj_t *forms_info_label;
    lv_obj_t *clock_form;
    lv_obj_t *driver_name_form;
    lv_obj_t *battery_form;
    lv_obj_t *clock_zones;
    lv_obj_t *driver_name_zones;
    lv_obj_t *battery_zones;
    lv_obj_t *all_ok_button;
    lv_obj_t *comp_ok_button;
    lv_obj_t *defect_button;
    lv_obj_t *zone_asset_list;
    lv_obj_t *asset_template;
    lv_obj_t *zone_list;
    lv_obj_t *zone_template_2;
    lv_obj_t *zone_template;
    lv_obj_t *zone_template_3;
    lv_obj_t *zone_component_list;
    lv_obj_t *compo_template;
    lv_obj_t *arrow;
    lv_obj_t *logo1_5;
    lv_obj_t *insp_component_instructions;
    lv_obj_t *back_from_form_zones;
    lv_obj_t *back_from_form_1;
    lv_obj_t *save_insp;
    lv_obj_t *submit_label_1;
    lv_obj_t *zone_pic;
    lv_obj_t *submit_label_2;
    lv_obj_t *take_pic;
    lv_obj_t *submit_label_3;
    lv_obj_t *submit;
    lv_obj_t *submit_label;
    lv_obj_t *inspection_zones_overlay;
    lv_obj_t *defect_dialog;
    lv_obj_t *component_label;
    lv_obj_t *defect_dialog_title;
    lv_obj_t *defect_dialog_notes;
    lv_obj_t *obj7;
    lv_obj_t *defect_dialog_close_btn_v2;
    lv_obj_t *defect_dialog_list;
    lv_obj_t *zone_template_4;
    lv_obj_t *zone_template_1;
    lv_obj_t *defect_dialog_delete;
    lv_obj_t *defect_dialog_minor;
    lv_obj_t *defect_dialog_major;
    lv_obj_t *pic_overlay;
    lv_obj_t *pic_dialog;
    lv_obj_t *pictures_label;
    lv_obj_t *pic_close;
    lv_obj_t *pic_shoot1;
    lv_obj_t *pic_del1;
    lv_obj_t *pic_view1;
    lv_obj_t *pic_shoot2;
    lv_obj_t *pic_del2;
    lv_obj_t *pic_view2;
    lv_obj_t *pic_shoot3;
    lv_obj_t *pic_del3;
    lv_obj_t *pic_view3;
    lv_obj_t *pic_shoot4;
    lv_obj_t *pic_del4;
    lv_obj_t *pic_view4;
    lv_obj_t *version_label;
    lv_obj_t *clock_login;
    lv_obj_t *battery_login;
    lv_obj_t *logo1_6;
    lv_obj_t *login_username;
    lv_obj_t *login_password;
    lv_obj_t *login;
    lv_obj_t *do_settings_2;
    lv_obj_t *do_sync_2;
    lv_obj_t *test2;
    lv_obj_t *logo1_2;
    lv_obj_t *battery_settings;
    lv_obj_t *back_from_settings;
    lv_obj_t *obj8;
    lv_obj_t *setting_wifi_name;
    lv_obj_t *setting_wifi_password;
    lv_obj_t *obj9;
    lv_obj_t *setting_company;
    lv_obj_t *obj10;
    lv_obj_t *settings_tz;
    lv_obj_t *dst;
    lv_obj_t *obj11;
    lv_obj_t *clock_settings;
    lv_obj_t *setting_location;
    lv_obj_t *logo1_7;
    lv_obj_t *history_list;
    lv_obj_t *obj12;
    lv_obj_t *back_from_history;
    lv_obj_t *obj13;
    lv_obj_t *clock_history;
    lv_obj_t *battery_history;
    lv_obj_t *driver_name_history;
    lv_obj_t *open_inspection;
    lv_obj_t *inspection_detail_dialog;
    lv_obj_t *re_submit_inspection;
    lv_obj_t *obj14;
    lv_obj_t *history_close;
    lv_obj_t *inspection_view;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
    SCREEN_ID_SELECT_ASSET_SCREEN = 2,
    SCREEN_ID_SELECT_INSPECTION_TYPE = 3,
    SCREEN_ID_INSPECTION_FORM = 4,
    SCREEN_ID_INSPECTION_ZONES = 5,
    SCREEN_ID_LOGIN_SCREEN = 6,
    SCREEN_ID_SETTINGS = 7,
    SCREEN_ID_SANDBOX = 8,
    SCREEN_ID_INSPECTION_HISTORY = 9,
};

void create_screen_main();
void tick_screen_main();

void create_screen_select_asset_screen();
void tick_screen_select_asset_screen();

void create_screen_select_inspection_type();
void tick_screen_select_inspection_type();

void create_screen_inspection_form();
void tick_screen_inspection_form();

void create_screen_inspection_zones();
void tick_screen_inspection_zones();

void create_screen_login_screen();
void tick_screen_login_screen();

void create_screen_settings();
void tick_screen_settings();

void create_screen_sandbox();
void tick_screen_sandbox();

void create_screen_inspection_history();
void tick_screen_inspection_history();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/