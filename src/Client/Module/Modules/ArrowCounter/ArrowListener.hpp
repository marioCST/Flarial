#pragma once


#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>
#include <chrono>

class ArrowListener : public Listener {
public:

    Module* module;
    int arrows = 0;



    void onLocalTick(TickEvent& event) override {

    }

    void onRender(RenderEvent& event) override {



        if (SDK::CurrentScreen == "hud_screen")
            if (module->settings.getSettingByName<bool>("enabled")->value && SDK::clientInstance->getLocalPlayer() != nullptr) {

                if (SDK::clientInstance->getLocalPlayer()->playerInventory != nullptr) {

                    auto inventory = SDK::clientInstance->getLocalPlayer()->playerInventory->inventory;

                    //dosent count offhand yet.


                    for (int i = 0; i < 36; i++) {
                        auto item = inventory->getItem(i);

                        if (item->getItem() != NULL) {
                            if (item->getItem()->name == "arrow") {


                                int counte = item->count;
                                if (counte != 16908288) {
                                    auto countagain = 0;
                                    countagain = counte / 65536 - 258;
                                    arrows = arrows + countagain + 2;
                                }
                            }

                        }
                    }


                    this->module->NormalRender(13, module->settings.getSettingByName<std::string>("text")->value, std::to_string(arrows));

                    arrows = 0;
                }
            }
    }

public:
    explicit ArrowListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }

};

