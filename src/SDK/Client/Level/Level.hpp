#pragma once
#include <string>
#include <unordered_map>

class mcUUID {
public:
    uint64_t mostSig, leastSig;
};

class PlayerListEntry {
public:
    char pad_0000[24]; //0x0000
    std::string name; //0x0018
    char pad_0020[328]; //0x0020
    void *N0000007C; //0x0168
};

class Level {
    using troll = std::unordered_map<mcUUID, PlayerListEntry>;

public:
    troll& getPlayerMap() {
        return direct_access<troll>(this, 0x1EA8);
    }
};