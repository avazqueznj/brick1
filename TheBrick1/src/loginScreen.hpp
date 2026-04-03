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
 ********************************************************************************************/

#include "arducam_dvp.h"
#include "OV7670/ov767x.h"

// global calls
void navigateTo(int screenId);
void configChanged();

class loginScreenClass : public screenClass
{
public:
    loginScreenClass(settingsClass *settingsParam) : screenClass(settingsParam, SCREEN_ID_LOGIN_SCREEN)
    {
    }

    void clockTic(String time) override
    {
        lv_label_set_text(objects.clock_login, time.c_str());
    }

    void batteryInfo(String info) override
    {
        lv_label_set_text(objects.battery_login, info.c_str());
    }

    void handleKeyboardEvent(String key) override
    {
        screenClass::handleKeyboardEvent(key);

        lv_obj_t *focused = lv_group_get_focused(inputGroup);

        // add numeric input to focused text areas
        if (key != "A" && key != "B" && key != "C" && key != "D" && key != "*" && key != "#")
        {

            if (focused && lv_obj_check_type(focused, &lv_textarea_class))
            {
                lv_textarea_add_text(focused, key.c_str());
            }
        }

        // use * as backspace
        if (key == "*")
        {
            if (focused && lv_obj_check_type(focused, &lv_textarea_class))
            {
                String txt = lv_textarea_get_text(focused); // copy the text
                int len = txt.length();
                if (len > 0)
                {
                    txt = txt.substring(0, len - 1); // remove last character
                    // lv_textarea_set_text(focused, txt.c_str());
                    // lv_textarea_add_text(focused, "*" );

                    // lvgl bug ??
                    // One for the ghost, one for the real char
                    lv_textarea_del_char(focused);
                    lv_textarea_del_char(focused);
                }
            }
        }

        if (
            (focused == objects.login && key == "#") ||
            (focused == objects.login_password && key == "#"))
        {
            try
            {
                if (
                    domainManagerClass::getInstance()->login(
                        String(lv_textarea_get_text(objects.login_username)),
                        String(lv_textarea_get_text(objects.login_password))))
                {
                    navigateTo(SCREEN_ID_MAIN);
                }
                else
                {
                    showDialog("Invalid credentials");
                }
            }
            catch (std::runtime_error &error)
            {
                showDialog(error.what());
            }
        }

        if (focused == objects.do_sync_2 && key == "#")
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

        if (focused == objects.do_settings_2 && key == "#")
        {
            Serial.println("Open Settings!");
            navigateTo(SCREEN_ID_SETTINGS);
        }
    }

    //---------->

    void handleTouchEvent(lv_event_t *e) override
    {
        lv_obj_t *target = lv_event_get_target(e);

        if (target == objects.do_sync_2)
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

        if (target == objects.do_settings_2)
        {
            Serial.println("Open Settings!");
            navigateTo(SCREEN_ID_SETTINGS);
        }

        if (
            (target == objects.login))
        {
            try
            {
                if (
                    domainManagerClass::getInstance()->login(
                        String(lv_textarea_get_text(objects.login_username)),
                        String(lv_textarea_get_text(objects.login_password)))

                )
                {
                    navigateTo(SCREEN_ID_MAIN);
                }
                else
                {
                    showDialog("Invalid credentials");
                }
            }
            catch (std::runtime_error &error)
            {
                showDialog(error.what());
            }
        }

        if (target == objects.test2)
        {

            try
            {

                cameraManagerClass *camera = cameraManagerClass::getInstance();
                camera->shoot();
                cameraManagerClass::getInstance()->displayJpegFromSDRAM(jpg_holder);
            }
            catch (std::runtime_error &error)
            {
            }
        }

    }; //==

    void init() override
    {

        {

            lv_group_add_obj(inputGroup, objects.login_username);
            lv_group_add_obj(inputGroup, objects.login_password);
            lv_group_add_obj(inputGroup, objects.login);

            lv_group_add_obj(inputGroup, objects.do_sync_2);
            lv_group_add_obj(inputGroup, objects.do_settings_2);
        }

        screenClass::makeKeyboards();
        screenClass::addNumericKeyboard(objects.login_username);
        screenClass::addNumericKeyboard(objects.login_password);

        screenClass::init();

        Serial.println("Login inited *********");
    }

    void start() override
    {

        Serial.println(">>>Setting Start *********");

        lv_textarea_set_text(objects.login_username, "");
        lv_textarea_set_text(objects.login_password, "");

        Serial.println("<<<Setting started *********");
    }

    virtual ~loginScreenClass() {
    };
};
