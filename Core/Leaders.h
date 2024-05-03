#pragma once
#include <vector>
#include <fstream>
#include <string>
#include "Utils.h"

const std::string LeaderResources[4] = {
    "Supplies",
    "CampaignFoo",
    "Collectable",
    "Power"
};

const std::string leadersPath = ("..\\ModData\\data\\leaders.xml");
const std::string leadersBackUpPath("..\\ModData\\data\\leadersBackUp.xml");

inline void SaveMultipleValues(std::ifstream& file, std::string& line, std::string& saveBuildString, const std::vector<std::string>&& values);
inline void SaveMultipleValues(std::ifstream& file, std::string& line, std::string& saveBuildString, const std::vector<int>&& values);


struct InitialLeaderVals
{
    std::string name;
    std::string originalName;
    std::string iconPath;
    int leaderPickOrder;
    int statsID;
    std::string defaultPlayerSlotFlags;
    bool hasAlpha = false;
    void Load(const std::string& str);
    void Save(const std::string& line, std::string& saveBuildString);
};

struct CivDetails
{
    std::string civ;
    std::string tech;
    int nameID;
    int descriptionID;
    void Load(std::string& str, std::ifstream& Stream);
    void Save(std::string& saveBuildString);
};

struct FlashDetails
{
    int flashID;
    std::string flashImg;
    std::string flashPortrait;
    std::string UIControlBackground;
    void Load(std::string& str, std::ifstream& Stream);
    void Save(std::string& saveBuildString);
};

struct StartingUnit
{
    Vec3 offset;
    std::string buildOther;
    bool doppleOnStart;
    std::string socket;
    void Load(const std::string& str, std::vector<std::string>& strings, std::vector<std::string>& nstrings);
    void Save(std::string& buildSaveString);
};

struct StartingSquad
{
    bool flyIn;
    Vec3 offset;
    std::string unitID;
    void Load(const std::string& str, std::vector<std::string>& strings, std::vector<std::string>& nstrings);
    void Save(std::string& buildSaveString);
};

struct StartingProperties
{
    std::pair<std::string, int> leaderResources[4];
    StartingUnit startUnit;
    std::vector<StartingSquad> startingSquads;
    Vec3 rallyPointOffset;
    void Load(std::string& str, std::ifstream& Stream);
    void Save(std::string& saveBuildString);
};

struct RepairProperties
{
    int repairRate;
    int repairDelay;
    std::pair<std::string, int> repairCost;
    int repairTime;
    void Load(std::string& str, std::ifstream& Stream);
    void Save(std::string& saveBuildString);
};

struct PopDefine
{
    std::string popType;
    int max;
    int current;
};

class LeaderUIHandler;

struct Leader
{
public:
    InitialLeaderVals initialValues;
    CivDetails civDetails;
    FlashDetails flashDetails;
    StartingProperties startProperties;
    RepairProperties repairProperties;
    std::pair<std::string, int> reverseDropCost;
    std::vector<PopDefine> populations;
    bool hasReverseDrop = false;
    Leader()
    {
        for (int i = 0; i < 4; i++)
        {
            startProperties.leaderResources[i].first = LeaderResources[i];
            startProperties.leaderResources[i].second = 0;
        }
    }
    void Load(std::ifstream& file, std::string& line);
    void Save(std::string& saveBuildString);
    void RemoveLeader(LeaderUIHandler* ui);
};