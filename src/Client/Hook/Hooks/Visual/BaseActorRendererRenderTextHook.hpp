#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "../Hook.hpp"
#include "../../../../SDK/Client/Render/Font.hpp"
#include "../../../../SDK/Client/Render/NameTagRenderObject.hpp"
#include "../../../../SDK/Client/Render/ScreenContext.hpp"
#include "../../../../SDK/Client/Render/ViewRenderData.hpp"
#include "../../../../SDK/Client/Render/Tessellator/MeshHelpers.hpp"
#include "../../../../SDK/Client/Util/MathUtility.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Utils.hpp"

class BaseActorRendererRenderTextHook : public Hook {
private:

    static void drawLogo(ScreenContext* screenContext, const Vec3<float>& cameraPos, const Vec3<float>& cameraTargetPos, const std::string& nameTag, const Vec3<float>& tagPos, Font* font) {
        std::string clearedName = Utils::removeNonAlphanumeric(Utils::removeColorCodes(nameTag));
        if (clearedName.empty()) clearedName = Utils::removeColorCodes(nameTag); // nametag might contain some unclearable stuff

        if(!contains(Client::onlinePlayers, clearedName)) return;

        if (MaterialUtils::getUITextured() == nullptr)
            MaterialUtils::update();

        constexpr float DEG_RAD = 180.0f / 3.1415927f;

        const auto rotPos = cameraPos.sub(tagPos);
        const auto rot = mce::MathUtility::getRotationFromNegZToVector(rotPos);
        const float yaw = rot.y * -DEG_RAD;
        const float pitch = rot.x * DEG_RAD;
        const auto pos = tagPos.sub(cameraTargetPos);

        auto& stack = SDK::clientInstance->getCamera().getWorldMatrixStack();

        stack.push();

        auto& matrix = stack.top().matrix;

        matrix = translate(matrix, {pos.x, pos.y, pos.z});
        matrix = rotate(matrix, glm::radians(yaw), {0.f, 1.f, 0.f});
        matrix = rotate(matrix, glm::radians(pitch), {1.f, 0.f, 0.f});

        const auto mScale = 0.026666669f; // 0.16f
        matrix = scale(matrix, {mScale * -1, mScale * -1, mScale});

        const ResourceLocation loc(Utils::getRoamingPath() + R"(\Flarial\assets\red-logo.png)", true); // The logo is the normal transparent Flarial logo, 128x128
        const TexturePtr ptr = SDK::clientInstance->getMinecraftGame()->textureGroup->getTexture(loc, false);

        const float fontHeight = font->getLineHeight();
        float x;
        const float size = fontHeight;
        const float y = -1.f;

        if (std::ranges::find(nameTag.begin(), nameTag.end(), '\n') != nameTag.end()) {
            const auto split = Utils::splitString(nameTag, '\n');

            float width = 0.f;

            for (const auto& tag : split) {
                const auto w = font->getLineLength(tag, 1.f, false);

                if (w > width)
                    width = w;
            }

            x = -(width / 2.f + size + 2.f);
        }
        else
            x = -(font->getLineLength(nameTag, 1.f, false) / 2.f + size + 2.f);

        const auto shaderColor = screenContext->getColorHolder();

        shaderColor->r = 1.f;
        shaderColor->g = 1.f;
        shaderColor->b = 1.f;
        shaderColor->a = 1.f;
        shaderColor->shouldDelete = true;

        const auto tess = screenContext->getTessellator();

        tess->begin();

        tess->vertexUV(x, y, 0.f, 0.f, 0.f);
        tess->vertexUV(x, y + size, 0.f, 0.f, 1.f);
        tess->vertexUV(x + size, y, 0.f, 1.f, 0.f);

        tess->vertexUV(x, y + size, 0.f, 0.f, 1.f);
        tess->vertexUV(x + size, y + size, 0.f, 1.f, 1.f);
        tess->vertexUV(x + size, y, 0.f, 1.f, 0.f);

        MeshHelpers::renderMeshImmediately2(screenContext, tess, MaterialUtils::getUITextured(), *ptr.clientTexture);

        stack.pop();
    }

    static bool contains(const std::vector<std::string>& vec, const std::string& str) {
        return std::find(vec.begin(), vec.end(), str) != vec.end();
    }

    static void BaseActorRenderer_renderTextCallback(ScreenContext* screenContext, ViewRenderData* viewData, NameTagRenderObject* tagData, Font* font, float size) {
        drawLogo(screenContext, viewData->cameraPos, viewData->cameraTargetPos, tagData->nameTag, tagData->pos, font);

        funcOriginal(screenContext, viewData, tagData, font, size);
    }

    static void BaseActorRenderer_renderTextCallback40(ScreenContext* screenContext, ViewRenderData* viewData, NameTagRenderObject* tagData, Font* font, void* mesh) {
        drawLogo(screenContext, viewData->cameraPos, viewData->cameraTargetPos, tagData->nameTag, tagData->pos, font);

        funcOriginal40(screenContext, viewData, tagData, font, mesh);
    }

public:
    typedef void(__fastcall* BaseActorRenderer_renderTextOriginal)(ScreenContext*, ViewRenderData*, NameTagRenderObject*, Font*, float size);
    typedef void(__fastcall* BaseActorRenderer_renderTextOriginal40)(ScreenContext*, ViewRenderData*, NameTagRenderObject*, Font*, void* mesh);

    static inline BaseActorRenderer_renderTextOriginal funcOriginal = nullptr;
    static inline BaseActorRenderer_renderTextOriginal40 funcOriginal40 = nullptr;

    BaseActorRendererRenderTextHook() : Hook("BaseActorRenderer renderText Hook", GET_SIG("BaseActorRenderer::renderText")) {}

    void enableHook() override {

        static auto sig = Memory::offsetFromSig(Memory::findSig(this->signature), 1);

        if (WinrtUtils::check(20, 40))
            this->manualHook( (void*) sig, (void*) BaseActorRenderer_renderTextCallback40, (void **) &funcOriginal40);
        else
            this->manualHook( (void*) sig, (void*) BaseActorRenderer_renderTextCallback, (void **) &funcOriginal);

    }
};


