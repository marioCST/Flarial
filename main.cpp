#pragma once

#include <windows.h>
#include <algorithm>

#include "src/Client/Client.hpp"
#include "src/Client/Events/EventHandler.hpp"
#include "src/Client/Hook/Hooks/Render/ResizeHook.hpp"
#include "src/Client/Hook/Hooks/Game/RaknetTick.hpp"
#include "src/Client/Module/Modules/Nick/NickListener.hpp"
#include <kiero.h>
#include <wininet.h>
#include "src/Client/Module/Modules/Misc/DiscordRPC/DiscordRPCListener.hpp"


std::chrono::steady_clock::time_point lastBeatTime;

std::string replaceAll(std::string subject, const std::string& search,
                       const std::string& replace);

std::string DownloadString(std::string URL);

std::string removeColorCodes(const std::string& input);

DWORD WINAPI init(HMODULE real)
{
#ifndef NDEBUG
    if (GetConsoleWindow() == nullptr) {
        AllocConsole();
        SetConsoleTitleA("Flarial-Debugger");
        FILE *out;
        freopen_s(&out, ("CONOUT$"), ("w"), stdout);
    }
#endif

    Client::initialize();
    Logger::info("[Client] Initializing");
    

    std::thread statusThread([]() {
        while (true) {

            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastBeatTime);

            if (!Client::disable) {
                if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
                    if (SDK::clientInstance->getLocalPlayer() != nullptr) {
                        if (elapsed >= std::chrono::seconds(60)) {
                            ModuleManager::onlineUsers.clear();
                            ModuleManager::onlineDevs.clear();
                            ModuleManager::onlinePluses.clear();
                            ModuleManager::onlineStaff.clear();
                            ModuleManager::onlineCommites.clear();
                            std::string name = SDK::clientInstance->getLocalPlayer()->playerName;
                            ModuleManager::onlineUsers.push_back(Utils::removeColorCodes(name));


                            std::string ipToSend;


                            auto module = ModuleManager::getModule("Nick");

                            if (SDK::clientInstance != nullptr)
                                if (SDK::clientInstance->getLocalPlayer() != nullptr)
                                    if (module->isEnabled()) {
                                        name = Utils::removeNonAlphanumeric(Utils::removeColorCodes(NickListener::original));
                                    }
                            Logger::info(DownloadString("http://de2.bot-hosting.net:20358/heartbeat/" + Utils::removeColorCodes(name)));
                            std::string pp = DownloadString("http://de2.bot-hosting.net:20358/users");
                            Logger::info(pp);
                            

                            json playersDict = json::parse(pp);

                            int totalPlaytime = 0;
                            int numberOfPlayers = 0;

                            for (const auto& player : playersDict.items()) {

                                std::time_t unixTimestamp = player.value()["lastbeat"];
                                std::chrono::time_point<std::chrono::system_clock> timePoint = std::chrono::system_clock::from_time_t(unixTimestamp);

                                std::string icon = player.value()["icon"];
                                if (icon == "regular") {

                                    ModuleManager::onlineUsers.push_back(Utils::removeNonAlphanumeric(player.key()));
                                }
                                if (icon == "dev") {

                                    ModuleManager::onlineDevs.push_back(Utils::removeNonAlphanumeric(player.key()));
                                }
                                if (icon == "commiter") {

                                    ModuleManager::onlineCommites.push_back(Utils::removeNonAlphanumeric(player.key()));
                                }
                                if (icon == "plus") {

                                    ModuleManager::onlinePluses.push_back(Utils::removeNonAlphanumeric(player.key()));
                                }
                                if (icon == "staff") {

                                    ModuleManager::onlineStaff.push_back(Utils::removeNonAlphanumeric(player.key()));
                                }

                                //std::cout << Utils::removeNonAlphanumeric(player.key()) << std::endl;

                            }

                            // send thing
                          
                            

                            lastBeatTime = now;
                        }
                    }
                }
                Sleep(60);

            }
            else break;
        }
        });
    statusThread.detach();
    

    while (true) {
        if (Client::disable) {
            break;
        } else {
            Sleep(50); // wtf 50ms too high
        }
    }

    Client::SaveSettings();

    Logger::info("Uninitializing Client");

    EventHandler::unregisterAll();

    ModuleManager::terminate();
    HookManager::terminate();

    kiero::shutdown();

    Logger::debug("[Kiero] Shut down Kiero.");

    ResizeHook::cleanShit();

    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();

    Logger::debug("[MinHook] Freeing Library.");

    Sleep(100);

    FreeLibraryAndExitThread(real, 1);
}

BOOL APIENTRY DllMain(HMODULE instance, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)init, instance, 0, nullptr);
        break;
    case DLL_PROCESS_DETACH:
        ModuleManager::terminate();
    }

    return TRUE;
}

std::string DownloadString(std::string URL) {
    HINTERNET interwebs = InternetOpenA("Samsung Smart Fridge", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
    HINTERNET urlFile;
    std::string rtn;
    if (interwebs) {
        urlFile = InternetOpenUrlA(interwebs, URL.c_str(), NULL, NULL, NULL, NULL);
        if (urlFile) {
            char buffer[2000];
            DWORD bytesRead;
            do {
                InternetReadFile(urlFile, buffer, 2000, &bytesRead);
                rtn.append(buffer, bytesRead);
                memset(buffer, 0, 2000);
            } while (bytesRead);
            InternetCloseHandle(interwebs);
            InternetCloseHandle(urlFile);
            std::string p = replaceAll(rtn, "|n", "\r\n");
            return p;
        }
    }
    InternetCloseHandle(interwebs);
    std::string p = replaceAll(rtn, "|n", "\r\n");
    return p;
}

std::string replaceAll(std::string subject, const std::string& search,
                       const std::string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
    return subject;
}

std::string removeColorCodes(const std::string& input) {
    std::string result;
    bool skipNext = false;

    for (char c : input) {
        if (skipNext) {
            skipNext = false;
        } else if (c == L'§') {
            skipNext = true;
        } else {
            result += c;
        }
    }

    return result;
}
