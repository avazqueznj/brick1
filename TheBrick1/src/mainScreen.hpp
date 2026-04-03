/********************************************************************************************
 * Copyright 2026 Alejandro Vazquez
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
 *
 * Portions of this software are based on LVGL (https://lvgl.io),
 * which is licensed under the MIT License.
 *
 ********************************************************************************************/

void navigateTo(int screenId);

class mainScreenClass : public screenClass
{
public:
    mainScreenClass(settingsClass *settingsParam) : screenClass(settingsParam, SCREEN_ID_SETTINGS)
    {
    }

    void clockTic(String time) override
    {
        lv_label_set_text(objects.clock, time.c_str());
        lv_label_set_text(objects.driver_name_main, domainManagerClass::getInstance()->loggedUser.name.c_str());
    }

    void batteryInfo(String info) override
    {
        lv_label_set_text(objects.battery_main, info.c_str());
    }

    void handleKeyboardEvent(String key) override
    {
        screenClass::handleKeyboardEvent(key);
        lv_obj_t *focused = lv_group_get_focused(inputGroup);

        if (
            (focused == objects.do_sync && key == "#"))
        {
            try
            {
                showDialog(domainManagerClass::getInstance()->sync());
            }
            catch (const std::runtime_error &error)
            {
                Serial.println(error.what());
                showDialog(error.what());
            }
        }

        if (
            (focused == objects.logout && key == "#"))
        {

            domainManagerClass::getInstance()->logout();
        }

        if (
            (focused == objects.do_inspect_button && key == "#"))
        {

            navigateTo(SCREEN_ID_SELECT_ASSET_SCREEN);
        }

        if (
            (focused == objects.do_history && key == "#"))
        {

            navigateTo(SCREEN_ID_INSPECTION_HISTORY);
        }
    }

    void handleTouchEvent(lv_event_t *e) override
    {
        lv_obj_t *target = lv_event_get_target(e);

        if (
            (target == objects.do_sync))
        {
            try
            {
                showDialog(domainManagerClass::getInstance()->sync());
            }
            catch (const std::runtime_error &error)
            {
                Serial.println(error.what());
                showDialog(error.what());
            }
        }

        if (
            (target == objects.logout))
        {

            domainManagerClass::getInstance()->logout();
        }

        if (
            (target == objects.do_inspect_button))
        {

            navigateTo(SCREEN_ID_SELECT_ASSET_SCREEN);
        }

        if (
            (target == objects.do_history))
        {

            navigateTo(SCREEN_ID_INSPECTION_HISTORY);
        }
    }

    void init() override
    {

        // Create fresh group or reuse existing
        {
            lv_group_add_obj(inputGroup, objects.do_inspect_button);
            lv_group_add_obj(inputGroup, objects.do_history);
            lv_group_add_obj(inputGroup, objects.do_sync);
            lv_group_add_obj(inputGroup, objects.logout);
        }
    }

    virtual ~mainScreenClass() {
    };
};
