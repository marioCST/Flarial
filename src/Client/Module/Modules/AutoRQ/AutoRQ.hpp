#pragma once

#include "../Module.hpp"
#include "../../../Events/EventHandler.hpp"
#include "AutoRQListener.hpp"


class AutoRQ : public Module {

public:


    AutoRQ() : Module("Auto RE Q", "Automatically requeues into a game (Hive)",
                      R"(\Flarial\assets\re-q.png)", "") {

        Module::setup();

    };

    void onEnable() override {
        EventHandler::registerListener(new AutoRQListener("AutoRQ", this));
        Module::onEnable();
    }

    void onDisable() override {
        EventHandler::unregisterListener("AutoRQ");
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<std::string>("command") == nullptr)
            settings.addSetting("command", (std::string)"");
        if (settings.getSettingByName<bool>("solo") == nullptr)
            settings.addSetting("solo", false);
    }

    void settingsRender() override {
        float toggleX = Constraints::PercentageConstraint(0.019, "left");
        float toggleY = Constraints::PercentageConstraint(0.10, "top");

        const float textWidth = Constraints::RelativeConstraint(0.12, "height", true);
        const float textHeight = Constraints::RelativeConstraint(0.029, "height", true);

        FlarialGUI::FlarialTextWithFont(toggleX + Constraints::SpacingConstraint(0.60, textWidth), toggleY,
                                        L"Solo mode", textWidth * 6.9f, textHeight,
                                        DWRITE_TEXT_ALIGNMENT_LEADING, Constraints::SpacingConstraint(1.05, textWidth),
                                        DWRITE_FONT_WEIGHT_NORMAL);

        if (FlarialGUI::Toggle(5, toggleX, toggleY, this->settings.getSettingByName<bool>(
                "solo")->value))
            this->settings.getSettingByName<bool>("solo")->value = !this->settings.getSettingByName<bool>(
                    "solo")->value;
    }
};

